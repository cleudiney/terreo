// api_status.ino
#include "variaveis.h"

// =====================================================
// API — STATUS ATUAL DO SISTEMA
// =====================================================
void apiStatus() {

  if (!autenticado) {
    server.send(401, "application/json",
                "{\"erro\":\"nao autenticado\"}");
    return;
  }

  String json = "{";

  // ============================
  // USUÁRIO
  // ============================
  json += "\"usuario\":\"" + usuarioLogado + "\",";

  // hora simples (sem NTP obrigatório)
  json += "\"datahora\":\"" + String(millis() / 1000) + "s\",";

  // ============================
  // OBJETO CAIXA
  // ============================
  json += "\"caixa\":{";

  json += "\"nivelPercentual\":"
       + String(estadoAtual.nivelPercentual, 1) + ",";

  json += "\"nivelCm\":"
       + String(estadoAtual.nivelCm, 1) + ",";

  json += "\"bombaA\":"
       + String(estadoAtual.bombaAAtiva ? "true" : "false") + ",";

  json += "\"bombaB\":"
       + String(estadoAtual.bombaBAtiva ? "true" : "false") + ",";

  json += "\"vazaoEntrada\":"
       + String(estadoAtual.vazaoEntrada ? "true" : "false");

  json += "}"; // caixa

  json += "}"; // json

String resposta = "{";
resposta += "\"success\":true,";
resposta += "\"data\":";
resposta += json;   // ou listaUsuarios
resposta += "}";

server.send(200, "application/json", resposta);

}
