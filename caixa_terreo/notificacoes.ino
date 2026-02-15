//notificacoes.ino

const unsigned long INTERVALO_REPETICAO_URGENTE_MS = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_CRITICA_MS = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_INFO_MS    = 2UL * 60UL * 1000UL;

bool podeEnviarNotificacao(
  const String& tipo,
  const String& mensagem,
  unsigned long intervaloMinimoMs
) {
  static String ultimoTipo = "";
  static String ultimaMensagem = "";
  static unsigned long ultimoEnvioMs = 0;

  bool mesmaNotificacao = (tipo == ultimoTipo && mensagem == ultimaMensagem);

  if (mesmaNotificacao &&
      ultimoEnvioMs > 0 &&
      (millis() - ultimoEnvioMs) < intervaloMinimoMs) {
    return false;
  }

  ultimoTipo = tipo;
  ultimaMensagem = mensagem;
  ultimoEnvioMs = millis();
  return true;
}

void notificarAviso(
  String tipo,
  String mensagem,
  String usuario
) {
  if (tipo == "controle") return;

  // urgente → uma vez
  if (tipo == "urgente") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_URGENTE_MS)) {
      enviarWhatsappTodos(mensagem);
    }
    return;
  }

  // crítica → a cada 10 minutos
  if (tipo == "critica") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_CRITICA_MS)) {
      enviarWhatsappTodos(mensagem);
    }
    return;
  }

  // resolvido → uma vez
  if (tipo == "resolvido") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_INFO_MS)) {
      enviarWhatsappTodos(mensagem);
    }
    return;
  }

  // evento → uma vez
  if (tipo == "evento") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_INFO_MS)) {
      enviarWhatsappTodos(mensagem);
    }
    return;
  }
}
void processarNotificacao(String tipo, String mensagem) {
  // controle nunca envia
  if (tipo == "controle") return;

  // urgente → uma vez
  if (tipo == "urgente") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_URGENTE_MS)) {
      enviarWhatsappTodos("⚠️ URGENTE\n" + mensagem);
    }
    return;
  }

  // crítica → a cada 10 minutos
  if (tipo == "critica") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_CRITICA_MS)) {
      enviarWhatsappTodos("🚨 CRÍTICO\n" + mensagem);
    }
    return;
  }

  // resolvido → uma vez
  if (tipo == "resolvido") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_INFO_MS)) {
      enviarWhatsappTodos("✅ RESOLVIDO\n" + mensagem);
    }
    return;
  }

  // evento → uma vez
  if (tipo == "evento") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_INFO_MS)) {
      enviarWhatsappTodos("ℹ️ EVENTO\n" + mensagem);
    }
    return;
  }
}
