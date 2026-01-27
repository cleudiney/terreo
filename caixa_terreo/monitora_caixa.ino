// monitora_caixa.ino
// Responsável pelo monitoramento do estado da caixa d'água

#include "variaveis.h"

// =====================================================
// HISTÓRICO DE NÍVEL
// =====================================================

float historicoNivel[5] = {0};
int historicoIndex = 0;

// =====================================================
// CONTROLES DE TEMPO (ANTI-OVERFLOW)
// =====================================================

int ultimoMinutoRegistrado = -1;
int ultimoMinutoEstado = -1;

unsigned long ultimaAtualizacaoHist = 0;
unsigned long ultimaVerificacaoEstado = 0;

// =====================================================
// INICIALIZAÇÃO
// =====================================================

void inicializarMonitoramento() {
  for (int i = 0; i < 5; i++) {
    historicoNivel[i] = 0;
  }

  ultimaAtualizacaoHist = millis();
  ultimaVerificacaoEstado = millis();

  Serial.println("✅ Monitoramento da caixa inicializado (NTP + millis)");
}

// =====================================================
// CONTROLE DE INTERVALOS
// =====================================================

bool deveAtualizarHistorico() {

  if (hasInternet) {
    timeClient.update();
    int minutoAtual = timeClient.getMinutes();

    if (minutoAtual % 2 == 0 && minutoAtual != ultimoMinutoRegistrado) {
      ultimoMinutoRegistrado = minutoAtual;
      return true;
    }
    return false;
  }

  unsigned long agora = millis();

  if (agora - ultimaAtualizacaoHist >= 120000UL || agora < ultimaAtualizacaoHist) {
    ultimaAtualizacaoHist = agora;
    return true;
  }

  return false;
}

bool deveVerificarEstado() {

  if (hasInternet) {
    timeClient.update();
    int minutoAtual = timeClient.getMinutes();

    if (minutoAtual % 4 == 0 && minutoAtual != ultimoMinutoEstado) {
      ultimoMinutoEstado = minutoAtual;
      return true;
    }
    return false;
  }

  unsigned long agora = millis();

  if (agora - ultimaVerificacaoEstado >= 240000UL || agora < ultimaVerificacaoEstado) {
    ultimaVerificacaoEstado = agora;
    return true;
  }

  return false;
}

// =====================================================
// PROTEÇÃO CONTRA OVERFLOW LONGO
// =====================================================

void verificarResetTempo() {
  static unsigned long ultimoResetCheck = 0;
  unsigned long agora = millis();

  if (agora - ultimoResetCheck >= 2592000000UL || agora < ultimoResetCheck) {

    serialMonitor("🔄 Reset preventivo de timers (overflow)");

    ultimoResetCheck = agora;
    ultimaAtualizacaoHist = agora;
    ultimaVerificacaoEstado = agora;

    ultimoMinutoRegistrado = -1;
    ultimoMinutoEstado = -1;
  }
}

// =====================================================
// DEBUG
// =====================================================

String getEstadoDebugString(EstadoCaixaEnum estado) {
  switch (estado) {
    case CAIXA_ESTAVEL:     return "ESTÁVEL";
    case CAIXA_ENCHENDO:    return "ENCHENDO";
    case CAIXA_ESVAZIANDO:  return "ESVAZIANDO";
    case CAIXA_ESTAGNADA:   return "ESTAGNADA";
    case CAIXA_CRITICA:     return "CRÍTICA";
    default:                return "DESCONHECIDO";
  }
}

// =====================================================
// ATUALIZAÇÃO PRINCIPAL
// =====================================================

void atualizarEstadoCaixa() {

  verificarResetTempo();

  // =====================================
  // ATUALIZA HISTÓRICO + VAZÃO
  // =====================================

  if (deveAtualizarHistorico()) {

    float nivelAtual = calcularNivelAgua();

    if (nivelAtual >= 0) {

      historicoNivel[historicoIndex] = nivelAtual;
      historicoIndex = (historicoIndex + 1) % 5;

      unsigned long agora = millis();

      if (ultimoNivelVazao >= 0) {

        float deltaNivel = nivelAtual - ultimoNivelVazao;
        float deltaTempoMin = (agora - ultimoTempoVazao) / 60000.0;

        if (deltaTempoMin > 0) {

          float litros =
            (deltaNivel / 100.0) * VOLUME_TOTAL_L;

          vazaoCalculada = litros / deltaTempoMin;

          serialMonitor(
            "💧 Vazão calculada: " +
            String(vazaoCalculada, 1) + " L/min"
          );
        }
      }

      ultimoNivelVazao = nivelAtual;
      ultimoTempoVazao = agora;

      String modo = hasInternet ? "NTP" : "MILLIS";
      serialMonitor("📊 Histórico (" + modo + ") → " + String(nivelAtual, 1) + "%");
    }
  }

  // =====================================
  // VERIFICA MUDANÇA DE ESTADO
  // =====================================

  if (deveVerificarEstado()) {

    EstadoCaixaEnum novoEstado = determinarEstado();

    String modo = hasInternet ? "NTP" : "MILLIS";

    serialMonitor("🔄 Avaliando estado (" + modo + ")");
    serialMonitor("   Atual: " + getEstadoString());

    serialMonitor(
      "   Detectado: " +
      getEstadoDebugString(novoEstado)
    );

    if (novoEstado != estadoAtual.estado) {
      estadoAtual.estado = novoEstado;
      serialMonitor("🎯 ESTADO ALTERADO → " + getEstadoString());
    }
  }
}

// =====================================================
// TENDÊNCIA
// =====================================================

float calcularTendencia() {

  float soma = 0;
  int validos = 0;

  for (int i = 1; i < 5; i++) {

    int atual = (historicoIndex - i + 5) % 5;
    int anterior = (historicoIndex - i - 1 + 5) % 5;

    if (historicoNivel[atual] > 0 &&
        historicoNivel[anterior] > 0) {

      soma += historicoNivel[atual] - historicoNivel[anterior];
      validos++;
    }
  }

  if (validos > 0) {
    float tendencia = soma / validos;
    serialMonitor("📈 Tendência: " + String(tendencia, 2) + "%");
    return tendencia;
  }

  return 0;
}

// =====================================================
// DETERMINAÇÃO DE ESTADO
// =====================================================

EstadoCaixaEnum determinarEstado() {

  float tendencia = calcularTendencia();

  if (tendencia > 2.0) {
    return CAIXA_ENCHENDO;
  }

  if (tendencia < -2.0) {
    return CAIXA_ESVAZIANDO;
  }

  return CAIXA_ESTAVEL;
}

// =====================================================
// STRINGS PARA INTERFACE
// =====================================================

String getEstadoString() {
  switch (estadoAtual.estado) {
    case CAIXA_ESTAVEL:     return "Estável";
    case CAIXA_ENCHENDO:    return "Enchendo";
    case CAIXA_ESVAZIANDO:  return "Esvaziando";
    case CAIXA_ESTAGNADA:   return "Estagnada";
    case CAIXA_CRITICA:     return "Crítica";
    default:                return "Desconhecido";
  }
}

String getEstadoCSS() {
  switch (estadoAtual.estado) {
    case CAIXA_ENCHENDO:    return "success";
    case CAIXA_ESVAZIANDO:  return "danger";
    case CAIXA_CRITICA:     return "dark";
    case CAIXA_ESTAGNADA:   return "secondary";
    case CAIXA_ESTAVEL:
    default:                return "warning";
  }
}
