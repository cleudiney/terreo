#include "variaveis.h"

// =====================================================
// API — LISTAR USUÁRIOS
// GET /api/usuarios
// =====================================================
void apiListarUsuarios() {

  if (!exigirNivel(ACESSO_ZELADOR)) return;

  File f = SPIFFS.open(ARQ_USUARIOS, FILE_READ);
  if (!f) {
    server.send(500, "application/json", "{\"erro\":\"arquivo nao encontrado\"}");
    return;
  }

  String json = "[";
  bool primeiro = true;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String usuario = linha.substring(0, p1);
    String perfil  = linha.substring(p2 + 1);

    if (!primeiro) json += ",";
    primeiro = false;

    json += "{\"usuario\":\"" + usuario + "\",\"perfil\":\"" + perfil + "\"}";
  }

  json += "]";
  f.close();
    
    String resposta = "{";
    resposta += "\"success\":true,";
    resposta += "\"data\":";
    resposta += json;   // ou listaUsuarios
    resposta += "}";
    
    server.send(200, "application/json", resposta);
}
//=======================================
bool exigirNivel(NivelAcesso minimo) {

  // não autenticado
  if (!autenticado || !sessaoAtiva) {
    server.send(
      401,
      "application/json",
      "{\"erro\":\"nao autenticado\"}"
    );
    return false;
  }

  // nível insuficiente
  if (nivelAcessoLogadoEnum < minimo) {
    server.send(
      403,
      "application/json",
      "{\"erro\":\"acesso negado\"}"
    );
    return false;
  }

  // sessão válida → renova atividade
  ultimaAtividadeSessao = millis();
  return true;
}

// =====================================================
// API — CRIAR USUÁRIO
// POST /api/usuarios
// =====================================================
void apiCriarUsuario() {
    if (!exigirNivel(ACESSO_ADMIN)) return;

  if (!server.hasArg("usuario") ||
      !server.hasArg("senha") ||
      !server.hasArg("perfil")) {

    server.send(400, "application/json",
      "{\"erro\":\"Parâmetros ausentes\"}");
    return;
  }

  String usuario = server.arg("usuario");
  String senha   = server.arg("senha");
  String perfil  = server.arg("perfil");

  if (!adicionarUsuario(usuario, senha, perfil)) {
    server.send(409, "application/json",
      "{\"erro\":\"Usuário já existe\"}");
    return;
  }

  server.send(200, "application/json",
    "{\"status\":\"usuario criado\"}");
}

// =====================================================
// API — EXCLUIR USUÁRIO
// DELETE /api/usuarios
// =====================================================

void apiExcluirUsuario() {

  if (!exigirNivel(ACESSO_ADMIN)) return;

  if (!server.hasArg("usuario")) {
    server.send(400, "application/json",
                "{\"erro\":\"usuario nao informado\"}");
    return;
  }

  bool ok = removerUsuario(server.arg("usuario"));

  if (ok)
    server.send(200, "application/json", "{\"status\":\"removido\"}");
  else
    server.send(404, "application/json", "{\"erro\":\"usuario nao encontrado\"}");
}
