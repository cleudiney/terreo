//leds.ino
#include "variaveis.h"

// =====================================================
// ATUALIZAÇÃO DOS LEDS DE STATUS
// =====================================================
void atualizarLedsStatus() {
  float nivel = estadoAtual.nivelPercentual;
  bool haVazao = estadoAtual.vazaoEntrada;

  // 🔴 ERRO CRÍTICO (VERMELHO)
  if (estadoAtual.estado == CAIXA_CRITICA || (nivel < 20 && !haVazao)) {
    digitalWrite(PINO_LED_VERMELHO, HIGH);
    digitalWrite(PINO_LED_AMARELO, LOW);
    digitalWrite(PINO_LED_VERDE, LOW);
    return;
  }

  // 🟡 ALERTA (AMARELO)
  if (nivel < 50 && !haVazao) {
    digitalWrite(PINO_LED_VERMELHO, LOW);
    digitalWrite(PINO_LED_AMARELO, HIGH);
    digitalWrite(PINO_LED_VERDE, LOW);
    return;
  }

  // 🟢 ENCHENDO (VERDE)
  if (estadoAtual.estado == CAIXA_ENCHENDO) {
    digitalWrite(PINO_LED_VERMELHO, LOW);
    digitalWrite(PINO_LED_AMARELO, LOW);
    digitalWrite(PINO_LED_VERDE, HIGH);
    return;
  }

  // 🟢 ESTADO NORMAL / ESTÁVEL (VERDE)
  digitalWrite(PINO_LED_VERMELHO, LOW);
  digitalWrite(PINO_LED_AMARELO, LOW);
  digitalWrite(PINO_LED_VERDE, HIGH);
}

// =====================================================
// PISCAR LED (USO GERAL)
// =====================================================
void piscarLed(int pino, int vezes, int intervaloMs) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(pino, HIGH);
    delay(intervaloMs);
    digitalWrite(pino, LOW);
    delay(intervaloMs);
  }
}

// =====================================================
// SINALIZAÇÃO DE BOOT
// =====================================================
void sinalizacaoBoot() {
  piscarLed(PINO_LED_VERDE, 2, 200);
  piscarLed(PINO_LED_AMARELO, 2, 200);
  piscarLed(PINO_LED_VERMELHO, 2, 200);
}

// =====================================================
// INICIALIZAÇÃO DOS LEDS
// =====================================================
void inicializarLeds() {
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AMARELO, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);

  digitalWrite(PINO_LED_VERDE, LOW);
  digitalWrite(PINO_LED_AMARELO, LOW);
  digitalWrite(PINO_LED_VERMELHO, LOW);

  sinalizacaoBoot();
  Serial.println("💡 LEDs de status inicializados");
}
