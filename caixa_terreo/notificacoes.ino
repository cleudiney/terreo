// notificacoes.ino

<<<<<<< ours
// =====================================================
// INTERVALOS DE REPETIÇÃO
// =====================================================
const unsigned long INTERVALO_REPETICAO_URGENTE_MS  = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_CRITICA_MS  = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_INFO_MS     = 2UL  * 60UL * 1000UL;

// =====================================================
// CONTROLE ANTI-SPAM (tipo + mensagem)
// =====================================================
=======
const unsigned long INTERVALO_REPETICAO_URGENTE_MS = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_CRITICA_MS = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_INFO_MS    = 2UL * 60UL * 1000UL;

>>>>>>> theirs
bool podeEnviarNotificacao(
  const String& tipo,
  const String& mensagem,
  unsigned long intervaloMinimoMs
) {
  static String ultimoTipo = "";
  static String ultimaMensagem = "";
  static unsigned long ultimoEnvioMs = 0;

<<<<<<< ours
  bool mesmaNotificacao =
    (tipo == ultimoTipo && mensagem == ultimaMensagem);
=======
  bool mesmaNotificacao = (tipo == ultimoTipo && mensagem == ultimaMensagem);
>>>>>>> theirs

  if (mesmaNotificacao &&
      ultimoEnvioMs > 0 &&
      (millis() - ultimoEnvioMs) < intervaloMinimoMs) {
    return false;
  }

  ultimoTipo = tipo;
  ultimaMensagem = mensagem;
  ultimoEnvioMs = millis();
<<<<<<< ours

  return true;
}

// =====================================================
// NOTIFICAR AVISO (sem prefixo)
// =====================================================
=======
  return true;
}

>>>>>>> theirs
void notificarAviso(
  String tipo,
  String mensagem,
  String usuario
) {
  if (tipo == "controle") return;

<<<<<<< ours
  unsigned long intervalo = INTERVALO_REPETICAO_INFO_MS;

  if (tipo == "urgente")
    intervalo = INTERVALO_REPETICAO_URGENTE_MS;
  else if (tipo == "critica")
    intervalo = INTERVALO_REPETICAO_CRITICA_MS;

  if (podeEnviarNotificacao(tipo, mensagem, intervalo)) {
    enviarWhatsappTodos(mensagem);
  }
}

// =====================================================
// PROCESSAR NOTIFICAÇÃO (com prefixo)
// =====================================================
void processarNotificacao(String tipo, String mensagem) {

=======
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
>>>>>>> theirs
  if (tipo == "controle") return;

  unsigned long intervalo = INTERVALO_REPETICAO_INFO_MS;
  String prefixo = "";

  if (tipo == "urgente") {
<<<<<<< ours
    intervalo = INTERVALO_REPETICAO_URGENTE_MS;
    prefixo = "⚠️ URGENTE\n";
  }
  else if (tipo == "critica") {
    intervalo = INTERVALO_REPETICAO_CRITICA_MS;
    prefixo = "🚨 CRÍTICO\n";
  }
  else if (tipo == "resolvido") {
    prefixo = "✅ RESOLVIDO\n";
  }
  else if (tipo == "evento") {
    prefixo = "ℹ️ EVENTO\n";
  }

  if (podeEnviarNotificacao(tipo, mensagem, intervalo)) {
    enviarWhatsappTodos(prefixo + mensagem);
=======
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
>>>>>>> theirs
  }
}
