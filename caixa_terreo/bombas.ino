// bombas.ino
#include "variaveis.h"

// =====================================================
// CONTROLE MANUAL DAS BOMBAS
// =====================================================
void ligarBomba(char bomba, String usuario) {

  if (bomba == 'A') {
    digitalWrite(PINO_BOMBA_A, HIGH);
    estadoAtual.bombaAAtiva = true;

    registrarAviso("evento", "Bomba A ligada manualmente", usuario);
  }

  if (bomba == 'B') {
    digitalWrite(PINO_BOMBA_B, HIGH);
    estadoAtual.bombaBAtiva = true;

    registrarAviso("evento", "Bomba B ligada manualmente", usuario);
  }
}

void desligarBomba(char bomba, String usuario) {

  if (bomba == 'A') {
    digitalWrite(PINO_BOMBA_A, LOW);
    estadoAtual.bombaAAtiva = false;

    registrarAviso("evento", "Bomba A desligada manualmente", usuario);
  }

  if (bomba == 'B') {
    digitalWrite(PINO_BOMBA_B, LOW);
    estadoAtual.bombaBAtiva = false;

    registrarAviso("evento", "Bomba B desligada manualmente", usuario);
  }
}

// =====================================================
// LIGAR / DESLIGAR AMBAS AS BOMBAS
// =====================================================
void ligarAmbasBombas(String usuario) {

  digitalWrite(PINO_BOMBA_A, HIGH);
  digitalWrite(PINO_BOMBA_B, HIGH);

  estadoAtual.bombaAAtiva = true;
  estadoAtual.bombaBAtiva = true;

  registrarAviso("evento", "Ambas as bombas ligadas", usuario);
}

void desligarAmbasBombas(String usuario) {

  digitalWrite(PINO_BOMBA_A, LOW);
  digitalWrite(PINO_BOMBA_B, LOW);

  estadoAtual.bombaAAtiva = false;
  estadoAtual.bombaBAtiva = false;

  registrarAviso("evento", "Ambas as bombas desligadas", usuario);
}

// =====================================================
// CONTROLE AUTOMÁTICO DAS BOMBAS
// =====================================================
void controleAutomaticoBombas() {

  float nivel = estadoAtual.nivelPercentual;
  bool vazao  = estadoAtual.vazaoEntrada;

  // 🔴 Segurança: sem vazão e nível baixo
  if (!vazao && nivel < NIVEL_ALERTA) {
    desligarAmbasBombas("sistema");
    registrarAviso("urgente", "Sem vazão e nível abaixo de alerta", "sistema");
    return;
  }

  // 🔴 Nível crítico
  if (!vazao && nivel < NIVEL_CRITICO) {
    desligarAmbasBombas("sistema");
    registrarAviso("critica", "Nível crítico (<20%) e sem vazão", "sistema");
    return;
  }

  // 🟢 Enchimento normal
  if (nivel < NIVEL_NORMAL) {
    if (!estadoAtual.bombaAAtiva || !estadoAtual.bombaBAtiva) {
      ligarAmbasBombas("sistema");
    }
  }

  // 🟢 Caixa cheia
  if (nivel >= 95.0) {
    if (estadoAtual.bombaAAtiva || estadoAtual.bombaBAtiva) {
      desligarAmbasBombas("sistema");
      registrarAviso("resolvido", "Caixa cheia - bombas desligadas", "sistema");
    }
  }
}

// =====================================================
// VERIFICAÇÃO DE SEGURANÇA
// =====================================================
void verificarSegurancaBombas() {

  if (estadoAtual.bombaAAtiva && !estadoAtual.vazaoEntrada) {
    desligarBomba('A', "sistema");
    registrarAviso("urgente", "Bomba A ligada sem vazão", "sistema");
  }

  if (estadoAtual.bombaBAtiva && !estadoAtual.vazaoEntrada) {
    desligarBomba('B', "sistema");
    registrarAviso("urgente", "Bomba B ligada sem vazão", "sistema");
  }
}

// =====================================================
// INICIALIZAÇÃO
// =====================================================
void inicializarControleBombas() {

  pinMode(PINO_BOMBA_A, OUTPUT);
  pinMode(PINO_BOMBA_B, OUTPUT);

  desligarAmbasBombas("sistema");

  Serial.println("✅ Controle de bombas inicializado");
}
