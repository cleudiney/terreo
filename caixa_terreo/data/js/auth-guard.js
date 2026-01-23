/* auth-guard.js */

function protegerPagina(niveisPermitidos = []) {

  const user  = localStorage.getItem("authUser");
  const nivel = localStorage.getItem("authNivel");
  const exp   = Number(localStorage.getItem("authExpira"));

  // ================= NÃO LOGADO =================
  if (!user || !nivel || !exp) {
    redirecionarLogin();
    return;
  }

  // ================= EXPIRADO =================
  if (Date.now() > exp) {
    console.warn("⌛ Sessão expirada");

    fetch("/api/logout", { method: "POST" }).finally(() => {
      localStorage.clear();
      redirecionarLogin();
    });

    return;
  }

  // ================= NÍVEL =================
  if (
    niveisPermitidos.length > 0 &&
    !niveisPermitidos.includes(nivel)
  ) {
    console.warn("⛔ Nível não autorizado:", nivel);

    redirecionarLogin();
    return;
  }

  // ================= OK =================
  window.usuarioAutenticado = true;
}

// ===================================================
function redirecionarLogin() {
  window.location.replace("/login.html");
}
