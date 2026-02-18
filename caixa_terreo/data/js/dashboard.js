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

    const payload = await resp.json();
    const data = payload?.data || payload;
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
  setText('userInfo', `👤 ${data.usuario || '---'}`);
  setText('horaSistema', data.datahora || '--:--');
  setText('currentTime', `⏰ ${data.datahora || '--:--'}`);

  // Nível
  setText('nivelPercentual', `${c.nivelPercentual.toFixed(1)}%`);
  setText('waterPercentage', `${c.nivelPercentual.toFixed(1)}%`);
  setText('nivelAltura', `${c.nivelCm.toFixed(1)} cm`);
  setText('nivelValue', `${c.nivelCm.toFixed(1)} cm`);
  setText('volumeValue', `${calcularVolumeEstimado(c.nivelPercentual)} L`);

  atualizarTanque(c.nivelPercentual);

  // Bombas
  const statusBomba = c.bombaA || c.bombaB ? 'LIGADA' : 'DESLIGADA';
  setText(
    'statusBomba',
    statusBomba
  );
  setText('bombaStatus', statusBomba);

  // Vazão
  setText(
    'statusVazao',
    c.vazaoEntrada ? 'COM VAZÃO' : 'SEM VAZÃO'
  );

  verificarAlerta(c);
}

function calcularVolumeEstimado(nivelPercentual) {
  const VOLUME_TOTAL_L = 20000;
  const volume = (nivelPercentual / 100) * VOLUME_TOTAL_L;
  return Math.round(volume);
}

function atualizarTanque(nivelPercentual) {
  const agua = document.getElementById('agua');
  if (!agua) return;

  agua.style.height = `${Math.max(0, Math.min(100, nivelPercentual))}%`;
  agua.classList.remove('baixo', 'medio', 'alto');

  if (nivelPercentual < 30) {
    agua.classList.add('baixo');
  } else if (nivelPercentual < 70) {
    agua.classList.add('medio');
  } else {
    agua.classList.add('alto');
  }
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
