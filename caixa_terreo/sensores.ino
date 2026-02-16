//sensores.ino
#include "variaveis.h"

// ==========================================
// CONTROLE DE ALERTAS (ANTI-SPAM)
// ==========================================
bool alerta110Enviado = false;
bool alerta115Enviado = false;
bool alertaExtremoEnviado = false;

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
  const int amostras = 20;
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

  // ==========================================
  // CONVERTE DISTÂNCIA EM ALTURA REAL DE ÁGUA
  // ==========================================
  float alturaAgua = ALTU_CAIXAT - distanciaMedia;

  if (alturaAgua < 0) alturaAgua = 0;
  if (alturaAgua > ALTU_CAIXAT) alturaAgua = ALTU_CAIXAT;

  estadoAtual.nivelCm = alturaAgua;

  // ==========================================
  // Percentual REAL (100% = ALTURA_UTIL_CM)
  // ==========================================
  float nivelPercentual =
      (alturaAgua / ALTURA_UTIL_CM) * 100.0f;

  estadoAtual.nivelPercentual = nivelPercentual;

  float distanciaAteSensor = distanciaMedia;

  // ==========================================
  // ALERTAS DE SOBRE-NÍVEL (ANTI-SPAM)
  // ==========================================

  // --- 110% ---
  if (nivelPercentual >= 110.0f && !alerta110Enviado) {

    registrarAviso("urgente",
      "ATENÇÃO: caixa em " +
      String(nivelPercentual,1) + "%",
      "sistema");

    alerta110Enviado = true;
  }

  // --- 115% ---
  if (nivelPercentual >= 115.0f && !alerta115Enviado) {

    registrarAviso("critica",
      "ATENÇÃO CRÍTICO: caixa em " +
      String(nivelPercentual,1) +
      "% - faltam " +
      String(distanciaAteSensor,1) +
      " cm para atingir o sensor",
      "sistema");

    alerta115Enviado = true;
  }

  // --- RISCO EXTREMO (<=2cm) ---
  if (distanciaAteSensor <= 2.0f && !alertaExtremoEnviado) {

    registrarAviso("critica",
      "RISCO EXTREMO: caixa em " +
      String(nivelPercentual,1) +
      "% - faltam 2 cm para atingir o sensor",
      "sistema");

    desligarAmbasBombas("sistema");

    alertaExtremoEnviado = true;
  }

  // ==========================================
  // RESET AUTOMÁTICO (HISTERÉSE)
  // ==========================================

  if (nivelPercentual < 105.0f) {
    alerta110Enviado = false;
    alerta115Enviado = false;
  }

  if (distanciaAteSensor > 5.0f) {
    alertaExtremoEnviado = false;
  }

  // ==========================================
  // Atualiza estado baseado no nível
  // ==========================================
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
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  pinMode(PINO_SENSOR_VAZAO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PINO_SENSOR_VAZAO),
                  pulsoVazao, FALLING);

  Serial.println("✅ Todos os sensores inicializados");
}
