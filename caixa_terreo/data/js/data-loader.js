// /js/data-loader.js
async function carregarEstatisticas() {
  const res = await fetch('/registros.dat');
  const txt = await res.text();
  const linhas = txt.trim().split('\n');

  const hoje = [];
  const semana = [];
  const mes = [];

  const agora = new Date();
  const hojeStr = agora.toISOString().slice(0,10);

  const inicioSemana = new Date(agora);
  inicioSemana.setDate(agora.getDate() - ((agora.getDay() + 6) % 7));

  linhas.forEach(l => {
    const [data,hora,pct,nivel,estado,faixa] = l.split(';');
    const d = new Date(`${data}T${hora}`);

    const registro = {
      data, hora,
      percentual: parseFloat(pct),
      nivel: parseFloat(nivel),
      estado, faixa
    };

    if (data === hojeStr) hoje.push(registro);
    if (d >= inicioSemana) semana.push(registro);
    if (d.getMonth() === agora.getMonth()) mes.push(registro);
  });

  renderResumo('stat-hoje', hoje);
  renderResumo('stat-semana', semana);
  renderResumo('stat-mes', mes);

  renderFaixas(hoje);
}

function renderResumo(id, dados) {
  if (!dados.length) {
    document.getElementById(id).innerHTML = '<small>Sem dados</small>';
    return;
  }

  const valores = dados.map(d => d.percentual);
  const min = Math.min(...valores);
  const max = Math.max(...valores);

  document.getElementById(id).innerHTML = `
    <div>Mín: <b>${min.toFixed(1)}%</b></div>
    <div>Máx: <b>${max.toFixed(1)}%</b></div>
    <div>Total pontos: <b>${dados.length}</b></div>
  `;
}

function renderFaixas(dados) {
  const tbody = document.getElementById('faixas-horarias');
  const mapa = {};

  dados.forEach(d => {
    if (!mapa[d.faixa]) mapa[d.faixa] = [];
    mapa[d.faixa].push(d);
  });

  tbody.innerHTML = Object.keys(mapa).map(f => {
    const vals = mapa[f].map(d => d.percentual);
    const min = Math.min(...vals);
    const max = Math.max(...vals);

    let badge = 'secondary';
    let texto = 'Normal';

    if (max < 90 && min < 20) {
      badge = 'danger';
      texto = 'Altíssimo';
    } else if (max < 90) {
      badge = 'warning';
      texto = 'Alto';
    }

    return `
      <tr>
        <td>${f}</td>
        <td>${min.toFixed(1)}%</td>
        <td>${max.toFixed(1)}%</td>
        <td><span class="badge bg-${badge}">${texto}</span></td>
      </tr>`;
  }).join('');
}

/* ENTRY POINT */
document.addEventListener('DOMContentLoaded', () => {
  if (window.DATA_CONFIG?.type === 'estatisticas') {
    carregarEstatisticas();
  }
});
