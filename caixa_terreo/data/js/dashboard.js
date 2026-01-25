/* ================= DASHBOARD.JS ================= */

let timerStatus = null;

/* ================= INIT ================= */
document.addEventListener('DOMContentLoaded', () => {
  console.log('🚀 Dashboard iniciado (HTTP)');
  atualizar();
  timerStatus = setInterval(atualizar, 3000);
});

/* ================= FETCH STATUS ================= */
async function atualizar() {
  try {
    const resp = await fetch('/api/status');
    if (!resp.ok) return;

    const data = await resp.json();
    atualizarDashboard(data);

  } catch (e) {
    console.warn('❌ Erro status', e);
  }
}

/* ================= DASHBOARD ================= */
function atualizarDashboard(data) {
  if (!data.caixa) return;

  const c = data.caixa;

  // Usuário / Hora
  setText('usuarioAtivo', data.usuario || '---');
  setText('horaSistema', data.datahora || '--:--');

  // Nível
  setText('nivelPercentual', `${c.nivelPercentual.toFixed(1)}%`);
  setText('nivelAltura', `${c.nivelCm.toFixed(1)} cm`);

  // Bombas
  setText(
    'statusBomba',
    c.bombaA || c.bombaB ? 'LIGADA' : 'DESLIGADA'
  );

  // Vazão
  setText(
    'statusVazao',
    c.vazaoEntrada ? 'COM VAZÃO' : 'SEM VAZÃO'
  );

  verificarAlerta(c);
}

/* ================= ALERTA ================= */
function verificarAlerta(caixa) {
  const faixa = document.getElementById('alertaVazao');
  if (!faixa) return;

  if (caixa.nivelPercentual <= 50 && caixa.vazaoEntrada === false) {
    faixa.classList.remove('oculto');
  } else {
    faixa.classList.add('oculto');
  }
}

/* ================= HELPERS ================= */
function setText(id, txt) {
  const el = document.getElementById(id);
  if (el) el.innerText = txt;
}
/* ================= END DASHBOARD.JS ================= */