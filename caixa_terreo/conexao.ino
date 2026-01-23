//conexao.ino
#include "variaveis.h"

// =====================================================
// WI-FI
// =====================================================
void inicializarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Conectando ao WiFi");

  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    hasInternet = true;
    modoAP = false;

    Serial.println("\n✅ WiFi conectado!");
    Serial.print("📶 IP: ");
    Serial.println(WiFi.localIP());
  } else {
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
