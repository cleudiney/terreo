// navbar.js
document.addEventListener("DOMContentLoaded", async () => {

  // carrega HTML
  const resp = await fetch("/partials/navbar.html");
  const html = await resp.text();
  document.getElementById("navbar").innerHTML = html;

  const user  = localStorage.getItem("authUser");
  const nivel = localStorage.getItem("authNivel");

  // usuário
  const userEl = document.getElementById("navbarUser");
  if (userEl && user) {
    userEl.textContent = `👤 ${user}`;
  }

  // badge de nível
  const badge = document.getElementById("nivelBadge");
  if (badge && nivel) {
    badge.textContent = nivel.toUpperCase();

    badge.classList.remove("admin", "sindico", "zelador");

    if (nivel === "admin")   badge.classList.add("admin");
    if (nivel === "sindico") badge.classList.add("sindico");
    if (nivel === "zelador") badge.classList.add("zelador");
  }

  // controle de menus por nível
  document.querySelectorAll("[data-level]").forEach(el => {

    const permitido = el.dataset.level
      .split(",")
      .map(v => v.trim());

    if (
      permitido.includes("all") ||
      permitido.includes(nivel)
    ) {
      el.style.display = "block";
    } else {
      el.style.display = "none";
    }
  });

  // toggle mobile
  const toggle = document.getElementById("navbarToggle");
  const menu   = document.getElementById("navbarMenu");

  if (toggle && menu) {
    toggle.addEventListener("click", () => {
      menu.classList.toggle("open");
    });
  }
});
function logout() {

  fetch("/api/logout", { method: "POST" })
    .finally(() => {

      localStorage.clear();

      window.location.href = "/login.html";
    });
}

