//sensores.ino
#include "variaveis.h"

// =====================================================
// VARIÁVEIS INTERNAS DO SENSOR
// =====================================================
volatile unsigned long pulsosVazao = 0;

// =====================================================
// LEITURA DE TODOS OS SENSORES
// =====================================================
void lerTodosSensores() {
  lerSensorUltrassom();
  lerSensorVazao();
  estadoAtual.ultimaAtualizacao = getDataHoraAtual();
}

// =====================================================
// LEITURA DO ULTRASSOM (COM FILTRO)
// =====================================================
void lerSensorUltrassom() {
  const int amostras = 5;
  float soma = 0;
  int leiturasValidas = 0;

  for (int i = 0; i < amostras; i++) {
    digitalWrite(PINO_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PINO_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PINO_TRIG, LOW);

    long duracao = pulseIn(PINO_ECHO, HIGH, 30000);
    if (duracao > 0) {
      float distanciaCm = (duracao * 0.0343f) / 2.0f;
      soma += distanciaCm;
      leiturasValidas++;
    }
    delay(20);
  }

  if (leiturasValidas == 0) return;
  
  float distanciaMedia = soma / leiturasValidas;
  estadoAtual.nivelCm = distanciaMedia;
  
  // Converte para percentual
  float nivelPercentual = 100.0f - ((distanciaMedia / ALTURA_CAIXA_CM) * 100.0f);
  if (nivelPercentual < 0)   nivelPercentual = 0;
  if (nivelPercentual > 100) nivelPercentual = 100;
  
  estadoAtual.nivelPercentual = nivelPercentual;
  
  // Atualiza estado baseado no nível
  if (estadoAtual.vazaoEntrada && (estadoAtual.bombaAAtiva || estadoAtual.bombaBAtiva)) {
    estadoAtual.estado = CAIXA_ENCHENDO;
  } else if (!estadoAtual.vazaoEntrada && nivelPercentual > 0) {
    estadoAtual.estado = CAIXA_ESVAZIANDO;
  } else if (estadoAtual.vazaoEntrada && nivelPercentual > 95) {
    estadoAtual.estado = CAIXA_ESTAGNADA;
  } else if (nivelPercentual < NIVEL_CRITICO) {
    estadoAtual.estado = CAIXA_CRITICA;
  } else {
    estadoAtual.estado = CAIXA_ESTAVEL;
  }
}

// =====================================================
// LEITURA DA VAZÃO
// =====================================================
void IRAM_ATTR pulsoVazao() {
  pulsosVazao++;
}

void lerSensorVazao() {
  static unsigned long ultimoCalculo = 0;
  if (millis() - ultimoCalculo < 1000) return;
  ultimoCalculo = millis();

  noInterrupts();
  unsigned long pulsos = pulsosVazao;
  pulsosVazao = 0;
  interrupts();

  float vazao = pulsos * FATOR_CALIBRACAO_VAZAO;
  estadoAtual.vazaoEntrada = (vazao > 0.01);
}

// =====================================================
// INICIALIZAÇÃO DE TODOS OS SENSORES
// =====================================================
void inicializarTodosSensores() {
  // Ultrassom
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  
  // Vazão
  pinMode(PINO_SENSOR_VAZAO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PINO_SENSOR_VAZAO), 
                  pulsoVazao, FALLING);
  
  Serial.println("✅ Todos os sensores inicializados");
}
