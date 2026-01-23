/* =====================================================
   DASHBOARD.JS – SPA WEBSOCKET
   ===================================================== */

let ws;

/* ================= INIT ================= */
document.addEventListener('DOMContentLoaded', () => {
  console.log('🚀 Dashboard iniciado');
  iniciarWebSocket();
});

/* ================= WEBSOCKET ================= */
function iniciarWebSocket() {
  ws = new WebSocket(`ws://${location.host}/ws`);

  ws.onopen = () => {
    console.log('🟢 WebSocket conectado');
  };

  ws.onmessage = (evt) => {
    const data = JSON.parse(evt.data);
    atualizarDashboard(data);
  };

  ws.onerror = (e) => {
    console.error('❌ WS erro', e);
  };

  ws.onclose = () => {
    console.warn('🔴 WS desconectado, reconectando...');
    setTimeout(iniciarWebSocket, 3000);
  };
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

  // Bombas (SOMENTE ESTADO)
  setText(
    'statusBomba',
    c.bombaA || c.bombaB ? 'LIGADA' : 'DESLIGADA'
  );

  // Vazão
  setText(
    'statusVazao',
    c.vazaoEntrada ? 'COM VAZÃO' : 'SEM VAZÃO'
  );

  // Alerta visual
  verificarAlerta(c);
}

/* ================= ALERTA ================= */
function verificarAlerta(caixa) {
  const faixa = document.getElementById('alertaVazao');

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
