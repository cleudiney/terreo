/*controle.js  */
/* =====================================================
   CONTROLE.JS — CONTROLE MANUAL DE BOMBAS
   ===================================================== */

let statusAtual = null;

/* ================= INIT ================= */
document.addEventListener('DOMContentLoaded', () => {
  carregarStatus();
  setInterval(carregarStatus, 2000);
});

/* ================= STATUS ================= */
function carregarStatus() {

  fetch('/api/status')
    .then(r => r.json())
    .then(resp => {

      if (!resp.success) return;

      statusAtual = resp.data;

      atualizarPainel(statusAtual);
    })
    .catch(err => {
      document.getElementById('statusSistema').innerHTML =
        `<div class="alert alert-danger">
          Erro de comunicação com o sistema
        </div>`;
    });
}

/* ================= RENDER ================= */
function atualizarPainel(data) {

  const c = data.caixa;

  // STATUS GERAL
  document.getElementById('statusSistema').innerHTML = `
    <div class="alert alert-info text-center">
      👤 ${data.usuario} |
      🕒 ${data.datahora} |
      💧 Nível: <strong>${c.nivelPercentual.toFixed(1)}%</strong>
      (${c.nivelCm.toFixed(1)} cm) |
      🚰 ${c.vazaoEntrada ? 'Com vazão' : 'Sem vazão'}
    </div>
  `;

  // BOMBAS
  let html = '';

  html += cardBomba('A', c.bombaA);
  html += cardBomba('B', c.bombaB);

  document.getElementById('painelBombas').innerHTML = html;
}

/* ================= CARD ================= */
function cardBomba(nome, ligada) {

  const estado = ligada ? 'LIGADA' : 'DESLIGADA';
  const cor    = ligada ? 'success' : 'secondary';

  const acao = ligada
    ? `desligar('${nome}')`
    : `ligar('${nome}')`;

  const texto = ligada ? 'Desligar' : 'Ligar';

  return `
    <div class="col-md-6 mb-3">
      <div class="card shadow-sm">
        <div class="card-header">
          🔌 Bomba ${nome}
        </div>
        <div class="card-body text-center">
          <h3 class="text-${cor}">
            ${estado}
          </h3>
          <button class="btn btn-${cor}"
                  onclick="${acao}">
            ${texto}
          </button>
        </div>
      </div>
    </div>
  `;
}

/* ================= COMANDOS ================= */

function ligar(bomba) {
  enviar(`LIGAR_${bomba}`);
}

function desligar(bomba) {
  enviar(`DESLIGAR`);
}

function acao(tipo) {

  if (tipo === 'ambas_on') enviar('LIGAR_AB');
  if (tipo === 'ambas_off') enviar('DESLIGAR');
  if (tipo === 'auto')
    alert('Modo automático já é gerenciado pelo firmware.');
}

/* ================= API ================= */
function enviar(comando) {

  fetch(`/api/bombas?comando=${comando}`)
    .then(r => r.json())
    .then(resp => {

      if (!resp.success) {
        alert(resp.msg || 'Erro ao executar comando');
        return;
      }

      carregarStatus();
    });
}
