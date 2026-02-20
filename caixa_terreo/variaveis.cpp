#include "variaveis.h"

// =====================================================
// ===================== CREDENCIAIS ====================
// =====================================================
const char* SSID        = "esp32_caixa";
const char* PASSWORD    = "Config123#";
const char* AP_SSID     = "caixa_agua_aquarios";
const char* AP_PASSWORD = "@caixa#C1234c!";
/*
const char* SSID        = "LAZARO";
const char* PASSWORD    = "20021831";
const char* AP_SSID     = "caixa_agua_aquarios";
const char* AP_PASSWORD = "@caixa#C1234c!";
*/
// =====================================================
// ===================== DUCKDNS ========================
// =====================================================
String DUCKDNS_DOMAIN = "edificioaquarios-copasa";
String DUCKDNS_TOKEN  = "00a4f00e-4f6a-43f1-8c50-ac0aaa3af2de";

// =====================================================
// ===================== CALLMEBOT ======================
// =====================================================
const String CALLMEBOT_TEL1    = "553199169087";
const String CALLMEBOT_APIKEY1 = "6036650";
const String CALLMEBOT_TEL2    = "553191853572";
const String CALLMEBOT_APIKEY2 = "9472853";
const String CALLMEBOT_TEL3    = "553192590362";
const String CALLMEBOT_APIKEY3 = "8786448";

// ==============================
// Intervalos padrão
// ==============================

const unsigned long INTERVALO_URGENTE_MS  = 10UL * 60UL * 1000UL; // 10 min
const unsigned long INTERVALO_CRITICA_MS  = 10UL * 60UL * 1000UL; // 10 min
const unsigned long INTERVALO_INFO_MS     = 2UL  * 60UL * 1000UL; // 2 min

// ==============================
// Timers
// ==============================

unsigned long ultimoUrgente  = 0;
unsigned long ultimoCritica  = 0;
unsigned long ultimoInfo     = 0;

// =====================================================
// ===================== CONECTIVIDADE ==================
// =====================================================
bool hasInternet = false;
bool modoAP      = false;

WebServer server(WEB_SERVER_PORT);

// =====================================================
// ===================== ESTADO DA CAIXA ================
// =====================================================
EstadoCaixa estadoAtual = {
  100.0,
  0.0,
  false,
  false,
  false,
  CAIXA_ESTAVEL,
  ""
};

unsigned long tUltimaMensagemVazamento = 0;

bool alerta110Enviado = false;
bool alerta115Enviado = false;
bool alertaExtremoEnviado = false;
bool estadoBotaoEmergenciaAnterior = false;
// =====================================================
// ===================== CONTROLE =======================
// =====================================================
bool chaveManualAtiva           = false;
bool botaoEmergenciaPressionado = false;

// =====================================================
// ===================== SENSORES =======================
// =====================================================
float vazaoTotalDiaria = 0.0;

float vazaoCalculada    = 0.0;
float ultimoNivelVazao  = -1.0;
unsigned long ultimoTempoVazao = 0;

// =====================================================
// ===================== TEMPO / NTP ====================
// =====================================================
WiFiUDP ntpUDP;

NTPClient timeClient(
  ntpUDP,
  "pool.ntp.org",
  -3 * 3600,
  60000
);

// =====================================================
// ===================== WEB / SESSÃO ===================
// =====================================================
bool autenticado = false;
bool sessaoAtiva = false;

String usuarioLogado = "";
String nivelAcessoLogado = "";

NivelAcesso nivelAcessoLogadoEnum = ACESSO_NENHUM;

unsigned long ultimaAtividadeSessao = 0;
