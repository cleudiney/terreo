// variaveis.cpp
#include "variaveis.h"

/*
 * Criado / reorganizado em: 2026-01-15
 * Motivo:
 *  - Refletir exatamente o contrato definido em variaveis.h
 *  - Centralizar definições globais (1 definição por variável)
 *  - Eliminar duplicações estruturais
 *
 * Este arquivo:
 *  - NÃO contém lógica
 *  - NÃO contém funções
 *  - SOMENTE definição de variáveis globais
 */

// =====================================================
// ===================== CREDENCIAIS ====================
// =====================================================

// --------------------- WI-FI --------------------------
const char* SSID        = "esp32_caixa";
const char* PASSWORD    = "Config123#";
const char* AP_SSID     = "caixa_agua_aquarios";
const char* AP_PASSWORD = "@caixa#C1234c!";

// --------------------- DDNS (No-IP) -------------------
// (mantido aqui por decisão de projeto — não faz parte
//  do contrato mínimo do webserver ainda)
const char* DDNS_HOST = "edificioaquarios.ddns.net";
const char* DDNS_USER = "qc2wtrf";
const char* DDNS_PASS = "wrtpK3LSpuWm";

// --------------------- DUCKDNS ------------------------
const char* DUCKDNS_DOMAIN = "edificioaquarios";
const char* DUCKDNS_TOKEN  = "00a4f00e-4f6a-43f1-8c50-ac0aaa3af2de";

// --------------------- CALLMEBOT ----------------------
const String CALLMEBOT_TEL1     = "553199169087";
const String CALLMEBOT_APIKEY1  = "6036650";
const String CALLMEBOT_TEL2     = "5553191853572";
const String CALLMEBOT_APIKEY2  = "9472853";
const String CALLMEBOT_TEL3     = "553192590362";
const String CALLMEBOT_APIKEY3  = "8786448";

// =====================================================
// ===================== CONECTIVIDADE ==================
// =====================================================
bool hasInternet = false;
bool modoAP      = false;
WebServer server(WEB_SERVER_PORT);

// =====================================================
// ===================== ESTADO DA CAIXA ================
// =====================================================
EstadoCaixa estadoAtual;

// =====================================================
// ===================== CONTROLE DE BOMBAS ==============
// =====================================================
bool chaveManualAtiva           = false;
bool botaoEmergenciaPressionado = false;

// =====================================================
// ===================== SENSORES =======================
// =====================================================
float vazaoTotalDiaria = 0.0;

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
// ===================== WEB SERVER / SESSÃO =============
// =====================================================
/*
 * Criado em: 2026-01-15
 * Motivo:
 *  - Espelhar contrato Web definido em variaveis.h
 *  - Suportar autenticação, sessão e permissões
 *
 * Usado em:
 *  - webserver.ino
 *  - usuarios.ino
 */

// Estado de autenticação
bool autenticado = false;

// Sessão ativa (futuro: token / timeout)
bool sessaoAtiva = false;

// Usuário autenticado
String usuarioLogado = "";

// Nível de acesso do usuário autenticado
NivelAcesso nivelAcessoLogadoEnum = ACESSO_NENHUM;

// Timestamp da última atividade válida
unsigned long ultimaAtividadeSessao = 0;
