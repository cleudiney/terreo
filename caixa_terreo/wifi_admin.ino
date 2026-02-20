#include "variaveis.h"

static String jsonEscape(const String& s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '\\' || c == '"') {
      out += '\\';
      out += c;
    } else {
      out += c;
    }
  }
  return out;
}

static String extrairCampoJson(const String& body, const String& chave) {
  String chaveBusca = "\"" + chave + "\"";
  int pChave = body.indexOf(chaveBusca);
  if (pChave < 0) return "";

  int pDoisPontos = body.indexOf(':', pChave);
  if (pDoisPontos < 0) return "";

  int pAspasIni = body.indexOf('"', pDoisPontos + 1);
  if (pAspasIni < 0) return "";

  int pAspasFim = body.indexOf('"', pAspasIni + 1);
  if (pAspasFim < 0) return "";

  return body.substring(pAspasIni + 1, pAspasFim);
}

static void salvarRedeNoArquivo(const String& ssid, const String& senha) {
  if (!SPIFFS.exists(ARQ_REDES)) {
    File f = SPIFFS.open(ARQ_REDES, FILE_WRITE);
    if (f) f.close();
  }

  String dataHora = getDataHoraAtual();
  String linha = dataHora + ";" + ssid + ";" + senha + "\n";

  File f = SPIFFS.open(ARQ_REDES, FILE_APPEND);
  if (!f) return;
  f.print(linha);
  f.close();
}

void apiWifiScan() {
  if (!autenticado || !sessaoAtiva || nivelAcessoLogadoEnum < ACESSO_SUBSINDICO) {
    server.send(403, "application/json", "{\"erro\":\"acesso negado\"}");
    return;
  }

  int total = WiFi.scanNetworks(false, true);
  if (total < 0) {
    server.send(200, "application/json", "[]");
    return;
  }

  String json = "[";
  for (int i = 0; i < total; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.isEmpty()) continue;

    if (json.length() > 1) json += ",";
    json += "{";
    json += "\"ssid\":\"" + jsonEscape(ssid) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i));
    json += "}";
  }
  json += "]";

  server.send(200, "application/json", json);
}

void apiWifiConnect() {
  if (!autenticado || !sessaoAtiva || nivelAcessoLogadoEnum < ACESSO_SUBSINDICO) {
    server.send(403, "application/json", "{\"ok\":false,\"erro\":\"acesso negado\"}");
    return;
  }

  String body = server.arg("plain");
  String ssid = extrairCampoJson(body, "ssid");
  String senha = extrairCampoJson(body, "senha");

  if (ssid.isEmpty()) {
    server.send(400, "application/json", "{\"ok\":false,\"erro\":\"ssid invalido\"}");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), senha.c_str());

  unsigned long inicio = millis();
  const unsigned long timeout = 20000UL;
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < timeout) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    hasInternet = true;
    modoAP = false;

    salvarRedeNoArquivo(ssid, senha);

    String json = "{\"ok\":true,\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    server.send(200, "application/json", json);
    return;
  }

  server.send(200, "application/json", "{\"ok\":false,\"erro\":\"falha ao conectar\"}");
}
