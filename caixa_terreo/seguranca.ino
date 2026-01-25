//seguranca.ino
// seguranca.ino
#include "variaveis.h"

// =====================================================
// SEGURANÇA CENTRALIZADA POR FUNÇÃO
// =====================================================

bool exigeNivel(NivelAcesso nivelMinimo) {

  // não autenticado
  if (!autenticado) {
    return false;
  }

  // nível insuficiente
  if (nivelAcessoLogadoEnum < nivelMinimo) {
    return false;
  }

  // sessão continua ativa
  ultimaAtividadeSessao = millis();

  return true;
}
