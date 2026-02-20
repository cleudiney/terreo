// admin.js

// ===================================================
// SEGURANÇA
// ===================================================
const nivel = localStorage.getItem("authNivel");

if (nivel !== "admin" && nivel !== "sindico" && nivel !== "subsindico") {
  document.body.innerHTML =
    "<h4>⛔ Acesso negado</h4><p>Você não tem permissão.</p>";
  throw new Error("Acesso negado");
}

// ===================================================
// UTIL
// ===================================================
function mostrarErro(msg) {
  const el = document.getElementById("erro");
  el.textContent = msg;
  el.classList.remove("d-none");
}

function mostrarSucesso(msg) {
  const el = document.getElementById("sucesso");
  el.textContent = msg;
  el.classList.remove("d-none");
  setTimeout(() => el.classList.add("d-none"), 2500);
}

function limparMensagens() {
  document.getElementById("erro").classList.add("d-none");
  document.getElementById("sucesso").classList.add("d-none");
}

// ===================================================
// LISTAR USUÁRIOS
// ===================================================
async function carregarUsuarios() {
  limparMensagens();

  const resp = await fetch("/api/usuarios");
  if (!resp.ok) {
    mostrarErro("Erro ao carregar usuários");
    return;
  }

  const data = await resp.json();
  const usuarios = data.usuarios || [];

  const tbody = document.getElementById("listaUsuarios");
  tbody.innerHTML = "";

  usuarios.forEach(u => {
    const tr = document.createElement("tr");

    tr.innerHTML = `
      <td>${u.usuario}</td>
      <td>${u.nivel}</td>
      <td>
        <span class="badge bg-success">Ativo</span>
      </td>
      <td class="text-center">
        <button class="btn btn-sm btn-warning me-1"
          onclick="editarUsuario('${u.usuario}', '${u.nivel}')">
          ✏️
        </button>
        <button class="btn btn-sm btn-danger"
          onclick="excluirUsuario('${u.usuario}')">
          ❌
        </button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

// ===================================================
// SALVAR (CREATE / UPDATE)
// ===================================================
async function salvarUsuario() {
  limparMensagens();

  const usuario = document.getElementById("usuario").value.trim();
  const senha   = document.getElementById("senha").value.trim();
  const nivel   = document.getElementById("nivel").value;

  if (!usuario || !nivel) {
    mostrarErro("Usuário e nível são obrigatórios");
    return;
  }

  // 🔍 verifica existência real
  const respLista = await fetch("/api/usuarios");
  const dataLista = await respLista.json();
  const usuarios  = dataLista.usuarios || [];

  const existe = usuarios.some(u => u.usuario === usuario);

  // ===================== CREATE =====================
  if (!existe) {
    if (!senha) {
      mostrarErro("Senha é obrigatória para novo usuário");
      return;
    }

    await enviarUsuario("POST", { usuario, senha, nivel });
    return;
  }

  // ===================== UPDATE =====================
  const body = { usuario, nivel };
  if (senha) body.senha = senha; // só envia se alterou

  await enviarUsuario("PUT", body);
}

// ===================================================
// ENVIO + CONFIRMAÇÃO PÓS-GRAVAÇÃO
// ===================================================
async function enviarUsuario(metodo, body) {
  const resp = await fetch("/api/usuarios", {
    method: metodo,
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body)
  });

  // erro real do backend
  if (!resp.ok) {
    const txt = await resp.text();
    mostrarErro(txt || "Erro ao salvar usuário");
    return;
  }

  // 🔎 confirmação pós-gravação
  const check = await fetch("/api/usuarios");
  const data  = await check.json();
  const lista = data.usuarios || [];

  const confirmado = lista.some(u =>
    u.usuario === body.usuario &&
    u.nivel   === body.nivel
  );

  if (!confirmado) {
    mostrarErro("Falha na confirmação da gravação");
    return;
  }

  mostrarSucesso("Usuário salvo com sucesso");
  limparFormulario();
  carregarUsuarios();
}

// ===================================================
// EDITAR
// ===================================================
function editarUsuario(usuario, nivel) {
  document.getElementById("usuario").value = usuario;
  document.getElementById("nivel").value   = nivel;
  document.getElementById("senha").value   = "";
}

// ===================================================
// EXCLUIR
// ===================================================
async function excluirUsuario(usuario) {
  if (!confirm("Desativar usuário " + usuario + "?")) return;

  const resp = await fetch("/api/usuarios", {
    method: "DELETE",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ usuario })
  });

  if (!resp.ok) {
    const txt = await resp.text();
    mostrarErro(txt || "Erro ao excluir usuário");
    return;
  }

  mostrarSucesso("Usuário desativado");
  carregarUsuarios();
}

// ===================================================
// LIMPAR FORM
// ===================================================
function limparFormulario() {
  document.getElementById("usuario").value = "";
  document.getElementById("senha").value   = "";
  document.getElementById("nivel").value   = "";
}

// INIT
carregarUsuarios();
