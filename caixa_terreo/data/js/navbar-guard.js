/* navbar-guard.js */

function filtrarNavbarPorNivel() {
  const nivel = localStorage.getItem('authNivel');

  if (!nivel) {
    console.warn('Navbar: usuário não autenticado');
    ocultarTudo();
    return;
  }

  document
    .querySelectorAll('[data-nivel]')
    .forEach(el => {
      const niveisPermitidos = el
        .getAttribute('data-nivel')
        .split(',')
        .map(n => n.trim());

      if (!niveisPermitidos.includes(nivel)) {
        el.style.display = 'none';
      }
    });

  console.log('Navbar filtrada para nível:', nivel);
}

function ocultarTudo() {
  document
    .querySelectorAll('[data-nivel]')
    .forEach(el => el.style.display = 'none');
}
