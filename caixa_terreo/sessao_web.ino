#include "variaveis.h"

// =====================================================
// CONTROLE DE TIMEOUT DE SESSÃO WEB
// =====================================================

void verificarTimeoutSessao() {

  if (!sessaoAtiva) return;

  unsigned long agora = millis();

  if (agora - ultimaAtividadeSessao > TEMPO_SESSAO_MS) {

    Serial.println("⏱ Sessão expirada por inatividade");

    registrarAviso(
      "evento",
      "Sessão expirada automaticamente",
      usuarioLogado
    );

    autenticado = false;
    sessaoAtiva = false;
    usuarioLogado = "";
    nivelAcessoLogadoEnum = ACESSO_NENHUM;
  }
}
