// avisos.ino
#include "variaveis.h"

// =====================================================
// REGISTRAR AVISO
// =====================================================
void registrarAviso(String tipo, String mensagem, String usuario = "") {

  // ===============================
  // Usuário automático
  // ===============================
  if (usuario.isEmpty() && autenticado) {
    usuario = usuarioLogado;
  }

  // ===============================
  // Data e hora
  // ===============================
  String dataHora = getDataHoraAtual();

  // ===============================
  // Montagem da linha
  // ===============================
  String linha = dataHora + ";" + tipo + ";" + mensagem;

  if (!usuario.isEmpty()) {
    linha += ";" + usuario;
  }

  linha += "\n";

  // ===============================
  // Grava no arquivo
  // ===============================
  File f = SPIFFS.open("/avisos.dat", FILE_APPEND);
  if (!f) {
    Serial.println("❌ Erro ao abrir avisos.dat");
    return;
  }

  f.print(linha);
  f.close();

  // ===============================
  // 🔔 DISPARO DE NOTIFICAÇÃO
  // ===============================
  processarNotificacao(tipo, mensagem);
}


// =====================================================
// CONTADOR DE AVISOS
// =====================================================
int contarAvisos() {
  File f = SPIFFS.open("/avisos.dat", FILE_READ);
  if (!f) return 0;

  int total = 0;
  while (f.available()) {
    if (f.read() == '\n') total++;
  }

  f.close();
  return total;
}

// =====================================================
// LIMPAR AVISOS ANTIGOS
// =====================================================
void limparAvisosAntigos(int maxLinhas) {
  File f = SPIFFS.open("/avisos.dat", FILE_READ);
  if (!f) return;

  std::vector<String> linhas;
  while (f.available()) {
    linhas.push_back(f.readStringUntil('\n'));
  }
  f.close();

  if ((int)linhas.size() <= maxLinhas) return;

  File w = SPIFFS.open("/avisos.dat", FILE_WRITE);
  for (int i = linhas.size() - maxLinhas; i < (int)linhas.size(); i++) {
    w.println(linhas[i]);
  }
  w.close();
}

// =====================================================
// TIPOS DE AVISO PRÉ-DEFINIDOS
// =====================================================
void avisoUrgente(String msg) {
  registrarAviso("urgente", msg, "");
}

void avisoCritico(String msg) {
  registrarAviso("critica", msg, "");
}

void avisoResolvido(String msg) {
  registrarAviso("resolvido", msg, "");
}

// =====================================================
// API - LISTAR AVISOS (WebServer)
// =====================================================


// =====================================================
// INICIALIZAÇÃO DO SISTEMA DE AVISOS
// =====================================================
void inicializarSistemaAvisos() {

  if (!SPIFFS.exists("/avisos.dat")) {
    File f = SPIFFS.open("/avisos.dat", FILE_WRITE);
    f.close();
  }

  //registrarAviso("sistema", "Sistema inicializado / reboot", "sistema");

  int total = contarAvisos();
  Serial.printf("✅ Sistema de avisos OK (%d avisos registrados)\n", total);

  if (total > 1000) {
    Serial.println("⚠️ Muitos avisos registrados. Considere limpeza.");
  }
}
