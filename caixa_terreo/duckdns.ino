// duckdns.ino
#include "variaveis.h"
#include <HTTPClient.h>

// ======= INTERVALOS =======
const unsigned long INTERVALO_ATUALIZACAO_MS = 5UL * 60UL * 1000UL; // 5 minutos

// ======= CONTROLE =======
static unsigned long ultimaAtualizacao = 0;
static bool primeiraVez = true;

// ======= ATUALIZAÇÃO DUCKDNS =======
bool atualizarDuckDNS() {
  if (!hasInternet || WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ [DuckDNS] Sem internet - atualização adiada");
    return false;
  }

  WiFiClient client;
  HTTPClient http;
  
  // URL do DuckDNS (igual ao seu comando curl)
  String url = "http://www.duckdns.org/update?domains=" + String(DUCKDNS_DOMAIN) + 
               "&token=" + String(DUCKDNS_TOKEN) + 
               "&ip="; // IP vazio = auto-detect
  
  Serial.println("🌐 [DuckDNS] Atualizando...");
  Serial.print("   URL: "); Serial.println(url);
  
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    String resposta = http.getString();
    resposta.trim();
    
    Serial.print("📡 HTTP: "); Serial.print(httpCode);
    Serial.print(" | Resposta: "); Serial.println(resposta);
    
    http.end();
    
    if (resposta == "OK") {
      Serial.println("✅ [DuckDNS] Atualização BEM SUCEDIDA!");
      serialMonitor("🌐 DuckDNS: " + String(DUCKDNS_DOMAIN) + ".duckdns.org");
      return true;
    } else if (resposta == "KO") {
      Serial.println("❌ [DuckDNS] Token ou domínio inválido");
      return false;
    } else {
      Serial.println("⚠️ [DuckDNS] Resposta inesperada: " + resposta);
      return false;
    }
  } else {
    Serial.print("❌ [DuckDNS] Erro de conexão: ");
    Serial.println(http.errorToString(httpCode));
    http.end();
    return false;
  }
}

// ======= VERIFICAÇÃO PERIÓDICA =======
void verificarDuckDNS() {
  if (!hasInternet || WiFi.status() != WL_CONNECTED) return;
  
  unsigned long agora = millis();
  
  // Primeira atualização imediata
  if (primeiraVez) {
    primeiraVez = false;
    Serial.println("🚀 [DuckDNS] Primeira atualização...");
    if (atualizarDuckDNS()) {
      ultimaAtualizacao = agora;
    }
    return;
  }
  
  // Atualizações periódicas a cada 5 minutos
  if (agora - ultimaAtualizacao >= INTERVALO_ATUALIZACAO_MS) {
    Serial.println("🔄 [DuckDNS] Atualização periódica...");
    bool sucesso = atualizarDuckDNS();
    ultimaAtualizacao = agora;
    
    // Se falhou, tenta novamente em 1 minuto
    if (!sucesso) {
      ultimaAtualizacao = agora - (INTERVALO_ATUALIZACAO_MS - 60000);
    }
  }
}

// ======= INICIALIZAÇÃO =======
void iniciarDuckDNS() {
  Serial.println("\n🦆 [DuckDNS] INICIANDO SERVICO");
  Serial.println("   Dominio: " + DUCKDNS_DOMAIN + ".duckdns.org");
  Serial.println("   Token: " + DUCKDNS_TOKEN);
  Serial.println("   Atualização: 5 minutos");
  
  // Força primeira atualização no próximo loop
  primeiraVez = true;
  ultimaAtualizacao = 0;
}

// ======= STATUS =======
void mostrarStatusDuckDNS() {
  Serial.println("\n=== STATUS DUCKDNS ===");
  Serial.println("🌐 Dominio: " + DUCKDNS_DOMAIN + ".duckdns.org");
  Serial.println("🔑 Token: " + DUCKDNS_TOKEN);
  Serial.print("📡 Internet: "); Serial.println(hasInternet ? "✅ CONECTADA" : "❌ OFFLINE");
  Serial.print("⏰ Próxima atualização: ");
  
  if (ultimaAtualizacao == 0) {
    Serial.println("Imediata");
  } else {
    unsigned long proxima = INTERVALO_ATUALIZACAO_MS - (millis() - ultimaAtualizacao);
    Serial.print(proxima / 60000); Serial.println(" minutos");
  }
  Serial.println("======================");
}

// ======= ATUALIZAÇÃO MANUAL =======
void forcarAtualizacaoDuckDNS() {
  Serial.println("🔧 [DuckDNS] Forçando atualização manual...");
  atualizarDuckDNS();
}
