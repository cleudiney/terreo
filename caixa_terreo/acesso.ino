// =====================================================
// acesso.ino
// Autor: Oficina de Robótica - IBHF
// Data: 2026-01-16
// Motivo:
//  - Converter perfil STRING para enum NivelAcesso
//  - Manter compatibilidade com usuarios.dat atual
// =====================================================

#include "variaveis.h"

// (Incluídos por segurança / clareza do módulo)
#include <WebServer.h>
#include <SPIFFS.h>

// =====================================================
// CONVERSÃO STRING → ENUM
// =====================================================
NivelAcesso converterNivelAcesso(const String& nivelStr) {

  if (nivelStr == "admin")      return ACESSO_ADMIN;
  if (nivelStr == "sindico")    return ACESSO_SINDICO;
  if (nivelStr == "subsindico") return ACESSO_SUBSINDICO;
  if (nivelStr == "zelador")    return ACESSO_ZELADOR;
  if (nivelStr == "morador")    return ACESSO_MORADOR;

  return ACESSO_NENHUM;
}
