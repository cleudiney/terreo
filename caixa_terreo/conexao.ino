//conexao.ino
#include "variaveis.h"

// =====================================================
// WI-FI
// =====================================================
static bool esperarConexaoWiFi(unsigned long timeoutMs) {
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < timeoutMs) {
    delay(500);
    Serial.print(".");
  }
  return WiFi.status() == WL_CONNECTED;
}

static void configurarDHCP() {
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
}

static bool tentarIpFixo199(const String& ssid, const String& senha) {
  IPAddress gateway = WiFi.gatewayIP();
  if (gateway[0] == 0 && gateway[1] == 0 && gateway[2] == 0 && gateway[3] == 0) {
    modoIpAtual = "DHCP";
    return false;
  }

  IPAddress ip199(gateway[0], gateway[1], gateway[2], 199);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns1 = gateway;
  IPAddress dns2(8, 8, 8, 8);

  if (WiFi.localIP() == ip199) {
    modoIpAtual = "FIXO .199";
    return true;
  }

  Serial.print("\nTentando IP fixo .199: ");
  Serial.println(ip199);

  WiFi.disconnect(false);
  delay(500);
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(ip199, gateway, subnet, dns1, dns2)) {
    Serial.println("⚠️ Falha ao configurar IP fixo .199");
    modoIpAtual = "DHCP fallback";
    return false;
  }

  WiFi.begin(ssid.c_str(), senha.c_str());

  if (esperarConexaoWiFi(60000UL) && WiFi.localIP() == ip199) {
    modoIpAtual = "FIXO .199";
    Serial.println("\n✅ IP fixo .199 conectado");
    return true;
  }

  Serial.println("\n⚠️ IP fixo .199 falhou, voltando para DHCP");
  WiFi.disconnect(false);
  delay(500);
  configurarDHCP();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), senha.c_str());

  if (esperarConexaoWiFi(20000UL)) {
    modoIpAtual = "DHCP fallback";
    Serial.println("\n✅ DHCP fallback conectado");
    return false;
  }

  modoIpAtual = "Sem conexao";
  return false;
}

bool conectarWiFiComIp199(const String& ssid, const String& senha) {
  modoIpAtual = "DHCP";
  WiFi.mode(WIFI_STA);
  configurarDHCP();
  WiFi.begin(ssid.c_str(), senha.c_str());

  if (!esperarConexaoWiFi(20000UL)) {
    hasInternet = false;
    modoIpAtual = "Sem conexao";
    return false;
  }

  hasInternet = true;
  modoAP = false;
  tentarIpFixo199(ssid, senha);
  return WiFi.status() == WL_CONNECTED;
}

void inicializarWiFi() {
  String ssidSalvo;
  String senhaSalva;
  bool temRedeSalva = carregarCredenciaisWiFiEEPROM(ssidSalvo, senhaSalva);
  String ssidUsado = temRedeSalva ? ssidSalvo : String(SSID);
  String senhaUsada = temRedeSalva ? senhaSalva : String(PASSWORD);

  Serial.print("Conectando ao WiFi");
  if (temRedeSalva) {
    Serial.print(" salvo: ");
    Serial.print(ssidSalvo);
  }

  if (conectarWiFiComIp199(ssidUsado, senhaUsada)) {
    hasInternet = true;
    modoAP = false;

    Serial.println("\n✅ WiFi conectado!");
    Serial.print("📶 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    if (temRedeSalva) {
      Serial.println("\n⚠️ Falha na rede salva, tentando rede padrao...");

      if (conectarWiFiComIp199(String(SSID), String(PASSWORD))) {
        hasInternet = true;
        modoAP = false;

        Serial.println("\n✅ WiFi padrao conectado!");
        Serial.print("📶 IP: ");
        Serial.println(WiFi.localIP());
        return;
      }
    }

    iniciarModoAP();
  }
}

void iniciarModoAP() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  modoAP = true;
  hasInternet = false;

  Serial.println("\n📡 Modo AP ativado");
  Serial.print("📶 IP AP: ");
  Serial.println(WiFi.softAPIP());
}

void verificarConexao() {
  static unsigned long ultimaVerificacao = 0;
  const unsigned long INTERVALO_VERIFICACAO = 30000;
  
  if (millis() - ultimaVerificacao > INTERVALO_VERIFICACAO) {
    ultimaVerificacao = millis();
    
    if (!modoAP && WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ WiFi desconectado, tentando reconectar...");
      WiFi.reconnect();
      
      delay(10000);
      if (WiFi.status() != WL_CONNECTED) {
        iniciarModoAP();
      }
    }
  }
}

// =====================================================
// NTP / TEMPO
// =====================================================
void inicializarTimeClient() {
  if (!hasInternet) return;

  timeClient.begin();

  if (!timeClient.forceUpdate()) {
    Serial.println("⚠️ Falha na sincronização NTP");
  } else {
    Serial.print("✅ Hora sincronizada: ");
    Serial.println(getDataHoraAtual());
  }
}

String getDataHoraAtual() {
  if (hasInternet) {
    timeClient.update();
    return timeClient.getFormattedTime(); // HH:MM:SS
  }
  return "00:00:00";
}

String getDataAtual() {
  // Versão simplificada - biblioteca antiga não tem getFormattedDate()
  if (hasInternet) {
    timeClient.update();
    
    // Extrair data manualmente da epoch time
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    
    int dia = ptm->tm_mday;
    int mes = ptm->tm_mon + 1;
    int ano = ptm->tm_year + 1900;
    
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d", ano, mes, dia);
    return String(buffer);
  }
  return "1970-01-01";
}

String getHoraAtual() {
  if (hasInternet) {
    timeClient.update();
    return timeClient.getFormattedTime().substring(0, 5); // HH:MM
  }
  return "00:00";
}

int getHoraInt() {
  if (hasInternet) {
    timeClient.update();
    return timeClient.getHours();
  }
  return 0;
}

// Função auxiliar para obter timestamp Unix
unsigned long getUnixTime() {
  if (hasInternet) {
    timeClient.update();
    return timeClient.getEpochTime();
  }
  return 0;
}
