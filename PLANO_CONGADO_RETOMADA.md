# Projeto Congado — Plano de Retomada (do bezerro ao leite)

## 1) Objetivo de negócio
Conectar os dados e as rotinas do ciclo produtivo (nascimento/aleitamento, recria, lactação e ordenha) em um fluxo único, com foco em:
- aumento de produtividade por animal;
- redução de perdas (sanidade, manejo, descarte de leite);
- rastreabilidade operacional para decisão diária.

---

## 2) Escopo inicial (MVP)
### 2.1 Módulos do MVP
1. **Cadastro base**
   - Animais
   - Lotes
   - Pessoas/responsáveis
2. **Bezerreiro**
   - Nascimento e colostragem
   - Aleitamento diário
   - Pesagens e ganho de peso
   - Alertas básicos de manejo
3. **Transição para produção**
   - Eventos reprodutivos essenciais
   - Mudança de status do animal (bezerro → novilha → vaca em lactação)
4. **Leite/ordenha**
   - Registro diário de produção
   - Indicadores simples por animal/lote
5. **Painel operacional**
   - Alertas do dia
   - Resumo de produção
   - Tarefas pendentes

### 2.2 Fora do MVP (fase posterior)
- Integração avançada com sensores externos;
- Modelos preditivos (IA) para produção/doenças;
- App offline robusto e sincronização assíncrona completa.

---

## 3) Entregas por fase
### Fase 0 — Diagnóstico técnico (1 semana)
- Inventariar funcionalidades já implementadas no repositório;
- Identificar lacunas entre o estado atual e o fluxo “bezerro ao leite”;
- Definir baseline de dados e critérios de aceite da retomada.

### Fase 1 — Estrutura de dados e cadastro (1–2 semanas)
- Consolidar entidades principais: `Animal`, `Lote`, `EventoManejo`, `OrdemOrdenha`;
- Definir relacionamentos mínimos para rastreabilidade;
- Validar fluxo de criação/edição/consulta dos cadastros.

### Fase 2 — Jornada do bezerro (2 semanas)
- Implementar eventos: nascimento, colostro, dieta, pesagem, intercorrência;
- Dashboard com indicadores de bezerreiro (ganho médio diário, taxa de alerta);
- Regras de alerta para eventos fora do padrão.

### Fase 3 — Jornada até leite (2 semanas)
- Implementar transição para lactação e produção diária;
- Consolidar visualização por animal e por lote;
- Indicadores operacionais: produção média, top/bottom desempenho.

### Fase 4 — Estabilização e operação (1 semana)
- Ajustes de UX e performance;
- Hardening de autenticação/autorização;
- Checklist de operação e suporte.

---

## 4) Backlog priorizado (primeiras 10 tarefas)
1. Mapear estado atual do código e endpoints disponíveis.
2. Definir dicionário de dados do ciclo produtivo.
3. Criar/ajustar estrutura de cadastro de animais com status produtivo.
4. Registrar eventos de nascimento e colostragem.
5. Registrar rotina de aleitamento e pesagens.
6. Implementar consulta de histórico por animal.
7. Implementar registro de produção de leite por dia.
8. Criar painel resumido com alertas e produção diária.
9. Definir perfis de acesso por função operacional.
10. Criar roteiro de validação com usuário de campo.

---

## 5) Indicadores de sucesso
- **Cobertura de dados**: % de animais com jornada registrada sem lacunas críticas;
- **Adoção operacional**: nº de registros/dia por equipe;
- **Tempo de decisão**: redução de tempo para identificar animais com alerta;
- **Produção**: evolução de média diária de leite por lote.

---

## 6) Riscos e mitigação
- **Risco**: dados incompletos ou inconsistentes na base atual.  
  **Mitigação**: validações mínimas obrigatórias por evento e auditoria de registros.
- **Risco**: baixa adesão no campo.  
  **Mitigação**: telas simples, fluxo rápido e treinamento curto por papel.
- **Risco**: acoplamento alto no código legado.  
  **Mitigação**: isolar módulos por domínio e priorizar interfaces estáveis.

---

## 7) Próximos passos imediatos (semana atual)
1. Levantamento do estado atual do repositório e APIs.
2. Alinhamento de escopo MVP com responsáveis do projeto.
3. Quebra da Fase 1 em issues técnicas pequenas (1–2 dias cada).
4. Início de implementação pelo fluxo de cadastro + eventos do bezerreiro.

