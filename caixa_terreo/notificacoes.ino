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

  // controle nunca envia
  if (tipo == "controle") return;

  // urgente → uma vez
  if (tipo == "urgente") {
    enviarWhatsappTodos("⚠️ URGENTE\n" + mensagem);
    return;
  }

  // crítica → a cada 10 minutos
  if (tipo == "critica") {
    if (millis() - ultimaCritica > 600000) {
      enviarWhatsappTodos("🚨 CRÍTICO\n" + mensagem);
      ultimaCritica = millis();
    }
    return;
  }

  // resolvido → uma vez
  if (tipo == "resolvido") {
    enviarWhatsappTodos("✅ RESOLVIDO\n" + mensagem);
    return;
  }

  // evento → uma vez
  if (tipo == "evento") {
    enviarWhatsappTodos("ℹ️ EVENTO\n" + mensagem);
    return;
  }
}
