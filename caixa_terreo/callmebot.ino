// CallMeBot.ino
#include "variaveis.h"
#include <WiFi.h>
#include <HTTPClient.h>

// montar mensagem padrão
String getCabecalhoWhatsApp() {
  String txt = String(SISTEMA_NOME) + "\n";
  txt += "----------------------------------\n";
  return txt;
}

// Função auxiliar para obter hora formatada
String getHoraCallMeBot() {
  if (hasInternet && WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    return timeClient.getFormattedTime();
  } else {
    // Retorna hora baseada em millis() como fallback
    static unsigned long startMillis = millis();
    unsigned long segundos = (millis() - startMillis) / 1000;
    int horas = (segundos / 3600) % 24;
    int minutos = (segundos % 3600) / 60;
    int segs = segundos % 60;
    
    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", horas, minutos, segs);
    return String(buffer);
  }
}

// Propaganda da empresa
String getPropaganda() {
  String prop = "\n\n💼 *Desenvolvido por:*\n";
  prop += "🤖 *Robótica na Lata*\n";
  prop += "🎨 _Uma empresa de ARTe_\n";
  prop += "⚡ _Automação Robótica e TEcnologia_\n";
  prop += "📞 31 99916-9087";
  return prop;
}

void enviarWhatsappTodos(const String& mensagem) {
  // Verificar se tem internet antes de enviar
  if (!hasInternet || WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Sem internet - WhatsApp não enviado");
    return;
  }

  // Adicionar propaganda ao final de cada mensagem
  String mensagemCompleta = mensagem + getPropaganda();
  
  Serial.println("📤 Enviando WhatsApp para todos...");
  enviarWhatsappFormatado(CALLMEBOT_TEL1, CALLMEBOT_APIKEY1, mensagemCompleta);
  delay(2000);
  enviarWhatsappFormatado(CALLMEBOT_TEL2, CALLMEBOT_APIKEY2, mensagemCompleta);
}

bool enviarWhatsappFormatado(const String& telefone, const String& apikey, const String& mensagem) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi não conectado - WhatsApp não enviado");
    return false;
  }

  WiFiClient client;
  HTTPClient http;

  // Encoding SIMPLES - apenas o necessário
  String mensagemCodificada = "";
  
  for (int i = 0; i < mensagem.length(); i++) {
    unsigned char c = mensagem[i];
    
    // Caracteres ASCII seguros - manter como estão
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
        c == '-' || c == '_' || c == '.' || c == '~' || c == '*' || 
        c == ':' || c == '/' || c == '(' || c == ')' || c == '!' ||
        c == '@' || c == '#' || c == '$' || c == '^' || c == '=' ||
        c == '+' || c == '?' || c == ',' || c == ';' || c == '[' ||
        c == ']' || c == '{' || c == '}' || c == '|' || c == '\\' ||
        c == '<' || c == '>' || c == '\'' || c == '%') {
      mensagemCodificada += (char)c;
    }
    else if (c == ' ') {
      mensagemCodificada += "%20";
    }
    else if (c == '\n') {
      mensagemCodificada += "%0A";  // Quebra de linha
    }
    else if (c == '&') {
      mensagemCodificada += "%26";
    }
    else {
      // EMOJIS e caracteres Unicode - manter como estão (UTF-8)
      mensagemCodificada += (char)c;
    }
  }

  // Construir URL
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + telefone + 
               "&apikey=" + apikey + 
               "&text=" + mensagemCodificada;
  
  Serial.println("=== ENVIANDO WHATSAPP ===");
  Serial.print("📱 Telefone: "); Serial.println(telefone);
  Serial.print("💬 Mensagem: "); Serial.println(mensagem);
  
  http.begin(client, url);
  int httpCode = http.GET();
  
  bool sucesso = false;
  
  if (httpCode > 0) {
    Serial.printf("📡 Código HTTP: %d\n", httpCode);
    
    if (httpCode == 200) {
      sucesso = true;
      Serial.println("✅ WhatsApp enviado com sucesso!");
    } else {
      String resposta = http.getString();
      Serial.printf("❌ Erro HTTP: %d\n", httpCode);
      Serial.print("📄 Resposta: "); Serial.println(resposta);
    }
  } else {
    Serial.printf("❌ Falha na conexão: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
  return sucesso;
}

void enviarMensagemBoot() {
  String msg = "🚀 *SISTEMA INICIALIZADO*\n";
  msg += "💧 Monitor Caixa d'Agua - Edifício Aquários\n";
  msg += "⏰ " + getHoraCallMeBot() + "\n";
  msg += "📡 WiFi: " + String(WiFi.SSID()) + "\n";
  msg += "🌐 IP Local: http://" + WiFi.localIP().toString() + "\n"; // ✅ Sem porta
  msg += "🌍 DuckDNS: http://edificioaquarios.duckdns.org:3000\n"; // ✅ Sem porta
  msg += "✅ Pronto para operar";
  
  Serial.println("=== ENVIANDO MENSAGEM DE BOOT ===");
  enviarWhatsappTodos(msg);
}


void enviarMensagemStatus() {
  float nivel = calcularNivelAgua();
  float volume = calcularVolume();
  
  String msg = "📊 *RELATORIO PERIODICO*\n";
  msg += "⏰ " + getHoraCallMeBot() + "\n";
  msg += "💧 Nivel: " + String(nivel, 1) + "%\n";
  msg += "💦 Volume: " + String(volume, 0) + " L\n";
  msg += "🔧 Estado: " + getEstadoString() + "\n";
  msg += "🌐 IP: " + WiFi.localIP().toString();
  
  Serial.println("=== ENVIANDO STATUS PERIODICO ===");
  enviarWhatsappTodos(msg);
}

void enviarAlertaEmergencia() {
  float nivel = calcularNivelAgua();
  
  String msg = "🚨 *ALERTA DE EMERGENCIA* 🚨\n";
  msg += "⏰ " + getHoraCallMeBot() + "\n";
  msg += "💧 Nivel CRITICO: " + String(nivel, 1) + "%\n";
  msg += "⚠️ Nivel abaixo de 20%\n";
  msg += "🚒 Modo emergencia ativado";
  
  Serial.println("=== ENVIANDO ALERTA DE EMERGENCIA ===");
  enviarWhatsappTodos(msg);
}

void enviarAlertaVazamentoCritico(float vazao) {
  float nivel = calcularNivelAgua();
  
  String msg = "💧 *ALERTA DE VAZAMENTO* 💧\n";
  msg += "⏰ " + getHoraCallMeBot() + "\n";
  msg += "💧 Nivel: " + String(nivel, 1) + "%\n";
  msg += "📉 Vazao: " + String(vazao, 1) + " L/min\n";
  msg += "🚨 ESVAZIAMENTO RAPIDO\n";
  msg += "⚠️ Acima de 60 L/min\n";
  msg += "🔍 Verificar possiveis vazamentos";
  
  Serial.println("=== ENVIANDO ALERTA DE VAZAMENTO ===");
  enviarWhatsappTodos(msg);
}

void enviarRelatorioDiario() {
  float nivel = calcularNivelAgua();
  
  String msg = "📅 *RELATORIO DIARIO - 23:30*\n";
  msg += "💧 Nivel final: " + String(nivel, 1) + "%\n";
  msg += "🔧 Status: " + getEstadoString() + "\n";
  msg += "🌜 Boa noite!";
  
  Serial.println("=== ENVIANDO RELATORIO DIARIO ===");
  enviarWhatsappTodos(msg);
}
