// estados.ino
#include "variaveis.h"
void serialMonitor(const String& texto) {
  Serial.println(texto);
}
void serialMonitorEstadoCaixa() {

  float nivel = calcularNivelAgua();
  float volume = calcularVolume();

  serialMonitor("=================================");
  serialMonitor("📦 ESTADO ATUAL DA CAIXA");
  serialMonitor("---------------------------------");

  serialMonitor("   Nível: " + String(nivel, 1) + " %");
  serialMonitor("   Volume: " + String(volume, 0) + " L / " +
                 String(VOLUME_TOTAL_L, 0) + " L");

  serialMonitor("   Vazão entrada: " +
                 String(estadoAtual.vazaoEntrada ? "SIM" : "NÃO"));

  serialMonitor("   Bomba A: " +
                 String(estadoAtual.bombaAAtiva ? "LIGADA" : "DESLIGADA"));

  serialMonitor("   Bomba B: " +
                 String(estadoAtual.bombaBAtiva ? "LIGADA" : "DESLIGADA"));

  serialMonitor("   Estado: " + getEstadoString());

  serialMonitor("=================================");
}


void serialMonitorConfiguracoes() {

  serialMonitor("⚙️ CONFIGURAÇÕES DO SISTEMA");
  serialMonitor("---------------------------------");

  serialMonitor("   Altura útil: " +
                 String(ALTURA_UTIL_CM, 1) + " cm");

  serialMonitor("   Volume total: " +
                 String(VOLUME_TOTAL_L, 0) + " L");

  serialMonitor("   Emergência: " +
                 String(NIVEL_DESLIGA_EMERGENCIA_CM, 1) + " cm");

  serialMonitor("---------------------------------");
}


void serialMonitorCapturaDados() {

  serialMonitor("📡 CAPTURA DE DADOS");
  serialMonitor("---------------------------------");

  serialMonitor("   Hora: " + getHoraAtual());
  serialMonitor("   Internet: " + String(hasInternet ? "OK" : "OFF"));

  serialMonitor("---------------------------------");
}
