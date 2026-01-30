// ===================================================
//login-localstorage.js
// =====================================================
// LOGIN — COMPATÍVEL COM ESP32 WebServer
//login-localstorage.js
// =====================================================
document.addEventListener("DOMContentLoaded", () => {

  const form = document.getElementById("loginForm");
  if (!form) return;

  form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const usuario = document.getElementById("username").value.trim();
    const senha   = document.getElementById("password").value.trim();

    limparMensagens();
    mostrarStatus("⏳ Autenticando...");

    try {

      const resp = await fetch("/api/login", {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded"
        },
        body:
          "usuario=" + encodeURIComponent(usuario) +
          "&senha="   + encodeURIComponent(senha)
      });

      if (!resp.ok) {
        throw new Error("Usuário ou senha inválidos");
      }

      await resp.text();

      // ✅ PADRÃO DO SISTEMA
      localStorage.setItem("sessaoAtiva", "true");
      localStorage.setItem("usuarioLogado", usuario);
      localStorage.setItem("loginTime", Date.now());

      mostrarSucesso("✅ Login realizado com sucesso");

      setTimeout(() => {
        window.location.replace("/index.html");
      }, 800);

    } catch (err) {
      mostrarErro(err.message);
      mostrarStatus("Falha no login");
    }
  });
});

// ===================== UI =====================
function mostrarErro(msg) {
  const el = document.getElementById("errorMsg");
  el.textContent = msg;
  el.classList.remove("d-none");
}

function mostrarSucesso(msg) {
  const el = document.getElementById("successMsg");
  el.textContent = msg;
  el.classList.remove("d-none");
}

function mostrarStatus(msg) {
  const el = document.getElementById("debugStatus");
  if (el) el.textContent = msg;
}

function limparMensagens() {
  document.getElementById("errorMsg")?.classList.add("d-none");
  document.getElementById("successMsg")?.classList.add("d-none");
}
