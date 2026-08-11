#include "variaveis.h"

static const int WIFI_EEPROM_SIZE = 128;
static const int WIFI_EEPROM_MAGIC_ADDR = 0;
static const int WIFI_EEPROM_SSID_LEN_ADDR = 4;
static const int WIFI_EEPROM_SENHA_LEN_ADDR = 5;
static const int WIFI_EEPROM_SSID_ADDR = 6;
static const int WIFI_EEPROM_SENHA_ADDR = 39;
static const int WIFI_EEPROM_MAX_SSID = 32;
static const int WIFI_EEPROM_MAX_SENHA = 64;
static const char WIFI_EEPROM_MAGIC[4] = {'W', 'F', 'I', '1'};

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

bool salvarCredenciaisWiFiEEPROM(const String& ssid, const String& senha) {
  if (ssid.isEmpty() || ssid.length() > WIFI_EEPROM_MAX_SSID ||
      senha.length() > WIFI_EEPROM_MAX_SENHA) {
    return false;
  }

  if (!EEPROM.begin(WIFI_EEPROM_SIZE)) {
    Serial.println("❌ Falha ao iniciar EEPROM para salvar WiFi");
    return false;
  }

  for (int i = 0; i < 4; i++) {
    EEPROM.write(WIFI_EEPROM_MAGIC_ADDR + i, WIFI_EEPROM_MAGIC[i]);
  }

  EEPROM.write(WIFI_EEPROM_SSID_LEN_ADDR, ssid.length());
  EEPROM.write(WIFI_EEPROM_SENHA_LEN_ADDR, senha.length());

  for (int i = 0; i <= WIFI_EEPROM_MAX_SSID; i++) {
    char c = (i < ssid.length()) ? ssid[i] : '\0';
    EEPROM.write(WIFI_EEPROM_SSID_ADDR + i, c);
  }

  for (int i = 0; i <= WIFI_EEPROM_MAX_SENHA; i++) {
    char c = (i < senha.length()) ? senha[i] : '\0';
    EEPROM.write(WIFI_EEPROM_SENHA_ADDR + i, c);
  }

  bool ok = EEPROM.commit();
  EEPROM.end();

  if (ok) {
    Serial.println("✅ Credenciais WiFi salvas na EEPROM");
  } else {
    Serial.println("❌ Falha ao gravar credenciais WiFi na EEPROM");
  }

  return ok;
}

bool carregarCredenciaisWiFiEEPROM(String& ssid, String& senha) {
  ssid = "";
  senha = "";

  if (!EEPROM.begin(WIFI_EEPROM_SIZE)) {
    Serial.println("❌ Falha ao iniciar EEPROM para ler WiFi");
    return false;
  }

  for (int i = 0; i < 4; i++) {
    if (EEPROM.read(WIFI_EEPROM_MAGIC_ADDR + i) != WIFI_EEPROM_MAGIC[i]) {
      EEPROM.end();
      return false;
    }
  }

  int ssidLen = EEPROM.read(WIFI_EEPROM_SSID_LEN_ADDR);
  int senhaLen = EEPROM.read(WIFI_EEPROM_SENHA_LEN_ADDR);

  if (ssidLen <= 0 || ssidLen > WIFI_EEPROM_MAX_SSID ||
      senhaLen < 0 || senhaLen > WIFI_EEPROM_MAX_SENHA) {
    EEPROM.end();
    return false;
  }

  for (int i = 0; i < ssidLen; i++) {
    ssid += (char)EEPROM.read(WIFI_EEPROM_SSID_ADDR + i);
  }

  for (int i = 0; i < senhaLen; i++) {
    senha += (char)EEPROM.read(WIFI_EEPROM_SENHA_ADDR + i);
  }

  EEPROM.end();
  return !ssid.isEmpty();
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

  if (conectarWiFiComIp199(ssid, senha)) {
    hasInternet = true;
    modoAP = false;

    bool salvoEEPROM = salvarCredenciaisWiFiEEPROM(ssid, senha);
    salvarRedeNoArquivo(ssid, senha);

    String json = "{\"ok\":true,\"ip\":\"" + WiFi.localIP().toString() +
                  "\",\"modoIp\":\"" + modoIpAtual +
                  "\",\"gateway\":\"" + WiFi.gatewayIP().toString() +
                  "\",\"eeprom\":" + String(salvoEEPROM ? "true" : "false") + "}";
    server.send(200, "application/json", json);
    return;
  }

  server.send(200, "application/json", "{\"ok\":false,\"erro\":\"falha ao conectar\"}");
}
