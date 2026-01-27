// mensagens.ino
#include "variaveis.h"

void checarEnchimentoEsvaziamento() {
  static float ultimoNivel = 0;
  static unsigned long ultimaVerificacao = 0;
  
  unsigned long agora = millis();
  
  if (agora - ultimaVerificacao >= 60000) { // 1 minuto
    float nivelAtual = calcularNivelAgua();
    float variacao = nivelAtual - ultimoNivel;
    
    // Detectar enchimento rápido (bomba funcionando)
    if (variacao > 2.0f) {
      Serial.println("📈 Enchimento detectado - possível bomba funcionando");
    }
    
    // Detectar esvaziamento anormal
    if (variacao < -5.0f) {
      Serial.println("📉 Esvaziamento rápido detectado");
    }
    
    ultimoNivel = nivelAtual;
    ultimaVerificacao = agora;
  }
}

void detectarAvisosLimiares() {
  static bool aviso50 = false;
  static bool aviso25 = false;
  static bool aviso75 = false;
  
  float percentual = calcularNivelAgua(); // Já retorna porcentagem
  
  // Aviso 75%
  if (percentual >= 75.0f && !aviso75) {
    Serial.println("💧 Nível acima de 75% - Boa reserva");
    aviso75 = true;
  } else if (percentual < 70.0f) {
    aviso75 = false;
  }
  
  // Aviso 50%
  if (percentual <= 50.0f && !aviso50) {
    Serial.println("⚠️  Nível chegou a 50% - Atenção");
    aviso50 = true;
  } else if (percentual > 55.0f) {
    aviso50 = false;
  }
  
  // Aviso 25%
  if (percentual <= 25.0f && !aviso25) {
    Serial.println("🚨 Nível chegou a 25% - Próximo do crítico");
    aviso25 = true;
  } else if (percentual > 30.0f) {
    aviso25 = false;
  }
}

void verificarVazamentoRapido() {
  static unsigned long ultimaVerificacaoVazao = 0;
  static float ultimoVolume = 0;
  
  unsigned long agora = millis();
  
  if (agora - ultimaVerificacaoVazao >= 30000) { // 30 segundos
    float volumeAtual = calcularVolume();
    float diferenca = ultimoVolume - volumeAtual;
    float intervaloHoras = 30.0 / 3600.0; // 30 segundos em horas
    float vazao = diferenca / intervaloHoras;
    
    if (vazao > 100.0f) { // > 100 L/h
      Serial.print("🔍 Possível vazamento detectado: ");
      Serial.print(vazao);
      Serial.println(" L/h");
    }
    
    ultimoVolume = volumeAtual;
    ultimaVerificacaoVazao = agora;
  }
}
