// estatistica.ino
#include "variaveis.h"

// =====================================================
// FAIXAS HORÁRIAS (ANÁLISE DE CONSUMO)
// =====================================================
String faixaHorariaAtual() {
  // getHoraAtual() -> "HH:MM:SS"
  int h = getHoraAtual().substring(0, 2).toInt();

  if (h >= 5  && h <= 7)  return "amanhecer";
  if (h >= 8  && h <= 10) return "manha";
  if (h >= 11 && h <= 13) return "almoco";
  if (h >= 14 && h <= 16) return "tarde";
  if (h >= 17 && h <= 19) return "anoitecer";
  if (h >= 20 && h <= 23) return "noite";
  return "madrugada";
}

// =====================================================
// CLASSIFICAÇÃO DO CONSUMO
// =====================================================
String classificarConsumo(float deltaNivel) {
  if (deltaNivel < -20) return "altissimo";
  if (deltaNivel < -10) return "alto";
  if (deltaNivel < -3)  return "moderado";
  return "baixo";
}

// =====================================================
// ANÁLISE DE CONSUMO PERIÓDICO
// =====================================================
void analisarConsumo() {
  static float nivelAnterior = -1.0;
  static unsigned long ultimoCheck = 0;

  if (millis() - ultimoCheck < 3600000UL) return;
  ultimoCheck = millis();

  float nivelAtual = estadoAtual.nivelPercentual;

  if (nivelAnterior < 0) {
    nivelAnterior = nivelAtual;
    return;
  }

  float delta = nivelAtual - nivelAnterior;
  String classificacao = classificarConsumo(delta);

  if (classificacao == "alto" || classificacao == "altissimo") {
    String msg = "Consumo " + classificacao +
                 " detectado (" + String(delta, 1) + "%)";
    registrarAviso("evento", msg);
  }

  nivelAnterior = nivelAtual;
}

// =====================================================
// REGISTRO ESTATÍSTICO DIÁRIO
// =====================================================
void registrarEstatisticaDiaria() {
  static bool registradoHoje = false;

  int hora = getHoraAtual().substring(0, 2).toInt();

  if (hora == 23 && !registradoHoje) {

    String linha =
      getDataAtual() + ";" +
      faixaHorariaAtual() + ";" +
      String(estadoAtual.nivelPercentual, 1) + "\n";

    File f = SPIFFS.open(ARQ_REGISTROS, FILE_APPEND);
    if (f) {
      f.print(linha);
      f.close();
    }

    registradoHoje = true;
  }

  if (hora == 0) {
    registradoHoje = false;
  }
}

// =====================================================
// LOOP
// =====================================================
void loopEstatisticas() {
  analisarConsumo();
  registrarEstatisticaDiaria();
}

// =====================================================
// INIT
// =====================================================
void inicializarEstatisticas() {
  Serial.println("📊 Sistema de estatísticas inicializado");
}
