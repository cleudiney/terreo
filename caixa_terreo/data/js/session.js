// =====================================================
// CONTROLE DE SESSÃO FRONTEND
// =====================================================
const SESSION_TIMEOUT_MS = 10 * 60 * 1000;

function verificarSessao() {

  const usuario = localStorage.getItem("usuarioLogado");
  const ativa   = localStorage.getItem("sessaoAtiva");
  const loginTime = localStorage.getItem("loginTime");

  if (!usuario || ativa !== "true" || !loginTime) {
    window.location.replace("/login-localstorage.html");
    return;
  }

  const agora = Date.now();

  if ((agora - loginTime) > SESSION_TIMEOUT_MS) {

    fetch("/api/logout", { method: "POST" })
      .finally(() => {
        localStorage.clear();
        window.location.replace("/login-localstorage.html");
      });
  }
}

setInterval(verificarSessao, 15000);
verificarSessao();
