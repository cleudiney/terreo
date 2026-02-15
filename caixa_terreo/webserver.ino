// =====================================================
// webserver.ino
// Autor: Oficina de Robótica - IBHF
// Data: 2026-01-15
// Motivo:
//  - Integrar WebServer ao sistema EXISTENTE
//  - Usar autenticação já implementada em usuarios.ino
//  - NÃO alterar lógica antiga
// =====================================================

#include "variaveis.h"

// =====================================================
// CONTROLE INTERNO DO MÓDULO
// =====================================================
// [ADIÇÃO]
// Evita reinicialização dupla do servidor
static bool webserverInicializado = false;

// =====================================================
// INICIALIZAÇÃO DO WEBSERVER
// =====================================================
void inicializarWebServer() {

  // [PROTEÇÃO]
  if (webserverInicializado) {
    Serial.println("🌐 WebServer já inicializado");
    return;
  }

  Serial.println("🌐 Inicializando WebServer...");

  // ===================================================
  // ROTAS DE AUTENTICAÇÃO
  // ===================================================
  // [JÁ EXISTEM EM usuarios.ino]
  server.on("/api/login", HTTP_POST, processarLogin);
  server.on("/api/logout", HTTP_POST, processarLogout);

  // ===================================================
  // ROTAS DE USUÁRIOS (ADMIN / SÍNDICO)
  // ===================================================
  server.on("/api/usuarios", HTTP_GET, apiListarUsuarios);
  server.on("/api/usuarios", HTTP_POST, apiCriarUsuario);
  server.on("/api/usuarios", HTTP_DELETE, apiExcluirUsuario);

  // ===================================================
  // ROTAS DE PÁGINAS
  // ===================================================

  // Página raiz
  server.on("/", HTTP_GET, []() {
    if (!autenticado) {
      server.sendHeader("Location", "/login.html", true);
      server.send(302, "text/plain", "");
      return;
    }

  });

  // Página de login
  server.on("/login.html", HTTP_GET, []() {
      File file = SPIFFS.open("/login-localstorage.html", "r");
      server.streamFile(file, "text/html");
      file.close();
  });

  // ===================================================
  // ARQUIVOS ESTÁTICOS (CSS / JS / IMG)
  // ===================================================
  // [IMPORTANTE]
  // Isso NÃO interfere nas rotas acima
  server.serveStatic("/", SPIFFS, "/");

  // ===================================================
  // FALLBACK (DEBUG / API)
  // ===================================================
  server.onNotFound([]() {
    server.send(404, "application/json",
                "{\"erro\":\"Rota não encontrada\"}");
  });

  // ===================================================
  // START
  // ===================================================
  server.begin();
  webserverInicializado = true;
  server.on("/api/status", HTTP_GET, apiStatus);

  Serial.println("✅ WebServer iniciado com sucesso");
}

// =====================================================
// LOOP DO WEBSERVER
// =====================================================
// [ANTI-WDT]
// Este loop NÃO bloqueia o sistema principal
void loopWebServer() {

  server.handleClient();

  if (sessaoAtiva) {
    if (millis() - ultimaAtividadeSessao > TEMPO_TIMEOUT_SESSAO) {
      sessaoAtiva = false;
      autenticado = false;
      usuarioLogado = "";
      nivelAcessoLogadoEnum = ACESSO_NENHUM;

      Serial.println("⏱ Sessão expirada automaticamente");
    }
  }
}
