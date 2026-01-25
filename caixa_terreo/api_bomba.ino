//api_bomba.ino
#include "variaveis.h"

void processarControleBombas() {

  if (!autenticado) {
    server.send(401, "application/json",
      "{\"success\":false,\"msg\":\"Não autenticado\"}");
    return;
  }

  if (!server.hasArg("comando")) {
    server.send(400, "application/json",
      "{\"success\":false,\"msg\":\"Comando ausente\"}");
    return;
  }

  String comando = server.arg("comando");
  String usuario = usuarioLogado; // 👈 essencial

 if (!exigeNivel(ACESSO_SINDICO)) {
  server.send(403, "application/json",
              "{\"success\":false,\"msg\":\"Permissão negada\"}");
  return;
}

  if (comando == "LIGAR_A") {
    ligarBomba('A', usuario);
  }
  else if (comando == "LIGAR_B") {
    ligarBomba('B', usuario);
  }
  else if (comando == "LIGAR_AB") {
    ligarAmbasBombas(usuario);
  }
  else if (comando == "DESLIGAR") {
    desligarAmbasBombas(usuario);
  }
  else {
    server.send(400, "application/json",
      "{\"success\":false,\"msg\":\"Comando inválido\"}");
    return;
  }

  server.send(200, "application/json",
    "{\"success\":true,\"msg\":\"Comando executado\"}");
}
