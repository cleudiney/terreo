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
    
    // Se não autenticado (401), redirecionar para login
    if (resp.status === 401) {
      console.warn('⛔ Sessão expirada (401)');
      localStorage.clear();
      window.location.replace("/login.html");
      return;
    }
    
    if (!resp.ok) {
      console.warn('❌ Erro status', resp.status);
      return;
    }

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
  const volumeTotal = Number(c.volumeTotalLitros) || getVolumeTotalLitros();
  setText('volumeValue', `${calcularVolumeEstimado(c.nivelPercentual, volumeTotal)} L`);

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
  atualizarEstimativaEnchimento(c, volumeTotal);
}

function getVolumeTotalLitros() {
  return ((200 - 30) * 355 * 415) / 1000;
}

function calcularVolumeEstimado(nivelPercentual, volumeTotalLitros) {
  const volume = (nivelPercentual / 100) * volumeTotalLitros;
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
    faixa.textContent = '🚨 ATENÇÃO: sem fornecimento de água! Verifique COPASA ou o registro geral.';
    faixa.classList.remove('oculto');
  } else if (caixa.nivelPercentual <= 50 && caixa.vazaoEntrada === true) {
    faixa.textContent = '✅ Bóia de água ativada: caixa abaixo de 50% e enchendo.';
    faixa.classList.remove('oculto');
  } else {
    faixa.classList.add('oculto');
  }
}

function atualizarEstimativaEnchimento(caixa, volumeTotalLitros) {
  const el = document.getElementById('tempoEstimado');
  if (!el) return;

  const vazao = Number(caixa.vazaoCalculadaLMin);
  if (!(caixa.nivelPercentual < 100) || !Number.isFinite(vazao) || vazao <= 0) {
    el.innerText = '';
    return;
  }

  const litrosFaltantes = ((100 - caixa.nivelPercentual) / 100) * volumeTotalLitros;
  const minutos = litrosFaltantes / vazao;
  if (!Number.isFinite(minutos) || minutos <= 0) {
    el.innerText = '';
    return;
  }

  el.innerText = `Estimativa de enchimento: ${formatarDuracao(minutos)} (${vazao.toFixed(1)} L/min)`;
}

function formatarDuracao(minutosTotal) {
  const horas = Math.floor(minutosTotal / 60);
  const minutos = Math.round(minutosTotal % 60);
  if (horas <= 0) return `${minutos} min`;
  return `${horas}h ${minutos}min`;
}

/* ================= HELPERS ================= */
function setText(id, txt) {
  const el = document.getElementById(id);
  if (el) el.innerText = txt;
}
/* ================= END DASHBOARD.JS ================= */
