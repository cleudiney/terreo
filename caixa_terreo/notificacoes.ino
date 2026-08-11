// =====================================================
// INTERVALOS DE REPETIÇÃO
// =====================================================
extern const unsigned long INTERVALO_REPETICAO_URGENTE_MS = 10UL * 60UL * 1000UL;
extern const unsigned long INTERVALO_REPETICAO_CRITICA_MS = 10UL * 60UL * 1000UL;
extern const unsigned long INTERVALO_REPETICAO_INFO_MS    = 2UL * 60UL * 1000UL;
extern const unsigned long INTERVALO_REPETICAO_NORMALIZADO_MS = 30UL * 60UL * 1000UL;

// =====================================================
// CONTROLE ANTI-SPAM (tipo + mensagem)
// =====================================================
bool podeEnviarNotificacao(
  const String& tipo,
  const String& mensagem,
  unsigned long intervaloMinimoMs
) {
  static String ultimosTipos[12];
  static String ultimasMensagens[12];
  static unsigned long ultimosEnviosMs[12] = {0};
  static int proximoSlot = 0;

  for (int i = 0; i < 12; i++) {
    bool mesmaNotificacao =
      (tipo == ultimosTipos[i] && mensagem == ultimasMensagens[i]);

    if (mesmaNotificacao) {
      if (ultimosEnviosMs[i] > 0 &&
          (millis() - ultimosEnviosMs[i]) < intervaloMinimoMs) {
        return false;
      }

      ultimosEnviosMs[i] = millis();
      return true;
    }
  }

  ultimosTipos[proximoSlot] = tipo;
  ultimasMensagens[proximoSlot] = mensagem;
  ultimosEnviosMs[proximoSlot] = millis();
  proximoSlot = (proximoSlot + 1) % 12;
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

  if (tipo == "urgente") {
    enviarUrgente(mensagem);
    return;
  }

  if (tipo == "critica") {
    enviarCritico(mensagem);
    return;
  }

  if (tipo == "resolvido") {
    enviarResolvido(mensagem);
    return;
  }

  if (tipo == "evento") {
    enviarEvento(mensagem);
    return;
  }

  if (tipo == "normalizado") {
    if (podeEnviarNotificacao(tipo, mensagem, INTERVALO_REPETICAO_NORMALIZADO_MS)) {
      enviarResolvido(mensagem);
    }
    return;
  }
}
