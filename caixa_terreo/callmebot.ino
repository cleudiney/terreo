// =====================================================
// callmebot.ino - Sistema NTFY (substitui CallMeBot antigo)
// =====================================================
// Mantido com este nome para não mexer nas outras abas do projeto.
// O CallMeBot foi substituído pelo ntfy com dois tópicos separados.
//
// No celular:
// 1) Instale o app ntfy.
// 2) Assine os tópicos definidos abaixo.
// 3) Todos os celulares inscritos nesse mesmo tópico receberão os alertas.
// =====================================================

#include "variaveis.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ========== TÓPICOS NTFY - Duas chaves separadas ==========
// IMPORTANTE: trate estes nomes como senhas, pois quem souber os tópicos
// poderá publicar/assinar enquanto eles estiverem públicos no ntfy.sh.
const String NTFY_TOPIC_VER = "caixa_copasa_aquarios_ver";           // Alertas e eventos críticos
const String NTFY_TOPIC_MEDICAO = "caixa_copasa_aquarios_medicao";   // Medições e relatórios

// Serviço público do ntfy.
const String NTFY_BASE_URL = "https://ntfy.sh/";

// =====================================================
// URL DE ACESSO WEB
// =====================================================
String getUrlAcessoWeb() {
  if (modoAP) {
    return "http://" + WiFi.softAPIP().toString() + ":" + String(WEB_SERVER_PORT);
  }
  return "http://" + WiFi.localIP().toString() + ":" + String(WEB_SERVER_PORT);
}

String getUrlDuckDNS() {
  return "http://" + DUCKDNS_DOMAIN + ".duckdns.org:" + String(WEB_SERVER_PORT);
}

String getRodapeAcesso() {
  String rodape = "\n\n🌐 *Acesso ao sistema:*\n";
  rodape += "Local: " + getUrlAcessoWeb() + "\n";
  rodape += "DuckDNS: " + getUrlDuckDNS();
  return rodape;
}

// =====================================================
// PROPAGANDA DA EMPRESA
// =====================================================
String getPropaganda() {
  String prop = "\n\n💼 *Desenvolvido por:*\n";
  prop += "🤖 *Robótica na Lata*\n";
  prop += "🎨 _Uma empresa de ARTe_\n";
  prop += "⚡ _Automação Robótica e TEcnologia_\n";
  prop += "📞 31 99916-9087";
  return prop;
}

// =====================================================
// ENVIO NTFY COM ROTA DINÂMICA
// =====================================================
// Envia uma publicação para o tópico ntfy com rota dinâmica.
// Um único POST entrega a mensagem para todos os celulares inscritos.
bool enviarNtfy(const String& mensagem, const String& topico) {
  if (!hasInternet || WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Sem internet - notificação ntfy não enviada");
    return false;
  }

  // Define tópico padrão se não informado
  String topicoUsado = (topico.length() > 0) ? topico : NTFY_TOPIC_MEDICAO;

  WiFiClientSecure client;
  client.setInsecure();  // Evita precisar manter certificado raiz no ESP32.

  HTTPClient http;
  String url = NTFY_BASE_URL + topicoUsado;

  if (!http.begin(client, url)) {
    Serial.println("❌ Falha ao iniciar conexão com ntfy");
    return false;
  }

  http.setTimeout(10000);
  http.addHeader("Content-Type", "text/plain; charset=utf-8");
  http.addHeader("Title", "Caixa d'Agua - COPASA Aquarios");
  http.addHeader("Markdown", "yes");
  http.addHeader("Click", getUrlDuckDNS());

  // Alertas críticos recebem prioridade maior no celular.
  bool urgente =
      mensagem.indexOf("ALERTA") >= 0 ||
      mensagem.indexOf("EMERGENCIA") >= 0 ||
      mensagem.indexOf("EMERGÊNCIA") >= 0 ||
      mensagem.indexOf("VAZAMENTO") >= 0 ||
      mensagem.indexOf("CRITICO") >= 0 ||
      mensagem.indexOf("CRÍTICO") >= 0;

  if (urgente) {
    http.addHeader("Priority", "urgent");
    http.addHeader("Tags", "warning");
  } else {
    http.addHeader("Priority", "default");
    http.addHeader("Tags", "droplet");
  }

  Serial.printf("=== ENVIANDO NOTIFICACAO NTFY [%s] ===\n", topicoUsado.c_str());
  Serial.print("💬 Mensagem: ");
  Serial.println(mensagem);

  int httpCode = http.POST(mensagem);
  bool sucesso = false;

  if (httpCode > 0) {
    Serial.printf("📡 Código HTTP ntfy: %d\n", httpCode);

    if (httpCode >= 200 && httpCode < 300) {
      sucesso = true;
      Serial.println("✅ Notificação ntfy enviada com sucesso!");
    } else {
      String resposta = http.getString();
      Serial.printf("❌ Erro HTTP ntfy: %d\n", httpCode);
      Serial.print("📄 Resposta: ");
      Serial.println(resposta);
    }
  } else {
    Serial.printf("❌ Falha na conexão ntfy: %s\n",
                  http.errorToString(httpCode).c_str());
  }

  http.end();
  return sucesso;
}

