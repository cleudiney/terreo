// =====================================================
// CallMeBot.ino — PADRÃO OFICIAL DO SISTEMA
// =====================================================
#include "variaveis.h"
#include <WiFi.h>
#include <HTTPClient.h>

// =====================================================
// CABEÇALHO PADRÃO DO SISTEMA
// =====================================================
String getCabecalhoWhatsApp() {
  String txt = String(SISTEMA_NOME) + "\n";
  txt += "----------------------------------\n";
  return txt;
}

// =====================================================
// HORA FORMATADA
// =====================================================
String getHoraCallMeBot() {
  if (hasInternet && WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    return timeClient.getFormattedTime();
  }

  unsigned long segundos = millis() / 1000;
  int h = (segundos / 3600) % 24;
  int m = (segundos % 3600) / 60;
  int s = segundos % 60;

  char buf[9];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
  return String(buf);
}

// =====================================================
// ASSINATURA DA EMPRESA
// =====================================================
String getPropaganda() {
  String p = "\n\n💼 *Desenvolvido por:*\n";
  p += "🤖 *Robótica na Lata*\n";
  p += "🎨 _Uma empresa de ARTe_\n";
  p += "⚡ _Automação Robótica e TEcnologia_\n";
  p += "📞 31 99916-9087";
  return p;
}

// =====================================================
// ENVIO PARA TODOS OS TELEFONES
// =====================================================
void enviarWhatsappTodos(const String& mensagem) {

  if (!hasInternet || WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Sem internet — WhatsApp não enviado");
    return;
  }

  String msgFinal = mensagem + getPropaganda();

  enviarWhatsappFormatado(CALLMEBOT_TEL1, CALLMEBOT_APIKEY1, msgFinal);
  //delay(1500);
  unsigned long inicio = millis();
    
    while (millis() - inicio < 1500) {
        yield();  // mantém WiFi, watchdog e RTOS vivos
    }
  enviarWhatsappFormatado(CALLMEBOT_TEL2, CALLMEBOT_APIKEY2, msgFinal);
  //delay(1500);
  inicio = millis();
    
    while (millis() - inicio < 1500) {
        yield();  // mantém WiFi, watchdog e RTOS vivos
    }
  enviarWhatsappFormatado(CALLMEBOT_TEL3, CALLMEBOT_APIKEY3, msgFinal);
    //delay(1500);
   inicio = millis();
    
    while (millis() - inicio < 1500) {
        yield();  // mantém WiFi, watchdog e RTOS vivos
    }
  Serial.println("WhatsApp enviado");

}

// =====================================================
// ENVIO HTTP
// =====================================================
bool enviarWhatsappFormatado(
  const String& telefone,
  const String& apikey,
  const String& mensagem
) {
  WiFiClient client;
  HTTPClient http;

  String texto = "";

  for (int i = 0; i < mensagem.length(); i++) {
    char c = mensagem[i];
    if (c == ' ') texto += "%20";
    else if (c == '\n') texto += "%0A";
    else if (c == '&') texto += "%26";
    else texto += c;
  }

  String url =
    "http://api.callmebot.com/whatsapp.php?phone=" +
    telefone + "&apikey=" + apikey + "&text=" + texto;

  http.begin(client, url);
  int code = http.GET();
  http.end();

  Serial.printf("📤 WhatsApp [%s] → HTTP %d\n",
                telefone.c_str(), code);

  return (code == 200);
}

// =====================================================
// PADRÃO GLOBAL DE MENSAGEM
// =====================================================
String montarMensagem(
  const String& emoji,
  const String& titulo,
  const String& corpo
) {
  String msg = getCabecalhoWhatsApp();
  msg += emoji + " *" + titulo + "*\n";
  msg += "⏰ " + getHoraCallMeBot() + "\n\n";
  msg += corpo;
  return msg;
}

// =====================================================
// ===================== TIPOS ==========================
// =====================================================

void enviarEvento(const String& texto) {
  enviarWhatsappTodos(
    montarMensagem("ℹ️", "EVENTO", texto)
  );
}

void enviarUrgente(const String& texto) {
  enviarWhatsappTodos(
    montarMensagem("⚠️", "URGENTE", texto)
  );
}

void enviarCritico(const String& texto) {
  enviarWhatsappTodos(
    montarMensagem("🚨", "CRÍTICO", texto)
  );
}

void enviarResolvido(const String& texto) {
  enviarWhatsappTodos(
    montarMensagem("✅", "RESOLVIDO", texto)
  );
}

// =====================================================
// ===================== BOOT ===========================
// =====================================================
void enviarMensagemBoot() {

  String corpo = "";
  corpo += "Sistema iniciado ou reiniciado\n";
  corpo += "📡 WiFi: " + WiFi.SSID() + "\n";

  if (modoAP)
    corpo += "🌐 IP AP: " + WiFi.softAPIP().toString() + "\n";
  else
    corpo += "🌐 IP Local: " + WiFi.localIP().toString() + "\n";

  corpo += "🌍 DuckDNS: http://" + DUCKDNS_DOMAIN + ".duckdns.org\n";
  corpo += "📊 Primeira leitura em andamento";

  enviarWhatsappTodos(
    montarMensagem("🚀", "SISTEMA INICIALIZADO", corpo)
  );
}

// =====================================================
// STATUS PERIÓDICO
// =====================================================
void enviarMensagemStatus() {

  float nivel = calcularNivelAgua();
  float volume = calcularVolume();

  String corpo = "";
  corpo += "💧 Nível: " + String(nivel, 1) + "%\n";
  corpo += "💦 Volume: " + String(volume, 0) + " L\n";
  corpo += "🔧 Estado: " + getEstadoString() + "\n";
  corpo += "📡 Vazão entrada: ";
  corpo += estadoAtual.vazaoEntrada ? "SIM" : "NÃO";

  enviarWhatsappTodos(
    montarMensagem("📊", "STATUS DO SISTEMA", corpo)
  );
}

// =====================================================
// ALERTAS ESPECÍFICOS
// =====================================================
void enviarAlertaEmergencia() {
  enviarCritico("Nível abaixo de 20%\nModo emergência ativado");
}

void enviarAlertaVazamentoCritico(float vazao) {
  String corpo = "";
  corpo += "📉 Vazão detectada: " + String(vazao, 1) + " L/min\n";
  corpo += "⚠️ Possível vazamento\n";
  corpo += "🔍 Verificar imediatamente";

  enviarCritico(corpo);
}
