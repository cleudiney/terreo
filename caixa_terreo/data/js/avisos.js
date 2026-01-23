// avisos.js

function carregarAvisos() {

  fetch("/api/avisos")
    .then(r => {
      if (!r.ok) throw new Error("Erro ao carregar avisos");
      return r.json();
    })
    .then(dados => {
      const tbody = document.getElementById("listaAvisos");
      tbody.innerHTML = "";

      if (dados.length === 0) {
        tbody.innerHTML =
          "<tr><td class='text-muted'>Nenhum aviso</td></tr>";
        return;
      }

      dados.reverse().forEach(a => {
        const tr = document.createElement("tr");
        tr.innerHTML = `<td><pre>${a.raw}</pre></td>`;
        tbody.appendChild(tr);
      });
    })
    .catch(err => {
      alert(err.message);
    });
}

// carrega ao abrir
document.addEventListener("DOMContentLoaded", carregarAvisos);
