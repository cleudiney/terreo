// =====================================================
// INTERVALOS DE REPETIÇÃO
// =====================================================
const unsigned long INTERVALO_REPETICAO_URGENTE_MS  = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_CRITICA_MS  = 10UL * 60UL * 1000UL;
const unsigned long INTERVALO_REPETICAO_INFO_MS     = 2UL  * 60UL * 1000UL;


// =====================================================
// CONTROLE ANTI-SPAM (tipo + mensagem)
// =====================================================
bool podeEnviarNotificacao(
  const String& tipo,
  const String& mensagem,
  unsigned long intervaloMinimoMs
) {
  static String ultimoTipo = "";
  static String ultimaMensagem = "";
  static unsigned long ultimoEnvioMs = 0;

  bool mesmaNotificacao =
    (tipo == ultimoTipo && mensagem == ultimaMensagem);

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


// =====================================================
// NOTIFICAR AVISO (sem prefixo)
// =====================================================
void notificarAviso(
  String tipo,
  String mensagem,
  String usuario
) {
  if (tipo == "controle") return;

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

  if (tipo == "controle") return;

  unsigned long intervalo = INTERVALO_REPETICAO_INFO_MS;
  String prefixo = "";

  if (tipo == "urgente") {
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
  }
}
