#ifndef VARIAVEIS_H
#define VARIAVEIS_H

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WebServer.h>

// ==============================
// Intervalos de notificação
// ==============================

extern const unsigned long INTERVALO_URGENTE_MS;
extern const unsigned long INTERVALO_CRITICA_MS;
extern const unsigned long INTERVALO_INFO_MS;

extern bool estadoBotaoEmergenciaAnterior;

// ==============================
// Controle de tempo (timers)
// ==============================

extern unsigned long ultimoUrgente;
extern unsigned long ultimoCritica;
extern unsigned long ultimoInfo;

// =====================================================
// ===================== DEFINIÇÕES BÁSICAS =============
// =====================================================
#define DIR_DADOS      "/dados"
#define ARQ_USUARIOS   "/dados/usuarios.dat"
#define ARQ_AVISOS     "/dados/avisos.dat"
#define ARQ_REGISTROS  "/dados/registros.dat"
// =====================================================
// =================== IDENTIDADE DO SISTEMA (WHATSAPP / LOGS / WEB)
// =====================================================
#define SISTEMA_NOME "💧 Monitor Caixa d'Água - TERREO / COPASA - Edifício Aquários"

// =====================================================
// ===================== CREDENCIAIS ====================
// =====================================================
extern const char* SSID;
extern const char* PASSWORD;
extern const char* AP_SSID;
extern const char* AP_PASSWORD;

// =====================================================
// ===================== PINAGEM ========================
// =====================================================
#define PINO_BOMBA_A          26
#define PINO_BOMBA_B          27
#define PINO_TRIG             32
#define PINO_ECHO             33
#define PINO_SENSOR_VAZAO     34
#define PINO_LED_VERDE        12
#define PINO_LED_AMARELO      14
#define PINO_LED_VERMELHO     25
#define PINO_CHAVE_MANUAL     35
#define PINO_BOTAO_EMERGENCIA 36

// =====================================================
// ===================== CONSTANTES =====================
// =====================================================
#define ALTURA_CAIXA_CM        200.0
#define FATOR_CALIBRACAO_VAZAO 0.0025

#define NIVEL_CRITICO  20.0
#define NIVEL_ALERTA   40.0
#define NIVEL_NORMAL   60.0

// =====================================================
// ===================== CALLMEBOT ======================
// =====================================================
extern const String CALLMEBOT_TEL1;
extern const String CALLMEBOT_APIKEY1;
extern const String CALLMEBOT_TEL2;
extern const String CALLMEBOT_APIKEY2;
extern const String CALLMEBOT_TEL3;
extern const String CALLMEBOT_APIKEY3;

// =====================================================
// ===================== DUCKDNS ========================
// =====================================================
extern String DUCKDNS_DOMAIN;
extern String DUCKDNS_TOKEN;

// =====================================================
// ===================== ENUMS ==========================
// =====================================================
enum EstadoCaixaEnum {
  CAIXA_ENCHENDO,
  CAIXA_ESVAZIANDO,
  CAIXA_ESTAGNADA,
  CAIXA_CRITICA,
  CAIXA_ESTAVEL
};

enum NivelAcesso {
  ACESSO_NENHUM = 0,
  ACESSO_MORADOR,
  ACESSO_ZELADOR,
  ACESSO_SUBSINDICO,
  ACESSO_SINDICO,
  ACESSO_ADMIN
};

// =====================================================
// ===================== ESTRUTURAS =====================
// =====================================================
struct EstadoCaixa {
  float nivelPercentual;
  float nivelCm;
  bool vazaoEntrada;
  bool bombaAAtiva;
  bool bombaBAtiva;
  EstadoCaixaEnum estado;
  String ultimaAtualizacao;
};

// =====================================================
// ===================== VARIÁVEIS GLOBAIS ===============
// =====================================================
extern bool hasInternet;
extern bool modoAP;

extern EstadoCaixa estadoAtual;

extern bool chaveManualAtiva;
extern bool botaoEmergenciaPressionado;

extern float vazaoTotalDiaria;

extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

extern bool alerta110Enviado;
extern bool alerta115Enviado;
extern bool alertaExtremoEnviado;

// =====================================================
// ============== CONTROLE CAIXA D’ÁGUA =================
// =====================================================
#define ALTU_CAIXAT 200.0
#define BOIA_CAIXAT 30.0
#define ALTURA_UTIL_CM (ALTU_CAIXAT - BOIA_CAIXAT)
#define LADO_CAIXAT 355.0
#define PROF_CAIXAT 415.0
#define VOLUME_TOTAL_L ((ALTURA_UTIL_CM * LADO_CAIXAT * PROF_CAIXAT) / 1000.0)
//lateral 355 profundidade 415 altura 200 boia 30 

#define VAZAO_VAZAMENTO_CRITICO 60.0
#define NIVEL_DESLIGA_EMERGENCIA_CM 10.0

extern float vazaoCalculada;
extern float ultimoNivelVazao;
extern unsigned long ultimoTempoVazao;

extern unsigned long tUltimaMensagemVazamento;

// =====================================================
// ===================== WEB SERVER =====================
// =====================================================
#define TEMPO_SESSAO_MS (15UL * 60UL * 1000UL)
#define TEMPO_TIMEOUT_SESSAO TEMPO_SESSAO_MS
#define WEB_SERVER_PORT 3000

extern WebServer server;

extern bool autenticado;
extern bool sessaoAtiva;

extern String usuarioLogado;
extern String nivelAcessoLogado;

extern NivelAcesso nivelAcessoLogadoEnum;

extern unsigned long ultimaAtividadeSessao;

// =====================================================
// ===================== PROTÓTIPOS =====================
// =====================================================
void inicializarWebServer();
void loopWebServer();

float calcularNivelAgua();
float calcularVolume();
String getEstadoString();

void registrarAviso(String tipo, String mensagem, String usuario);

// ======================= SPIFFS =======================
void inicializarSPIFFS();
bool verificarArquivosEssenciais();

// ======================= USUÁRIOS =====================
void inicializarBancoUsuarios();
bool autenticarUsuario(String usuario, String senha);
void listarUsuariosSerial();

// ======================= API ==========================
void processarLogin();
void processarLogout();
void apiListarUsuarios();
void apiCriarUsuario();
void apiExcluirUsuario();

// ======================= SEGURANÇA ====================
bool exigirNivel(NivelAcesso minimo);

// ======================= SERIAL =======================
void serialMonitor(const String& texto);

#endif
