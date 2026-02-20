#include "variaveis.h"

struct EstadoDisparosDia {
  String dataRef;
  String dataUltimoReset;
  uint32_t horasStatusEnviadas;
  bool avisoAbaixo50SemVazao;
  bool avisoAbaixo50Enchendo;
  bool emergenciaAbaixo40SemVazao;
  bool emergenciaAbaixo20SemVazao;
};

class DisparosManager {
public:
  void begin() {
    garantirArquivos();
    carregarEstado();
    alinharDataAtual();
  }

  void tick() {
    static unsigned long ultimaAvaliacao = 0;
    if (millis() - ultimaAvaliacao < 1000) return;
    ultimaAvaliacao = millis();

    alinharDataAtual();
    avaliarRegras();
    salvarEstado();
  }

private:
  static constexpr const char* ARQ_ESTADO = "/disparos_estado.dat";
  static constexpr const char* ARQ_HISTORICO = "/disparos_historico.dat";
  static constexpr const char* ARQ_HISTORICO_DEMO = "/historico.dat";
  static constexpr const char* ARQ_REGISTROS_DEMO = "/registros.dat";

  EstadoDisparosDia estado = {
    "", "", 0UL,
    false, false, false, false
  };

  void garantirArquivos() {
    if (!SPIFFS.exists(ARQ_ESTADO)) {
      File f = SPIFFS.open(ARQ_ESTADO, FILE_WRITE);
      if (f) f.close();
    }
    if (!SPIFFS.exists(ARQ_HISTORICO)) {
      File f = SPIFFS.open(ARQ_HISTORICO, FILE_WRITE);
      if (f) f.close();
    }
    if (!SPIFFS.exists(ARQ_HISTORICO_DEMO)) {
      File f = SPIFFS.open(ARQ_HISTORICO_DEMO, FILE_WRITE);
      if (f) f.close();
    }
    if (!SPIFFS.exists(ARQ_REGISTROS_DEMO)) {
      File f = SPIFFS.open(ARQ_REGISTROS_DEMO, FILE_WRITE);
      if (f) f.close();
    }
  }

  void resetarFlagsDoDia(const String& hoje) {
    estado.dataRef = hoje;
    estado.horasStatusEnviadas = 0UL;
    estado.avisoAbaixo50SemVazao = false;
    estado.avisoAbaixo50Enchendo = false;
    estado.emergenciaAbaixo40SemVazao = false;
    estado.emergenciaAbaixo20SemVazao = false;
  }

  void alinharDataAtual() {
    String hoje = getDataAtual();
    String hora = getHoraAtual();

    if (hora == "23:59" && estado.dataUltimoReset != hoje) {
      resetarFlagsDoDia(hoje);
      estado.dataUltimoReset = hoje;
      salvarEstado();
      return;
    }

    if (estado.dataRef != hoje) {
      resetarFlagsDoDia(hoje);
      salvarEstado();
    }
  }

  bool horaEhProgramada(int h) const {
    if (h == 5) return true;
    if (h >= 6 && h <= 8) return true;
    if (h >= 10 && h <= 16 && (h % 2 == 0)) return true;
    if (h >= 18 && h <= 23) return true;
    return false;
  }

  String montarDetalhes() const {
    String msg;
    msg += "Dia: " + getDataAtual() + "\n";
    msg += "Hora: " + getHoraAtual() + "\n";
    msg += "Nivel: " + String(estadoAtual.nivelPercentual, 1) + "%\n";
    msg += "Boia ativa: ";
    msg += estadoAtual.vazaoEntrada ? "SIM" : "NAO";
    return msg;
  }

  String faixaHoraByInt(int h) const {
    if (h >= 5 && h <= 7) return "amanhecer";
    if (h >= 8 && h <= 10) return "manha";
    if (h >= 11 && h <= 13) return "almoco";
    if (h >= 14 && h <= 16) return "tarde";
    if (h >= 17 && h <= 19) return "anoitecer";
    if (h >= 20 && h <= 23) return "noite";
    return "madrugada";
  }

  void registrarHistorico(const String& evento, bool emergencia) {
    File f = SPIFFS.open(ARQ_HISTORICO, FILE_APPEND);
    if (!f) return;

    String linha;
    linha += getDataAtual() + ";";
    linha += getHoraAtual() + ";";
    linha += evento + ";";
    linha += String(estadoAtual.nivelPercentual, 1) + ";";
    linha += estadoAtual.vazaoEntrada ? "true" : "false";
    linha += ";";
    linha += emergencia ? "true" : "false";
    linha += "\n";

    f.print(linha);
    f.close();

    File hf = SPIFFS.open(ARQ_HISTORICO_DEMO, FILE_APPEND);
    if (hf) {
      hf.print(linha);
      hf.close();
    }

    File rf = SPIFFS.open(ARQ_REGISTROS_DEMO, FILE_APPEND);
    if (rf) {
      String estado = getEstadoString();
      String faixa = faixaHoraByInt(getHoraInt());
      String reg =
        getDataAtual() + ";" +
        getHoraAtual() + ";" +
        String(estadoAtual.nivelPercentual, 1) + ";" +
        String(estadoAtual.nivelCm, 1) + ";" +
        estado + ";" + faixa + "\n";
      rf.print(reg);
      rf.close();
    }
  }

