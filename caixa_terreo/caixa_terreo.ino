#include "variaveis.h"

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Sistema Caixa d'Água ESP32 ===");

  // 1. Inicializar GPIOs
  Serial.println("⚙️ Inicializando GPIOs...");
  pinMode(PINO_BOMBA_A, OUTPUT);
  pinMode(PINO_BOMBA_B, OUTPUT);
  digitalWrite(PINO_BOMBA_A, LOW);
  digitalWrite(PINO_BOMBA_B, LOW);

  // 2. Inicializar LEDs e Chaves
  Serial.println("💡 Inicializando LEDs e chaves...");
  inicializarLeds();
  inicializarChaves();

  // 3. Inicializar SPIFFS
  Serial.println("📂 Inicializando SPIFFS...");
  inicializarSPIFFS();
  verificarArquivosEssenciais();

  // 4. Inicializar WiFi
  Serial.println("🌐 Conectando ao WiFi...");
  inicializarWiFi();

  // 5. Sincronizar hora (NTP)
  Serial.println("⏰ Sincronizando NTP...");
  inicializarTimeClient();

  // 6. Inicializar estado da caixa
  Serial.println("📦 Inicializando estado da caixa...");
  inicializarEstadoCaixa();

  // 7. Inicializar sensores
  Serial.println("🔍 Inicializando sensores...");
  inicializarTodosSensores();

  // 8. Inicializar banco de usuários
  Serial.println("👤 Inicializando banco de usuários...");
  inicializarBancoUsuarios();

  // 9. Inicializar controle de bombas
  Serial.println("🔧 Inicializando controle de bombas...");
  inicializarControleBombas();

  // 10. Inicializar sistema de avisos
  Serial.println("📢 Inicializando sistema de avisos...");
  inicializarSistemaAvisos();

  // 11. Inicializar estatísticas
  Serial.println("📊 Inicializando estatísticas...");
  inicializarEstatisticas();

  Serial.println("\n✅ Sistema inicializado com sucesso!");
  Serial.print("📅 Data/hora atual: ");
  Serial.println(getDataHoraAtual());

  if (modoAP) {
    Serial.print("📶 Modo AP - IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.print("📶 WiFi - IP: ");
    Serial.println(WiFi.localIP());
  }
  //==================== inicializar webserver ====================================
  //============== incluso em 15/01/2025 as 17h47min 
  loopWebServer();

  verificarTimeoutSessao();
    inicializarWebServer();  
}

// ===================== LOOP =====================
void loop() {
  // 1. Manter conexão WiFi
  verificarConexao();

  // 2. Ler sensores (a cada 2 segundos)
  static unsigned long ultimaLeituraSensores = 0;
  if (millis() - ultimaLeituraSensores > 2000) {
    lerTodosSensores();
    ultimaLeituraSensores = millis();
    
    // Log do estado atual
    Serial.printf("[%s] 📊 Nível: %.1f%% | Vazão: %s | Bomba A: %s | Bomba B: %s\n",
                  getHoraAtual().c_str(),
                  estadoAtual.nivelPercentual,
                  estadoAtual.vazaoEntrada ? "SIM" : "NÃO",
                  estadoAtual.bombaAAtiva ? "LIGADA" : "DESLIGADA",
                  estadoAtual.bombaBAtiva ? "LIGADA" : "DESLIGADA");
  }

  // 3. Atualizar LEDs de status
  atualizarLedsStatus();

  // 4. Verificar interrupções manuais
  verificarInterrupcoesManuais();

  // 5. Controle automático de bombas
  controleAutomaticoBombas();

  // 6. Verificação de segurança das bombas
  verificarSegurancaBombas();

  // 7. Estatísticas
  loopEstatisticas();
  // 8. Webserver
  loopWebServer();      

  delay(10);
}
