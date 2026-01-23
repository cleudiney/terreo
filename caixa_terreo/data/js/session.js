// =====================================================
// CONTROLE DE SESSÃO FRONTEND
// =====================================================

const SESSION_TIMEOUT_MS = 10 * 60 * 1000;

function verificarSessao() {

  const logado    = localStorage.getItem("logado");
  const loginTime = localStorage.getItem("loginTime");

  if (!logado || !loginTime) {
    window.location.href = "/login.html";
    return;
  }

  const agora = Date.now();

  if ((agora - loginTime) > SESSION_TIMEOUT_MS) {

    fetch("/api/logout", { method: "POST" })
      .finally(() => {
        localStorage.clear();
        window.location.href = "/login.html";
      });
  }
}

// executa a cada 15 segundos
setInterval(verificarSessao, 15000);
verificarSessao();
