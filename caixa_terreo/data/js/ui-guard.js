// ui-guard.js
// Controle de elementos da interface por nível de acesso

document.addEventListener('DOMContentLoaded', () => {
  aplicarPermissoesUI();
});

function aplicarPermissoesUI() {
  const nivel = localStorage.getItem('authNivel');

  if (!nivel) {
    console.warn('UI Guard: nível não encontrado');
    return;
  }

  // Elementos controlados por nível
  const elementos = document.querySelectorAll('[data-level]');

  elementos.forEach(el => {
    const permitido = el.getAttribute('data-level')
      .split(',')
      .map(n => n.trim());

    if (!permitido.includes(nivel)) {
      bloquearElemento(el);
    }
  });
}

function bloquearElemento(el) {
  // Botões e inputs
  if (
    el.tagName === 'BUTTON' ||
    el.tagName === 'INPUT' ||
    el.tagName === 'SELECT'
  ) {
    el.disabled = true;
  }

  // Visual
  el.style.opacity = '0.5';
  el.style.cursor = 'not-allowed';

  // Tooltip
  el.title = 'Acesso restrito para seu nível';

  // Segurança extra: bloquear click
  el.addEventListener('click', e => {
    e.preventDefault();
    e.stopPropagation();
    alert('⛔ Você não tem permissão para esta ação');
  });
}