  void dispararEmergenciaSeNovo(
    bool& flagEvento,
    const String& tipoAviso,
    const String& evento,
    const String& mensagem
  ) {
    if (flagEvento) return;
    flagEvento = true;

    registrarHistorico(evento, true);
    registrarAviso(tipoAviso, mensagem + "\n" + montarDetalhes(), "sistema");
  }

  void avaliarRegras() {
    int h = getHoraInt();
    if (h >= 0 && h <= 23 && horaEhProgramada(h)) {
      uint32_t bitHora = (1UL << h);
      if ((estado.horasStatusEnviadas & bitHora) == 0UL) {
        estado.horasStatusEnviadas |= bitHora;
        registrarHistorico("status_programado", false);
        enviarEvento("Status programado do sistema\n" + montarDetalhes());
      }
    }

    bool abaixo50 = estadoAtual.nivelPercentual <= 50.0f;
    bool semVazao = !estadoAtual.vazaoEntrada;

    if (abaixo50 && semVazao && !estado.avisoAbaixo50SemVazao) {
      estado.avisoAbaixo50SemVazao = true;
      registrarHistorico("abaixo50_sem_vazao", false);
      enviarUrgente("Caixa abaixo de 50% sem fornecimento\n" + montarDetalhes());
    }

    if (abaixo50 && !semVazao && !estado.avisoAbaixo50Enchendo) {
      estado.avisoAbaixo50Enchendo = true;
      registrarHistorico("abaixo50_enchendo", false);
      enviarEvento("Boia ativa: caixa abaixo de 50% e enchendo\n" + montarDetalhes());
    }

    if (!semVazao && estadoAtual.nivelPercentual > 55.0f) {
      estado.avisoAbaixo50SemVazao = false;
      estado.avisoAbaixo50Enchendo = false;
    }

    if (semVazao && estadoAtual.nivelPercentual < NIVEL_ALERTA) {
      dispararEmergenciaSeNovo(
        estado.emergenciaAbaixo40SemVazao,
        "urgente",
        "emergencia_abaixo40_sem_vazao",
        "Sem vazao e nivel abaixo de alerta (" + String(NIVEL_ALERTA, 0) + "%)"
      );
    } else if (estadoAtual.nivelPercentual >= (NIVEL_ALERTA + 5.0f)) {
      estado.emergenciaAbaixo40SemVazao = false;
    }

    if (semVazao && estadoAtual.nivelPercentual < NIVEL_CRITICO) {
      dispararEmergenciaSeNovo(
        estado.emergenciaAbaixo20SemVazao,
        "critica",
        "emergencia_abaixo20_sem_vazao",
        "Nivel critico abaixo de " + String(NIVEL_CRITICO, 0) + "% sem vazao"
      );
    } else if (estadoAtual.nivelPercentual >= (NIVEL_CRITICO + 5.0f)) {
      estado.emergenciaAbaixo20SemVazao = false;
    }
  }

  void carregarEstado() {
    File f = SPIFFS.open(ARQ_ESTADO, FILE_READ);
    if (!f) return;

    String linha = f.readStringUntil('\n');
    f.close();
    linha.trim();
    if (linha.isEmpty()) return;

    int p1 = linha.indexOf(';');
    int p2 = (p1 >= 0) ? linha.indexOf(';', p1 + 1) : -1;
    int p3 = (p2 >= 0) ? linha.indexOf(';', p2 + 1) : -1;
    int p4 = (p3 >= 0) ? linha.indexOf(';', p3 + 1) : -1;
    int p5 = (p4 >= 0) ? linha.indexOf(';', p4 + 1) : -1;
    int p6 = (p5 >= 0) ? linha.indexOf(';', p5 + 1) : -1;

    if (p1 < 0 || p2 < 0 || p3 < 0 || p4 < 0 || p5 < 0 || p6 < 0) return;

    estado.dataRef = linha.substring(0, p1);
    estado.dataUltimoReset = linha.substring(p1 + 1, p2);
    estado.horasStatusEnviadas = (uint32_t) linha.substring(p2 + 1, p3).toInt();
    estado.avisoAbaixo50SemVazao = (linha.substring(p3 + 1, p4) == "1");
    estado.avisoAbaixo50Enchendo = (linha.substring(p4 + 1, p5) == "1");
    estado.emergenciaAbaixo40SemVazao = (linha.substring(p5 + 1, p6) == "1");
    estado.emergenciaAbaixo20SemVazao = (linha.substring(p6 + 1) == "1");
  }

  void salvarEstado() {
    File f = SPIFFS.open(ARQ_ESTADO, FILE_WRITE);
    if (!f) return;

    String linha;
    linha += estado.dataRef + ";";
    linha += estado.dataUltimoReset + ";";
    linha += String(estado.horasStatusEnviadas) + ";";
    linha += estado.avisoAbaixo50SemVazao ? "1;" : "0;";
    linha += estado.avisoAbaixo50Enchendo ? "1;" : "0;";
    linha += estado.emergenciaAbaixo40SemVazao ? "1;" : "0;";
    linha += estado.emergenciaAbaixo20SemVazao ? "1" : "0";
    linha += "\n";

    f.print(linha);
    f.close();
  }
};

static DisparosManager gDisparos;

void inicializarDisparos() {
  gDisparos.begin();
  Serial.println("Disparos inicializados");
}

void loopDisparos() {
  gDisparos.tick();
}
