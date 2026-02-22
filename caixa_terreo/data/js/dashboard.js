/* ================= DASHBOARD.JS ================= */

let timerStatus = null;

function formatarDataHoraBrasil(valor) {
  const d = parseDataHoraFlexivel(valor);
  if (!d) return valor || '--:--';
  return new Intl.DateTimeFormat('pt-BR', {
    timeZone: 'America/Sao_Paulo',
    day: '2-digit',
    month: '2-digit',
    year: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit'
  }).format(d);
}

function formatarAgoraBrasil() {
  return new Intl.DateTimeFormat('pt-BR', {
    timeZone: 'America/Sao_Paulo',
    day: '2-digit',
    month: '2-digit',
    year: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit'
  }).format(new Date());
}

function parseDataHoraFlexivel(valor) {
  if (!valor || typeof valor !== 'string') return null;

  // yyyy-mm-dd HH:MM:SS
  const isoLike = valor.match(/^(\d{4})-(\d{2})-(\d{2})[ T](\d{2}):(\d{2})(?::(\d{2}))?$/);
  if (isoLike) {
    const [, y, m, d, hh, mm, ss] = isoLike;
    return new Date(`${y}-${m}-${d}T${hh}:${mm}:${ss || '00'}`);
  }

  // dd/mm/yyyy HH:MM:SS
  const brLike = valor.match(/^(\d{2})\/(\d{2})\/(\d{4})[ T](\d{2}):(\d{2})(?::(\d{2}))?$/);
  if (brLike) {
    const [, d, m, y, hh, mm, ss] = brLike;
    return new Date(`${y}-${m}-${d}T${hh}:${mm}:${ss || '00'}`);
  }

  const parsed = new Date(valor);
  return Number.isNaN(parsed.getTime()) ? null : parsed;
}

/* ================= INIT ================= */
document.addEventListener('DOMContentLoaded', () => {
  console.log('Dashboard iniciado (HTTP)');
  atualizar();
  timerStatus = setInterval(atualizar, 3000);
});

/* ================= FETCH STATUS ================= */
async function atualizar() {
  try {
    const resp = await fetch('/api/status');

    // Se nao autenticado (401), redirecionar para login
    if (resp.status === 401) {
      console.warn('Sessao expirada (401)');
      localStorage.clear();
      window.location.replace('/login.html');
      return;
    }

    if (!resp.ok) {
      console.warn('Erro status', resp.status);
      return;
    }

    const payload = await resp.json();
    const data = payload?.data || payload;
    atualizarDashboard(data);
  } catch (e) {
    console.warn('Erro status', e);
  }
}

/* ================= DASHBOARD ================= */
function atualizarDashboard(data) {
  if (!data || !data.caixa) return;

  const c = data.caixa;

  // Usuario / Hora
  setText('usuarioAtivo', data.usuario || '---');
  setText('userInfo', `Usuario: ${data.usuario || '---'}`);
  setText('horaSistema', data.datahora || '--:--');
  setText('currentTime', `Brasil: ${formatarDataHoraBrasil(data.datahora)}`);

  // Nivel
  const nivelPercentual = Number(c.nivelPercentual) || 0;
  const nivelCm = Number(c.nivelCm) || 0;
  const volumeTotal = Number(c.volumeTotalLitros) || getVolumeTotalLitros();

  setText('nivelPercentual', `${nivelPercentual.toFixed(1)}%`);
  setText('waterPercentage', `${nivelPercentual.toFixed(1)}%`);
  setText('nivelAltura', `${nivelCm.toFixed(1)} cm`);
  setText('nivelValue', `${nivelCm.toFixed(1)} cm`);
  setText('volumeValue', `${calcularVolumeEstimado(nivelPercentual, volumeTotal)} L`);

  atualizarTanque(nivelPercentual);

  // Bombas
  const statusBomba = c.bombaA || c.bombaB ? 'LIGADA' : 'DESLIGADA';
  setText('statusBomba', statusBomba);
  setText('bombaStatus', statusBomba);

  // Emergencia
  const emergencia = normalizarEmergencia(c.emergencia);
  setText('emergenciaStatus', emergencia);

  // Vazao
  setText('statusVazao', c.vazaoEntrada ? 'COM VAZAO' : 'SEM VAZAO');

  verificarAlerta(c);
  atualizarEstimativaEnchimento(c, volumeTotal);
  atualizarRodape(c);
}

function normalizarEmergencia(valor) {
  if (typeof valor === 'string' && valor.trim()) return valor;
  if (valor === true || valor === 1) return 'ATIVA';
  if (valor === false || valor === 0) return 'NAO';
  return '--';
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
    faixa.textContent = 'ATENCAO: sem fornecimento de agua. Verifique COPASA ou o registro geral.';
    faixa.classList.remove('oculto');
  } else if (caixa.nivelPercentual <= 50 && caixa.vazaoEntrada === true) {
    faixa.textContent = 'Vazao detectada: caixa abaixo de 50% e enchendo.';
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

function atualizarRodape(caixa) {
  const el = document.getElementById('lastUpdate');
  if (!el) return;

  el.innerText = `Ultima atualizacao: ${formatarAgoraBrasil()}`;

  const alertEl = document.getElementById('alertStatus');
  if (!alertEl) return;

  if (caixa.nivelPercentual <= 20) {
    alertEl.className = 'alert alert-danger mt-3 mb-2';
    alertEl.innerText = 'Nivel critico';
  } else if (caixa.nivelPercentual <= 50) {
    alertEl.className = 'alert alert-warning mt-3 mb-2';
    alertEl.innerText = 'Nivel baixo';
  } else {
    alertEl.className = 'alert alert-success mt-3 mb-2';
    alertEl.innerText = 'Sistema operando normalmente';
  }
}

/* ================= HELPERS ================= */
function setText(id, txt) {
  const el = document.getElementById(id);
  if (el) el.innerText = txt;
}

/* ================= END DASHBOARD.JS ================= */
