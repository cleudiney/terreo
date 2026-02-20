//usuarios.ino
#include "variaveis.h"

// =====================================================
// AUTENTICAÇÃO
// =====================================================
bool autenticarUsuario(String usuario, String senha) {
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String u = linha.substring(0, p1);
    String s = linha.substring(p1 + 1, p2);

    if (u == usuario && s == senha) {
      usuarioLogado = usuario;
      autenticado = true;

      registrarAviso("evento", "Login realizado", usuario);
      f.close();
      return true;
    }
  }

  f.close();
  return false;
}

// =====================================================
// LOGOUT
// =====================================================
void logoutUsuario() {
  if (autenticado) {
    registrarAviso("evento", "Logout realizado", usuarioLogado);
  }

  autenticado = false;
  usuarioLogado = "";
}

// =====================================================
// LISTAR USUÁRIOS (DEBUG / ADMIN)
// =====================================================
void listarUsuariosSerial() {
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) {
    Serial.println("❌ usuarios.dat não encontrado");
    return;
  }

  Serial.println("👥 Usuários cadastrados:");
  while (f.available()) {
    Serial.println("  " + f.readStringUntil('\n'));
  }
  f.close();
}

// =====================================================
// INICIALIZAÇÃO DO BANCO DE USUÁRIOS (NOME CORRIGIDO!)
// =====================================================
void inicializarBancoUsuarios() {
  if (!SPIFFS.exists("/dados/usuarios.dat")) {
    File f = SPIFFS.open("/dados/usuarios.dat", FILE_WRITE);
    f.println("adm;aquarios2025!;admin");
    f.println("sindico;aquarios23!;sindico");
    f.println("subsindico;aquarios23!;subsindico");
    f.println("zelador;aquarios9999#;zelador");
    f.close();

    Serial.println("✅ usuarios.dat criado com usuários padrão");
  } else {
    Serial.println("✅ usuarios.dat encontrado");
  }

  listarUsuariosSerial();
}

// =====================================================
// VERIFICAR PERMISSÕES (OPCIONAL)
// =====================================================
bool usuarioTemPermissao(String usuario, String permissao) {
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String u = linha.substring(0, p1);
    String p = linha.substring(p2 + 1);
    
    if (u == usuario && p == permissao) {
      f.close();
      return true;
    }
  }

  f.close();
  return false;
}

// =====================================================
// ALTERAR SENHA
// =====================================================
bool alterarSenhaUsuario(String usuario, String senhaAntiga, String senhaNova) {
  if (!autenticarUsuario(usuario, senhaAntiga)) {
    return false;
  }

  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return false;

  String conteudoNovo = "";
  bool usuarioEncontrado = false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String u = linha.substring(0, p1);
    String s = linha.substring(p1 + 1, p2);
    String p = linha.substring(p2 + 1);

    if (u == usuario) {
      usuarioEncontrado = true;
      conteudoNovo += u + ";" + senhaNova + ";" + p + "\n";
      registrarAviso("evento", "Senha alterada para " + usuario, usuario);
    } else {
      conteudoNovo += linha + "\n";
    }
  }

  f.close();

  if (!usuarioEncontrado) return false;

  // Reescrever arquivo com senha atualizada
  File w = SPIFFS.open("/dados/usuarios.dat", FILE_WRITE);
  if (!w) return false;

  w.print(conteudoNovo);
  w.close();

  return true;
}

// =====================================================
// ADICIONAR NOVO USUÁRIO (APENAS ADMIN)
// =====================================================
bool adicionarUsuario(String usuario, String senha, String perfil) {
  // Verificar se usuário já existe
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    
    int p1 = linha.indexOf(';');
    if (p1 < 0) continue;
    
    String u = linha.substring(0, p1);
    if (u == usuario) {
      f.close();
      return false; // Usuário já existe
    }
  }
  f.close();

  // Adicionar novo usuário
  File a = SPIFFS.open("/dados/usuarios.dat", FILE_APPEND);
  if (!a) return false;

  a.println(usuario + ";" + senha + ";" + perfil);
  a.close();

  registrarAviso("sistema", "Novo usuário criado: " + usuario, "admin");
  return true;
}

// =====================================================
// REMOVER USUÁRIO (APENAS ADMIN)
// =====================================================
bool removerUsuario(String usuario) {
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return false;

  String conteudoNovo = "";
  bool usuarioEncontrado = false;

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    if (linha.isEmpty()) continue;

    int p1 = linha.indexOf(';');
    if (p1 < 0) continue;

    String u = linha.substring(0, p1);
    
    if (u == usuario) {
      usuarioEncontrado = true;
      registrarAviso("sistema", "Usuário removido: " + usuario, "admin");
      continue; // Não adiciona à nova lista
    }
    
    conteudoNovo += linha + "\n";
  }

  f.close();

  if (!usuarioEncontrado) return false;

  // Reescrever arquivo sem o usuário
  File w = SPIFFS.open("/dados/usuarios.dat", FILE_WRITE);
  if (!w) return false;

  w.print(conteudoNovo);
  w.close();

  return true;
}

// =====================================================
// OBTER PERFIL DO USUÁRIO LOGADO
// =====================================================
String obterPerfilUsuario(String usuario) {
  File f = SPIFFS.open("/dados/usuarios.dat", FILE_READ);
  if (!f) return "";

  while (f.available()) {
    String linha = f.readStringUntil('\n');
    linha.trim();
    
    int p1 = linha.indexOf(';');
    int p2 = linha.indexOf(';', p1 + 1);
    if (p1 < 0 || p2 < 0) continue;

    String u = linha.substring(0, p1);
    String p = linha.substring(p2 + 1);
    
    if (u == usuario) {
      f.close();
      return p;
    }
  }

  f.close();
  return "";
}
// =====================================================
// WRAPPERS PARA WEBSERVER
// (mantém compatibilidade com contrato)
// =====================================================

void processarLogin() {

  if (!server.hasArg("usuario") || !server.hasArg("senha")) {
    server.send(400, "application/json", "{\"erro\":\"Dados ausentes\"}");
    return;
  }

  String usuario = server.arg("usuario");
  String senha   = server.arg("senha");

  if (!autenticarUsuario(usuario, senha)) {
    server.send(401, "application/json", "{\"erro\":\"Credenciais inválidas\"}");
    return;
  }

  String nivel = obterPerfilUsuario(usuario);

  nivelAcessoLogadoEnum =
    nivel == "admin"   ? ACESSO_ADMIN :
    nivel == "sindico" ? ACESSO_SINDICO :
    nivel == "subsindico" ? ACESSO_SUBSINDICO :
    nivel == "zelador" ? ACESSO_ZELADOR :
                          ACESSO_NENHUM;

  sessaoAtiva = true;
  ultimaAtividadeSessao = millis();

  String json =
    "{"
    "\"status\":\"ok\","
    "\"usuario\":\"" + usuario + "\","
    "\"nivel\":\"" + nivel + "\","
    "\"token\":\"local\""
    "}";

String resposta = "{";
resposta += "\"success\":true,";
resposta += "\"data\":";
resposta += json;   // ou listaUsuarios
resposta += "}";

server.send(200, "application/json", resposta);
}

// =====================================================
// ===================== SEGURANÇA WEB ==================
// =====================================================
void processarLogout() {
  logoutUsuario();
  server.send(200, "application/json", "{\"status\":\"logout\"}");
}



//===================================================
