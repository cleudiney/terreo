//spiffs.ino
#include "variaveis.h"
#include <SPIFFS.h>

// ===================== SPIFFS =====================
void inicializarSPIFFS() {
  Serial.print("📂 Inicializando SPIFFS... ");

  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Falha ao montar SPIFFS!");
    return;
  }

  Serial.println("✅ OK");

  // Criar diretório /dados se não existir
  criarDiretorioSeNaoExistir(DIR_DADOS);

  // Listar arquivos para debug
  listarSPIFFSDebug();
  
  // Mostrar espaço disponível
  mostrarEspacoSPIFFS();
}

bool criarDiretorioSeNaoExistir(String caminho) {
  if (!SPIFFS.exists(caminho)) {
    if (SPIFFS.mkdir(caminho)) {
      Serial.printf("📁 Diretório criado: %s\n", caminho.c_str());
      return true;
    } else {
      Serial.printf("❌ Falha ao criar diretório: %s\n", caminho.c_str());
      return false;
    }
  }
  return true;
}

// ===================== LISTAGEM DEBUG =====================
void listarSPIFFSDebug() {
  Serial.println("📋 Conteúdo do SPIFFS:");

  File root = SPIFFS.open("/");
  if (!root) {
    Serial.println("❌ Erro ao abrir diretório raiz");
    return;
  }

  File file = root.openNextFile();
  int count = 0;

  while (file) {
    count++;
    Serial.printf("  %-30s %8d bytes\n",
                  file.name(),
                  file.size());
    file = root.openNextFile();
  }

  if (count == 0) {
    Serial.println("  (vazio)");
  }

  root.close();
}

// ===================== VERIFICAÇÃO DE ARQUIVOS ESSENCIAIS =====================
bool verificarArquivosEssenciais() {
  // CORRIGIDO: Removido ARQ_HISTORICO que não existe nas definições
  const char* arquivos[] = {
    ARQ_USUARIOS,
    ARQ_AVISOS,
    ARQ_REGISTROS
    // ARQ_HISTORICO foi removido - não definido em variaveis.h
  };

  bool todosExistem = true;

  for (const char* arq : arquivos) {
    if (!SPIFFS.exists(arq)) {
      Serial.printf("⚠️ Arquivo não encontrado: %s\n", arq);

      if (String(arq) == ARQ_USUARIOS) {
        Serial.println("📄 Criando usuarios.dat com dados padrão...");
        criarUsuariosPadrao();
      } else {
        File f = SPIFFS.open(arq, FILE_WRITE);
        if (f) {
          f.close();
          Serial.printf("📄 Criado arquivo vazio: %s\n", arq);
        } else {
          Serial.printf("❌ Falha ao criar arquivo: %s\n", arq);
        }
      }
      todosExistem = false;
    }
  }

  if (todosExistem) {
    Serial.println("✅ Todos os arquivos essenciais encontrados");
  }

  return todosExistem;
}

void criarUsuariosPadrao() {
  File file = SPIFFS.open(ARQ_USUARIOS, FILE_WRITE);
  if (!file) {
    Serial.println("❌ Falha ao criar usuarios.dat");
    return;
  }

  // Usuários padrão com senhas (altere para seu uso!)
  file.println("adm;aquarios2025!;admin");
  file.println("sindico;aquarios23!;sindico");
  file.println("zelador;aquarios9999#;zelador");
  
  file.close();
  Serial.println("✅ usuarios.dat criado com usuários padrão");
}

// ===================== LEITURA / ESCRITA =====================
String lerArquivo(String caminho) {
  if (!SPIFFS.exists(caminho)) {
    return "";
  }

  File file = SPIFFS.open(caminho, FILE_READ);
  if (!file) {
    return "";
  }

  // Limitar tamanho da leitura para evitar problemas de memória
  const size_t MAX_SIZE = 8192; // 8KB máximo
  String conteudo;
  
  while (file.available() && conteudo.length() < MAX_SIZE) {
    conteudo += (char)file.read();
  }
  
  file.close();
  return conteudo;
}

bool escreverArquivo(String caminho, String conteudo, bool append) {
  File file = SPIFFS.open(caminho, append ? FILE_APPEND : FILE_WRITE);
  if (!file) {
    return false;
  }

  size_t bytes = file.print(conteudo);
  file.close();

  return (bytes > 0);
}

// ===================== ESPAÇO DISPONÍVEL =====================
void mostrarEspacoSPIFFS() {
  size_t total = SPIFFS.totalBytes();
  size_t usado = SPIFFS.usedBytes();
  size_t livre = total - usado;

  Serial.printf("💾 SPIFFS: %d KB total | %d KB usado | %d KB livre (%.1f%% usado)\n",
                total / 1024,
                usado / 1024,
                livre / 1024,
                (usado * 100.0) / total);
}

