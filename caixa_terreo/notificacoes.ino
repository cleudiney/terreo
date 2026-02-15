//notificacoes.ino

void notificarAviso(
  String tipo,
  String mensagem,
  String usuario
) {
  if (tipo == "controle") return;

  static unsigned long ultimaCritica = 0;

  // urgente → uma vez
  if (tipo == "urgente") {
    enviarWhatsappTodos(mensagem);
    return;
  }

  // crítica → a cada 10 minutos
  if (tipo == "critica") {
    if (millis() - ultimaCritica > 600000) {
      enviarWhatsappTodos(mensagem);
      ultimaCritica = millis();
    }
    return;
  }

  // resolvido → uma vez
  if (tipo == "resolvido") {
    enviarWhatsappTodos(mensagem);
    return;
  }

  // evento → uma vez
  if (tipo == "evento") {
    enviarWhatsappTodos(mensagem);
    return;
  }
}
void processarNotificacao(String tipo, String mensagem) {

  static unsigned long ultimaCritica = 0;
  static String ultimaMensagemEnviada = "";
  static unsigned long ultimoEnvio = 0;

  unsigned long agora = millis();

  // Nunca envia controle
  if (tipo == "controle") return;

  // 🔒 BLOQUEIO GLOBAL DE REPETIÇÃO (30s)
  if (mensagem == ultimaMensagemEnviada &&
      (agora - ultimoEnvio) < 30000) {
    return;
  }

  // =========================
  // URGENTE (1x)
  // =========================
  if (tipo == "urgente") {
    enviarWhatsappTodos("⚠️ URGENTE\n" + mensagem);
  }

  // =========================
  // CRÍTICO (10 min)
  // =========================
  else if (tipo == "critica") {
    if (agora - ultimaCritica < 600000) return;

    enviarWhatsappTodos("🚨 CRÍTICO\n" + mensagem);
    ultimaCritica = agora;
  }

  // =========================
  // RESOLVIDO (1x)
  // =========================
  else if (tipo == "resolvido") {
    enviarWhatsappTodos("✅ RESOLVIDO\n" + mensagem);
  }

  // =========================
  // EVENTO (1x com bloqueio)
  // =========================
  else if (tipo == "evento") {
    enviarWhatsappTodos("ℹ️ EVENTO\n" + mensagem);
  }

  // Atualiza controle global
  ultimaMensagemEnviada = mensagem;
  ultimoEnvio = agora;
}
