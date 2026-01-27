#ifndef VARIAVEIS_H
#define VARIAVEIS_H

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WebServer.h>   // ✅ TEM QUE FICAR AQUI

// =====================================================
// ===================== DEFINIÇÕES BÁSICAS =============
// =====================================================
#define DIR_DADOS      "/dados"
#define ARQ_USUARIOS   "/dados/usuarios.dat"
#define ARQ_AVISOS     "/dados/avisos.dat"
#define ARQ_REGISTROS  "/dados/registros.dat"

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

#define NIVEL_CRITICO          20.0
#define NIVEL_ALERTA           40.0
#define NIVEL_NORMAL           60.0

//=================CALLMEBOT==================
extern const String CALLMEBOT_TEL1;
extern const String CALLMEBOT_APIKEY1;

extern const String CALLMEBOT_TEL2;
extern const String CALLMEBOT_APIKEY2;

extern const String CALLMEBOT_TEL3;
extern const String CALLMEBOT_APIKEY3;

// ======================
// DUCKDNS
// ======================
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

// ⚠️ MANTIDO, MAS AINDA NÃO USADO PELO SISTEMA ATUAL
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
  float nivelPercentual = 100.0;
  float nivelCm = 0.0;
  bool vazaoEntrada = false;
  bool bombaAAtiva = false;
  bool bombaBAtiva = false;
  EstadoCaixaEnum estado = CAIXA_ESTAVEL;
  String ultimaAtualizacao = "";
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

//=====================CONTROLE_CAIXA_AGUA ===============
// =====================================
// PARÂMETROS DA CAIXA
// =====================================
#define ALTURA_UTIL_CM 180.0      // ajuste depois
#define VOLUME_TOTAL_L 20000.0    // ajuste depois

#define VAZAO_VAZAMENTO_CRITICO 60.0   // L/min
#define NIVEL_DESLIGA_EMERGENCIA_CM 10.0


// =================================
// VAZÃO CALCULADA e CAIXA DE AGUA
// =================================
extern float vazaoCalculada;
extern float ultimoNivelVazao;
extern unsigned long ultimoTempoVazao;

extern unsigned long tUltimaMensagemVazamento;
extern EstadoCaixa estadoAtual;


// =====================================================
// ===================== WEB SERVER / SESSÃO =============
// =====================================================
// =====================================================
// TEMPO DE SESSÃO WEB
// =====================================================
#define TEMPO_SESSAO_MS (15UL * 60UL * 1000UL)  
// 15 minutos
// compatibilidade interna (alias)
#define TEMPO_TIMEOUT_SESSAO TEMPO_SESSAO_MS
// Porta do servidor 3000
#define WEB_SERVER_PORT 3000
extern WebServer server;

extern bool autenticado;
extern bool sessaoAtiva;
extern String usuarioLogado;

// ⚠️ MANTIDO COMO STRING PARA NÃO QUEBRAR usuarios.ino
extern String nivelAcessoLogado;

extern unsigned long ultimaAtividadeSessao;

extern WebServer server;

extern bool autenticado;
extern bool sessaoAtiva;
extern String usuarioLogado;

// ⚠️ MANTIDO COMO STRING PARA NÃO QUEBRAR usuarios.ino
extern String nivelAcessoLogado;

// ✅ NOVO — enum para uso futuro (ainda NÃO usado)
extern NivelAcesso nivelAcessoLogadoEnum;

extern unsigned long ultimaAtividadeSessao;

// =====================================================
// ===================== PROTÓTIPOS =====================
// =====================================================
// =====================================================
// WEBSERVER
// =====================================================
void inicializarWebServer();
void loopWebServer();
float calcularNivelAgua();
float calcularVolume();
String getEstadoString();
void registrarAviso(String tipo, String mensagem, String usuario);

//====================== SPIFFS =====================
void inicializarSPIFFS();
bool verificarArquivosEssenciais();

// Usuários
void inicializarBancoUsuarios();
bool autenticarUsuario(String usuario, String senha);
void listarUsuariosSerial();

// API / Web
void processarLogin();
void processarLogout();
void apiListarUsuarios();
void apiCriarUsuario();
void apiExcluirUsuario();

// =====================================================
// ===================== SEGURANÇA WEB ==================
// =====================================================

// Verifica autenticação + nível mínimo de acesso
bool exigirNivel(NivelAcesso minimo);

// =====================================================
// ===================== WEB AUTH =======================
// =====================================================
void processarLogin();
void processarLogout();
// =======================
// SERIAL MONITOR GLOBAL
// =======================
void serialMonitor(const String& texto);


#endif // VARIAVEIS_H