// ===================== LIMPEZA DE ARQUIVOS ANTIGOS =====================
void limparArquivosAntigos(String padraoNome, int manterMaximo) {
  // Esta função limpa arquivos antigos com base em um padrão de nome
  // Exemplo: limparArquivosAntigos("historico_", 10) mantém apenas os 10 mais recentes
  
  Serial.printf("🗑️  Limpando arquivos antigos com padrão: %s\n", padraoNome.c_str());
  
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  std::vector<String> arquivosParaLimpar;
  
  while (file) {
    String nomeArquivo = String(file.name());
    if (nomeArquivo.indexOf(padraoNome) != -1) {
      arquivosParaLimpar.push_back(nomeArquivo);
    }
    file = root.openNextFile();
  }
  root.close();
  
  if (arquivosParaLimpar.size() > manterMaximo) {
    // Ordenar por data se necessário (implementação específica)
    // Por enquanto, apenas remove os mais antigos
    int excedente = arquivosParaLimpar.size() - manterMaximo;
    for (int i = 0; i < excedente; i++) {
      Serial.printf("  Removendo: %s\n", arquivosParaLimpar[i].c_str());
      SPIFFS.remove(arquivosParaLimpar[i]);
    }
  }
}

// ===================== BACKUP DE DADOS =====================
bool criarBackupDados() {
  String nomeBackup = "/backup_" + getDataAtual() + ".dat";
  
  File backup = SPIFFS.open(nomeBackup, FILE_WRITE);
  if (!backup) {
    Serial.println("❌ Falha ao criar backup");
    return false;
  }
  
  // Copiar dados importantes para o backup
  String dados = "";
  
  // 1. Avisos
  if (SPIFFS.exists(ARQ_AVISOS)) {
    dados += "=== AVISOS ===\n";
    dados += lerArquivo(ARQ_AVISOS);
    dados += "\n\n";
  }
  
  // 2. Registros
  if (SPIFFS.exists(ARQ_REGISTROS)) {
    dados += "=== REGISTROS ===\n";
    dados += lerArquivo(ARQ_REGISTROS);
  }
  
  backup.print(dados);
  backup.close();
  
  Serial.printf("✅ Backup criado: %s (%d bytes)\n", nomeBackup.c_str(), dados.length());
  return true;
}

// ===================== VERIFICAÇÃO DE INTEGRIDADE =====================
void verificarIntegridadeSPIFFS() {
  Serial.println("🔍 Verificando integridade do SPIFFS...");
  
  bool problemas = false;
  
  // Verificar se podemos escrever e ler
  String testeArquivo = "/teste_integridade.tmp";
  String conteudoTeste = "Teste de integridade " + String(millis());
  
  if (!escreverArquivo(testeArquivo, conteudoTeste, false)) {
    Serial.println("❌ Falha ao escrever arquivo de teste");
    problemas = true;
  }
  
  String lido = lerArquivo(testeArquivo);
  if (lido != conteudoTeste) {
    Serial.println("❌ Falha na leitura/escrita - dados corrompidos");
    problemas = true;
  }
  
  // Remover arquivo de teste
  SPIFFS.remove(testeArquivo);
  
  if (!problemas) {
    Serial.println("✅ Integridade do SPIFFS OK");
  } else {
    Serial.println("⚠️  Possíveis problemas no SPIFFS");
  }
}

// ===================== UTILITÁRIO =====================
bool arquivoExiste(String caminho) {
  return SPIFFS.exists(caminho);
}

size_t tamanhoArquivo(String caminho) {
  if (!SPIFFS.exists(caminho)) {
    return 0;
  }
  
  File file = SPIFFS.open(caminho, FILE_READ);
  if (!file) {
    return 0;
  }
  
  size_t tamanho = file.size();
  file.close();
  return tamanho;
}

// ===================== LIMPAR DADOS ANTIGOS =====================
void limparDadosAntigos(int diasManter) {
  // Esta função seria implementada se houvesse datas nos arquivos
  // Por enquanto, apenas um placeholder
  Serial.println("🔄 Função limparDadosAntigos() - em desenvolvimento");
}

// ===================== ESTATÍSTICAS DE ARQUIVOS =====================
void mostrarEstatisticasArquivos() {
  Serial.println("📊 Estatísticas de arquivos:");
  
  const char* arquivos[] = {ARQ_USUARIOS, ARQ_AVISOS, ARQ_REGISTROS};
  const char* nomes[] = {"Usuários", "Avisos", "Registros"};
  
  for (int i = 0; i < 3; i++) {
    size_t tamanho = tamanhoArquivo(arquivos[i]);
    Serial.printf("  %-15s: %6d bytes", nomes[i], tamanho);
    
    if (tamanho > 0) {
      File f = SPIFFS.open(arquivos[i], FILE_READ);
      if (f) {
        int linhas = 0;
        while (f.available()) {
          if (f.read() == '\n') linhas++;
        }
        f.close();
        Serial.printf(" (%d linhas)", linhas);
      }
    }
    Serial.println();
  }
}
