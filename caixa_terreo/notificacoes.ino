// notificacoes.ino

void processarNotificacao(String tipo, String mensagem) {

  static unsigned long ultimaCritica = 0;
  static unsigned long ultimoEnvioGlobal = 0;
  static String ultimaMensagem = "";

  unsigned long agora = millis();

  // Nunca envia controle
  if (tipo == "controle") return;

  // =========================
  // BLOQUEIO GLOBAL (30s)
  // Evita spam idêntico
  // =========================
  if (mensagem == ultimaMensagem &&
      (agora - ultimoEnvioGlobal) < 30000) {
    return;
  }

  // =========================
  // URGENTE (envia sempre)
  // =========================
  if (tipo == "urgente") {
    enviarWhatsappTodos("⚠️ URGENTE\n" + mensagem);
  }

  // =========================
  // CRÍTICO (10 minutos)
  // =========================
  else if (tipo == "critica") {

    if ((agora - ultimaCritica) < 600000) {
      return;
    }

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
  // EVENTO (1x)
  // =========================
  else if (tipo == "evento") {
    enviarWhatsappTodos("ℹ️ EVENTO\n" + mensagem);
  }

  // Atualiza controle global
  ultimaMensagem = mensagem;
  ultimoEnvioGlobal = agora;
}
