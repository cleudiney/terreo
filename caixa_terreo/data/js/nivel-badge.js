/* nivel-badge.js */

document.addEventListener('DOMContentLoaded', () => {
  aplicarBadgeNivel();
});

function aplicarBadgeNivel() {
  const nivel = localStorage.getItem('authNivel');
  const badge = document.getElementById('nivelBadge');

  if (!badge || !nivel) return;

  badge.className = 'badge ms-2'; // reset seguro

  switch (nivel) {
    case 'admin':
      badge.classList.add('bg-danger');
      badge.innerText = 'ADMIN';
      break;

    case 'operador':
      badge.classList.add('bg-primary');
      badge.innerText = 'OPERADOR';
      break;

    case 'visitante':
      badge.classList.add('bg-secondary');
      badge.innerText = 'VISITANTE';
      break;

    default:
      badge.classList.add('bg-dark');
      badge.innerText = 'DESCONHECIDO';
  }
}
