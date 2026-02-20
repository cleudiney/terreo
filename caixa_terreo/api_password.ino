#include "variaveis.h"

static String extrairCampoJsonSenha(const String& body, const String& chave) {
  String chaveBusca = "\"" + chave + "\"";
  int pChave = body.indexOf(chaveBusca);
  if (pChave < 0) return "";

  int pDoisPontos = body.indexOf(':', pChave);
  if (pDoisPontos < 0) return "";

  int pAspasIni = body.indexOf('"', pDoisPontos + 1);
  if (pAspasIni < 0) return "";

  int pAspasFim = body.indexOf('"', pAspasIni + 1);
  if (pAspasFim < 0) return "";

  return body.substring(pAspasIni + 1, pAspasFim);
}

static bool atualizarSenhaPorPerfil(
  const String& perfil,
  const String& novaSenha
) {
  File f = SPIFFS.open(ARQ_USUARIOS, FILE_READ);
  if (!f) return false;

  String novoConteudo = "";
  bool atualizado = false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String usuario = linha.substring(0, p1);
    String senha = linha.substring(p1 + 1, p2);
    String perfilLinha = linha.substring(p2 + 1);

    if (!atualizado && perfilLinha == perfil) {
      senha = novaSenha;
      atualizado = true;
    }

    novoConteudo += usuario + ";" + senha + ";" + perfilLinha + "\n";
  }
  f.close();

  if (!atualizado) return false;

  File w = SPIFFS.open(ARQ_USUARIOS, FILE_WRITE);
  if (!w) return false;
  w.print(novoConteudo);
  w.close();

  return true;
}

void apiSetPassword() {
  if (!autenticado || !sessaoAtiva || nivelAcessoLogadoEnum < ACESSO_SUBSINDICO) {
    server.send(403, "application/json", "{\"ok\":false,\"erro\":\"acesso negado\"}");
    return;
  }

  String body = server.arg("plain");
  String tipo = extrairCampoJsonSenha(body, "tipo");
  String senha = extrairCampoJsonSenha(body, "senha");

  if (tipo.isEmpty() || senha.isEmpty()) {
    server.send(400, "application/json", "{\"ok\":false,\"erro\":\"dados invalidos\"}");
    return;
  }

  if (senha.length() < 4) {
    server.send(400, "application/json", "{\"ok\":false,\"erro\":\"senha muito curta\"}");
    return;
  }

  String perfilAlvo;
  if (tipo == "admin") perfilAlvo = "admin";
  else if (tipo == "sindico") perfilAlvo = "sindico";
  else if (tipo == "subsindico") perfilAlvo = "subsindico";
  else if (tipo == "zelador") perfilAlvo = "zelador";
  else if (tipo == "visitante") perfilAlvo = "morador";
  else {
    server.send(400, "application/json", "{\"ok\":false,\"erro\":\"tipo invalido\"}");
    return;
  }

  bool ok = atualizarSenhaPorPerfil(perfilAlvo, senha);
  if (!ok && tipo == "visitante") {
    ok = adicionarUsuario("visitante", senha, "morador");
  }
  if (!ok && tipo == "subsindico") {
    ok = adicionarUsuario("subsindico", senha, "subsindico");
  }

  if (!ok) {
    server.send(404, "application/json", "{\"ok\":false,\"erro\":\"perfil nao encontrado\"}");
    return;
  }

  registrarAviso("evento", "Senha alterada para perfil " + tipo, usuarioLogado);
  server.send(200, "application/json", "{\"ok\":true}");
}
