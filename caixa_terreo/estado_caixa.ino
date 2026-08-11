//estado_caixa.ino
#include "variaveis.h"

// =====================================================
// TEXTO DO ESTADO PARA INTERFACE
// =====================================================
String getStatusCaixa() {
  if (estadoAtual.vazaoEntrada && (estadoAtual.bombaAAtiva || estadoAtual.bombaBAtiva)) {
    return "Enchendo";
  }
  if (!estadoAtual.vazaoEntrada && !estadoAtual.bombaAAtiva && !estadoAtual.bombaBAtiva) {
    return "Esvaziando";
  }
  if (estadoAtual.vazaoEntrada && !estadoAtual.bombaAAtiva && !estadoAtual.bombaBAtiva) {
    return "Estagnada";
  }
  return "Estável";
}

float calcularNivelAgua() {
  return estadoAtual.nivelPercentual;
}

float calcularVolume() {
  return (estadoAtual.nivelPercentual / 100.0f) * VOLUME_TOTAL_L;
}

// =====================================================
// INICIALIZAÇÃO
// =====================================================
void inicializarEstadoCaixa() {
  estadoAtual.nivelPercentual = 100.0;
  estadoAtual.vazaoEntrada   = false;
  estadoAtual.bombaAAtiva    = false;
  estadoAtual.bombaBAtiva    = false;
  estadoAtual.estado         = CAIXA_ESTAVEL;
  estadoAtual.ultimaAtualizacao = getDataHoraAtual();

  Serial.println("✅ Estado da caixa inicializado");
}