// =====================================================
// INTERFACE COMPATÍVEL COM CÓDIGO LEGADO
// =====================================================
// Nome mantido de propósito.
// Assim alertas.ino, relatorios.ino e demais abas NÃO precisam ser alterados.
void enviarWhatsappTodos(const String& mensagem, const String& topico) {
  String mensagemCompleta = mensagem + getRodapeAcesso() + getPropaganda();

  Serial.println("📤 Enviando notificação para todos via ntfy...");
  enviarNtfy(mensagemCompleta, topico);
}

void enviarEvento(const String& texto) {
  if (podeEnviarNotificacao("evento", texto, INTERVALO_REPETICAO_INFO_MS)) {
    enviarWhatsappTodos("ℹ️ EVENTO\n" + texto, NTFY_TOPIC_MEDICAO);
  }
}

void enviarUrgente(const String& texto) {
  if (podeEnviarNotificacao("urgente", texto, INTERVALO_REPETICAO_URGENTE_MS)) {
    enviarWhatsappTodos("⚠️ URGENTE\n" + texto, NTFY_TOPIC_VER);
  }
}

void enviarCritico(const String& texto) {
  if (podeEnviarNotificacao("critica", texto, INTERVALO_REPETICAO_CRITICA_MS)) {
    enviarWhatsappTodos("🚨 CRÍTICO\n" + texto, NTFY_TOPIC_VER);
  }
}

void enviarResolvido(const String& texto) {
  if (podeEnviarNotificacao("resolvido", texto, INTERVALO_REPETICAO_INFO_MS)) {
    enviarWhatsappTodos("✅ RESOLVIDO\n" + texto, NTFY_TOPIC_MEDICAO);
  }
}

void enviarEventoPeriodico(const String& texto) {
  enviarWhatsappTodos("ℹ️ EVENTO PERIODICO\n" + texto, NTFY_TOPIC_MEDICAO);
}

String montarMensagemMedicaoAtual(bool leituraForcada) {
  if (leituraForcada) {
    lerTodosSensores();
  }

  float nivel = estadoAtual.nivelPercentual;
  float volume = calcularVolume();
  float vazaoLh = vazaoCalculada * 60.0f;

  String msg = leituraForcada ? "📏 *MEDICAO SOLICITADA*\n" : "👁️ *STATUS ATUAL*\n";
  msg += "⏰ " + getHoraAtual() + "\n";
  msg += "💧 Nivel: " + String(nivel, 1) + "%\n";
  msg += "📐 Altura: " + String(estadoAtual.nivelCm, 1) + " cm\n";
  msg += "💦 Volume: " + String(volume, 0) + " L / " + String(VOLUME_TOTAL_L, 0) + " L\n";
  msg += "🚰 Vazao entrada: ";
  msg += estadoAtual.vazaoEntrada ? "SIM\n" : "NAO\n";
  msg += "📊 Vazao calc.: " + String(vazaoLh, 1) + " L/h\n";
  msg += "🔧 Estado: " + getEstadoString();

  return msg;
}

String extrairCampoJsonString(const String& json, const String& campo) {
  String chave = "\"" + campo + "\":\"";
  int inicio = json.indexOf(chave);
  if (inicio < 0) return "";

  inicio += chave.length();
  String valor = "";
  bool escape = false;

  for (int i = inicio; i < json.length(); i++) {
    char c = json[i];

    if (escape) {
      valor += c;
      escape = false;
      continue;
    }

    if (c == '\\') {
      escape = true;
      continue;
    }

    if (c == '"') break;
    valor += c;
  }

  return valor;
}

void processarComandoNtfy(const String& comando) {
  String cmd = comando;
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "medir") {
    enviarWhatsappTodos(montarMensagemMedicaoAtual(true), NTFY_TOPIC_MEDICAO);
    return;
  }

  if (cmd == "ver") {
    enviarWhatsappTodos(montarMensagemMedicaoAtual(false), NTFY_TOPIC_MEDICAO);
    return;
  }
}

