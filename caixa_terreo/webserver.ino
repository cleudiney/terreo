#include "variaveis.h"

static bool webserverInicializado = false;

// =====================================================
// MIME TYPES
// =====================================================
static String getContentType(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".css"))  return "text/css";
  if (path.endsWith(".js"))   return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png"))  return "image/png";
  if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".svg"))  return "image/svg+xml";
  if (path.endsWith(".ico"))  return "image/x-icon";
  return "text/plain";
}

// =====================================================
// CONTROLE DE ACESSO
// =====================================================
static bool rotaHtmlPublica(const String& path) {
  return (path == "/login.html" ||
          path == "/login-localstorage.html" ||
          path == "/login-zero.html");
}

static NivelAcesso obterNivelMinimoHtml(const String& path) {

  if (path == "/adm.html" ||
      path == "/admin.html" ||
      path == "/estatisticas.html")
    return ACESSO_SINDICO;

  if (path == "/index.html" ||
      path == "/avisos.html" ||
      path == "/historico.html" ||
      path == "/pontos.html" ||
      path == "/controle.html" ||
      path == "/registros.html")
    return ACESSO_ZELADOR;

  return ACESSO_MORADOR;
}

static bool podeAcessarHtml(const String& path) {

  if (rotaHtmlPublica(path)) return true;

  if (!autenticado || !sessaoAtiva)
    return false;

  NivelAcesso minimo = obterNivelMinimoHtml(path);
  return nivelAcessoLogadoEnum >= minimo;
}

// =====================================================
// SERVIDOR DE ARQUIVOS COM CONTROLE
// =====================================================
static void servirArquivo(const String& path) {

  if (path.endsWith(".html") && !podeAcessarHtml(path)) {

    if (!autenticado || !sessaoAtiva) {
      server.sendHeader("Location", "/login.html", true);
      server.send(302, "text/plain", "Redirecting...");
      return;
    }

    server.send(403, "application/json", "{\"erro\":\"Acesso negado\"}");
    return;
  }

  if (!SPIFFS.exists(path)) {
    server.send(404, "application/json", "{\"erro\":\"Arquivo não encontrado\"}");
    return;
  }

  File file = SPIFFS.open(path, "r");
  if (!file) {
    server.send(500, "application/json", "{\"erro\":\"Falha ao abrir arquivo\"}");
    return;
  }

  server.streamFile(file, getContentType(path));
  file.close();
}

// =====================================================
// INICIALIZAÇÃO
// =====================================================
void inicializarWebServer() {

  if (webserverInicializado) {
    Serial.println("🌐 WebServer já inicializado");
    return;
  }

  Serial.println("🌐 Inicializando WebServer...");

  // ================= API =================
  server.on("/api/login", HTTP_POST, processarLogin);
  server.on("/api/logout", HTTP_POST, processarLogout);
  server.on("/api/usuarios", HTTP_GET, apiListarUsuarios);
  server.on("/api/usuarios", HTTP_POST, apiCriarUsuario);
  server.on("/api/usuarios", HTTP_DELETE, apiExcluirUsuario);
  server.on("/api/status", HTTP_GET, apiStatus);

  // ================= ROTAS =================
  server.on("/", HTTP_GET, []() {

    if (!autenticado) {
      server.sendHeader("Location", "/login.html", true);
      server.send(302, "text/plain", "");
      return;
    }

    server.sendHeader("Location", "/index.html", true);
    server.send(302, "text/plain", "");
  });

  // HTML controlado
  server.on("/index.html", HTTP_GET, []() { servirArquivo("/index.html"); });
  server.on("/adm.html", HTTP_GET, []() { servirArquivo("/adm.html"); });
  server.on("/admin.html", HTTP_GET, []() { servirArquivo("/admin.html"); });
  server.on("/avisos.html", HTTP_GET, []() { servirArquivo("/avisos.html"); });
  server.on("/controle.html", HTTP_GET, []() { servirArquivo("/controle.html"); });
  server.on("/estatisticas.html", HTTP_GET, []() { servirArquivo("/estatisticas.html"); });
  server.on("/historico.html", HTTP_GET, []() { servirArquivo("/historico.html"); });
  server.on("/pontos.html", HTTP_GET, []() { servirArquivo("/pontos.html"); });
  server.on("/registros.html", HTTP_GET, []() { servirArquivo("/registros.html"); });
  server.on("/login.html", HTTP_GET, []() { servirArquivo("/login.html"); });
  server.on("/login-zero.html", HTTP_GET, []() { servirArquivo("/login-zero.html"); });
  server.on("/login-localstorage.html", HTTP_GET, []() { servirArquivo("/login-localstorage.html"); });

  // ================= ESTÁTICOS SEGUROS =================
  server.serveStatic("/css/", SPIFFS, "/css/");
  server.serveStatic("/js/", SPIFFS, "/js/");
  server.serveStatic("/partials/", SPIFFS, "/partials/");

  // ================= FALLBACK =================
  server.onNotFound([]() {
    server.send(404, "application/json",
                "{\"erro\":\"Rota não encontrada\"}");
  });

  server.begin();
  webserverInicializado = true;

  Serial.println("✅ WebServer iniciado com sucesso");
}

// =====================================================
// LOOP
// =====================================================
void loopWebServer() {

  server.handleClient();

  if (sessaoAtiva &&
      millis() - ultimaAtividadeSessao > TEMPO_TIMEOUT_SESSAO) {

    sessaoAtiva = false;
    autenticado = false;
    usuarioLogado = "";
    nivelAcessoLogadoEnum = ACESSO_NENHUM;

    Serial.println("⏱ Sessão expirada automaticamente");
  }
}
