//chaves.ino
#include "variaveis.h"

// =====================================================
// LEITURA DAS CHAVES FÍSICAS
// =====================================================
void verificarInterrupcoesManuais() {
  static bool estadoAnteriorManual = false;
  static bool estadoAnteriorEmergencia = false;

  bool estadoAtualManual     = digitalRead(PINO_CHAVE_MANUAL) == LOW;
  bool estadoAtualEmergencia = digitalRead(PINO_BOTAO_EMERGENCIA) == LOW;

  // =====================
  // CHAVE MANUAL
  // =====================
  if (estadoAtualManual && !estadoAnteriorManual) {
    chaveManualAtiva = !chaveManualAtiva;

    if (chaveManualAtiva) {
      registrarAviso("evento", "Modo manual ativado pela chave física");
    } else {
      registrarAviso("evento", "Modo manual desativado pela chave física");
    }
  }

  // =====================
  // BOTÃO DE EMERGÊNCIA
  // =====================
  if (estadoAtualEmergencia && !estadoAnteriorEmergencia) {
    botaoEmergenciaPressionado = true;

    // CORREÇÃO: usar desligarAmbasBombas() em vez de desligarBombas()
    desligarAmbasBombas("sistema");
    registrarAviso("urgente", "BOTÃO DE EMERGÊNCIA acionado - bombas desligadas");
  }

  estadoAnteriorManual     = estadoAtualManual;
  estadoAnteriorEmergencia = estadoAtualEmergencia;
}

// =====================================================
// INICIALIZAÇÃO DAS CHAVES
// =====================================================
void inicializarChaves() {
  pinMode(PINO_CHAVE_MANUAL, INPUT_PULLUP);
  pinMode(PINO_BOTAO_EMERGENCIA, INPUT_PULLUP);

  chaveManualAtiva = false;
  botaoEmergenciaPressionado = false;

  Serial.println("✅ Chaves físicas inicializadas");
}