void verificarComandosNtfy() {
  static unsigned long ultimaConsulta = 0;
  static String ultimoIdProcessado = "";
  const unsigned long INTERVALO_CONSULTA_MS = 15000UL;

  if (!hasInternet || WiFi.status() != WL_CONNECTED) return;
  if (millis() - ultimaConsulta < INTERVALO_CONSULTA_MS) return;
  ultimaConsulta = millis();

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = NTFY_BASE_URL + NTFY_TOPIC_MEDICAO + "/json?poll=1&since=latest";

  if (!http.begin(client, url)) {
    Serial.println("❌ Falha ao consultar comandos ntfy");
    return;
  }

  http.setTimeout(7000);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String resposta = http.getString();
    int inicioLinha = 0;

    while (inicioLinha < resposta.length()) {
      int fimLinha = resposta.indexOf('\n', inicioLinha);
      if (fimLinha < 0) fimLinha = resposta.length();

      String linha = resposta.substring(inicioLinha, fimLinha);
      linha.trim();

      if (linha.indexOf("\"event\":\"message\"") >= 0) {
        String id = extrairCampoJsonString(linha, "id");
        String mensagem = extrairCampoJsonString(linha, "message");

        if (id.length() > 0 && id != ultimoIdProcessado) {
          ultimoIdProcessado = id;
          processarComandoNtfy(mensagem);
        }
      }

      inicioLinha = fimLinha + 1;
    }
  } else if (httpCode > 0) {
    Serial.printf("⚠️ Consulta ntfy retornou HTTP %d\n", httpCode);
  } else {
    Serial.printf("⚠️ Falha ao consultar ntfy: %s\n",
                  http.errorToString(httpCode).c_str());
  }

  http.end();
}

// =====================================================
// MENSAGENS ESPECÍFICAS DO SISTEMA
// =====================================================

void enviarMensagemBoot() {
  lerTodosSensores();
  delay(250);
  lerTodosSensores();

  float nivel = calcularNivelAgua();
  float volume = calcularVolume();

  String msg = "🚀 *SISTEMA INICIALIZADO*\n";
  msg += "💧 Monitor Caixa d'Agua - COPASA Aquários\n";
  msg += "⏰ " + getHoraAtual() + "\n";
  msg += "📡 WiFi: " + String(WiFi.SSID()) + "\n";
  msg += "🌐 Modo IP: " + modoIpAtual + "\n";
  msg += "🚪 Gateway: " + WiFi.gatewayIP().toString() + "\n\n";
  msg += "💧 *Dados iniciais da caixa*\n";
  msg += "Nivel: " + String(nivel, 1) + "%\n";
  msg += "Altura: " + String(estadoAtual.nivelCm, 1) + " cm\n";
  msg += "Volume: " + String(volume, 0) + " L\n";
  msg += "Vazao entrada: ";
  msg += estadoAtual.vazaoEntrada ? "SIM\n" : "NAO\n";
  msg += "Estado: " + getEstadoString() + "\n\n";
  msg += "✅ Pronto para operar";

  Serial.println("=== ENVIANDO MENSAGEM DE BOOT ===");
  // Boot vai para o tópico de visualização (alertas/eventos críticos)
  enviarWhatsappTodos(msg, NTFY_TOPIC_VER);
}

void enviarMensagemStatus() {
  float nivel = calcularNivelAgua();
  float volume = calcularVolume();

  String msg = "📊 *RELATORIO PERIODICO*\n";
  msg += "⏰ " + getHoraAtual() + "\n";
  msg += "💧 Nivel: " + String(nivel, 1) + "%\n";
  msg += "💦 Volume: " + String(volume, 0) + " L\n";
  msg += "🔧 Estado: " + getEstadoString();

  Serial.println("=== ENVIANDO STATUS PERIODICO ===");
  // Status vai para o tópico de medição (dados periódicos)
  enviarEventoPeriodico(msg);
}

void enviarAlertaEmergencia() {
  float nivel = calcularNivelAgua();

  String msg = "🚨 *ALERTA DE EMERGENCIA* 🚨\n";
  msg += "⏰ " + getHoraAtual() + "\n";
  msg += "💧 Nivel CRITICO: " + String(nivel, 1) + "%\n";
  msg += "⚠️ Nivel abaixo de 20%\n";
  msg += "🚒 Modo emergencia ativado";

  Serial.println("=== ENVIANDO ALERTA DE EMERGENCIA ===");
  // Alertas críticos vão para o tópico de visualização
  enviarCritico(msg);
}

void enviarAlertaVazamentoCritico(float vazao) {
  float nivel = calcularNivelAgua();

  String msg = "💧 *ALERTA DE VAZAMENTO* 💧\n";
  msg += "⏰ " + getHoraAtual() + "\n";
  msg += "💧 Nivel: " + String(nivel, 1) + "%\n";
  msg += "📉 Vazao: " + String(vazao, 1) + " L/min\n";
  msg += "🚨 ESVAZIAMENTO RAPIDO\n";
  msg += "⚠️ Acima de 60 L/min\n";
  msg += "🔍 Verificar possiveis vazamentos";

  Serial.println("=== ENVIANDO ALERTA DE VAZAMENTO ===");
  // Alertas críticos vão para o tópico de visualização
  enviarCritico(msg);
}
