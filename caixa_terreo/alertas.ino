// alertas.ino
#include "variaveis.h"

// Constante para intervalo de alerta de vazamento
const unsigned long INTERVALO_ALERTA_VAZAMENTO_MS = 30 * 60 * 1000; // 30 minutos

int obterHoraAtual() {
  if (hasInternet && WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    return timeClient.getHours();
  }
  return -1;
}

void enviarStatusPeriodico() {
  static unsigned long tUltimoEnvio = 0;
  static int ultimaHoraEnviada = -1;

  int horaAtual = obterHoraAtual();

  if (horaAtual >= 0 &&
      deveEnviarMensagemPeriodica(horaAtual) &&
      horaAtual != ultimaHoraEnviada) {

    if (tUltimoEnvio == 0 || millis() - tUltimoEnvio > 3600000) {
      enviarMensagemStatus();
      tUltimoEnvio = millis();
      ultimaHoraEnviada = horaAtual;
    }
  }
}

bool deveEnviarMensagemPeriodica(int hora) {
  if (hora == 5) return true;
  if (hora >= 6 && hora <= 8) return true;
  if (hora >= 10 && hora <= 16 && hora % 2 == 0) return true;
  if (hora >= 18 && hora <= 23) return true;
  return false;
}

void checarAlertasCriticos() {

  float percentual = calcularNivelAgua();
  float vazao = calcularVazaoInstantanea();

  // ===============================
  // ALERTA DE VAZAMENTO CRÍTICO
  // ===============================
  if (
      !estadoAtual.vazaoEntrada &&
      vazao < -VAZAO_VAZAMENTO_CRITICO &&
      percentual < 40.0f
     ) {

    if (millis() - tUltimaMensagemVazamento > INTERVALO_ALERTA_VAZAMENTO_MS) {

      registrarAviso(
        "critica",
        "Vazamento detectado: " +
        String(abs(vazao), 1) + " L/min",
        ""
      );

      enviarAlertaVazamentoCritico(abs(vazao));
      tUltimaMensagemVazamento = millis();
    }
  }
}

void enviarAlertaEmergenciaNormalizada() {

  float perc = calcularNivelAgua();
  float alturaAgua = (perc / 100.0) * ALTURA_UTIL_CM;

  float percEmergencia =
    (NIVEL_DESLIGA_EMERGENCIA_CM / ALTURA_UTIL_CM) * 100.0;

  if (perc > percEmergencia) {

    String msg = "✅ *EMERGÊNCIA NORMALIZADA* ✅\n";
    msg += "⏰ " + getHoraAtual() + "\n";
    msg += "💧 Nível: " + String(alturaAgua, 1) + " cm\n";
    msg += "📊 Percentual: " + String(perc, 1) + "%\n";
    msg += "🔧 Retorno ao modo automático\n";
    msg += "🎉 Sistema estabilizado";

    enviarWhatsappTodos(msg);
  }
}

float calcularVazaoInstantanea() {

  static unsigned long tUltimaMedicaoVazao = 0;
  static float ultimoVolume = 0;
  static float vazaoAtual = 0;

  unsigned long agora = millis();
  float volumeAtual = calcularVolume();

  if (tUltimaMedicaoVazao > 0 &&
      agora - tUltimaMedicaoVazao >= 60000) {

    float diferencaVolume = ultimoVolume - volumeAtual;
    float intervaloMinutos =
      (agora - tUltimaMedicaoVazao) / 60000.0;

    vazaoAtual = diferencaVolume / intervaloMinutos;

    ultimoVolume = volumeAtual;
    tUltimaMedicaoVazao = agora;

  } else if (tUltimaMedicaoVazao == 0) {

    tUltimaMedicaoVazao = agora;
    ultimoVolume = volumeAtual;
  }

  return vazaoAtual;
}
