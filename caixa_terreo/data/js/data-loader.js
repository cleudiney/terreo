/* /js/data-loader.js */
const dataLoader = {
  afterLoad: null,

  async load() {
    const cfg = window.DATA_CONFIG;
    if (!cfg?.file) return [];

    const resp = await fetch(`/${cfg.file}`);
    if (!resp.ok) throw new Error(`Erro ao carregar ${cfg.file}`);
    const txt = await resp.text();
    return this.parseByType(cfg.type, txt);
  },

  parseByType(type, txt) {
    const linhas = txt
      .split('\n')
      .map((l) => l.trim())
      .filter(Boolean);

    if (type === 'avisos') return this.parseAvisos(linhas);
    if (type === 'historico') return this.parseHistorico(linhas);
    if (type === 'estatisticas') return this.parseRegistros(linhas);
    if (type === 'pontos') return this.parsePontos(linhas);
    return [];
  },

  parseAvisos(linhas) {
    return linhas.map((l) => {
      const [data, tipo, mensagem, usuario] = l.split(';');
      const tRaw = (tipo || 'evento').toLowerCase();
      const tNorm = tRaw === 'critica' ? 'critico' : tRaw;
      return {
        data: data || '',
        tipo: tNorm,
        mensagem: mensagem || '',
        usuario: usuario || ''
      };
    });
  },

  parseHistorico(linhas) {
    return linhas.map((l) => {
      const [data, hora, evento, percentual, boia, emergencia] = l.split(';');
      return {
        data: data || '',
        hora: hora || '',
        evento: evento || '',
        percentual: Number(percentual) || 0,
        boia: boia === 'true',
        emergencia: emergencia === 'true'
      };
    });
  },

  parseRegistros(linhas) {
    return linhas.map((l) => {
      const [data, hora, pct, nivel, estado, faixa] = l.split(';');
      return {
        data: data || '',
        hora: hora || '',
        percentual: Number(pct) || 0,
        nivel: Number(nivel) || 0,
        estado: estado || '',
        faixa: faixa || 'madrugada'
      };
    });
  },

  parsePontos(linhas) {
    return linhas.map((l) => {
      const [hora, nivel, vazao, status] = l.split(';');
      return {
        hora: hora || '',
        nivel: Number(nivel) || 0,
        vazao: vazao || '',
        status: status || ''
      };
    });
  }
};
const chartRefs = {};

function renderResumo(id, dados) {
  if (!dados.length) {
    document.getElementById(id).innerHTML = '<small>Sem dados</small>';
    return;
  }

  const valores = dados.map((d) => d.percentual);
  const min = Math.min(...valores);
  const max = Math.max(...valores);

  document.getElementById(id).innerHTML = `
    <div>Min: <b>${min.toFixed(1)}%</b></div>
    <div>Max: <b>${max.toFixed(1)}%</b></div>
    <div>Total pontos: <b>${dados.length}</b></div>
  `;
}

function renderFaixas(dados) {
  const tbody = document.getElementById('faixas-horarias');
  if (!tbody) return;

  const mapa = {};
  dados.forEach((d) => {
    if (!mapa[d.faixa]) mapa[d.faixa] = [];
    mapa[d.faixa].push(d);
  });

  tbody.innerHTML = Object.keys(mapa).map((f) => {
    const vals = mapa[f].map((d) => d.percentual);
    const min = Math.min(...vals);
    const max = Math.max(...vals);

    let badge = 'secondary';
    let texto = 'Normal';
    if (max < 90 && min < 20) {
      badge = 'danger';
      texto = 'Altissimo';
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

function renderHistorico(lista) {
  const c = document.getElementById('conteudo');
  if (!c) return;

  if (!lista.length) {
    c.innerHTML = '<div class="alert alert-info text-center">Sem historico.</div>';
    return;
  }

  c.innerHTML = `
    <div class="table-responsive">
      <table class="table table-striped align-middle">
        <thead>
          <tr>
            <th>Data</th>
            <th>Hora</th>
            <th>Evento</th>
            <th>Nivel</th>
            <th>Boia</th>
            <th>Emergencia</th>
          </tr>
        </thead>
        <tbody>
          ${lista.map((r) => `
            <tr>
              <td>${r.data}</td>
              <td>${r.hora}</td>
              <td>${r.evento}</td>
              <td>${r.percentual.toFixed(1)}%</td>
              <td>${r.boia ? 'SIM' : 'NAO'}</td>
              <td>${r.emergencia ? 'SIM' : 'NAO'}</td>
            </tr>`).join('')}
        </tbody>
      </table>
    </div>
  `;

  const stats = document.getElementById('stats');
  if (stats) {
    const emerg = lista.filter((x) => x.emergencia).length;
    stats.innerHTML = `
      <div class="col-md-4"><div class="stats-card">Total: <b>${lista.length}</b></div></div>
      <div class="col-md-4"><div class="stats-card">Emergencias: <b>${emerg}</b></div></div>
      <div class="col-md-4"><div class="stats-card">Ultimo nivel: <b>${lista[lista.length - 1].percentual.toFixed(1)}%</b></div></div>
    `;
  }

  renderHistoricoChart(lista);
}

function renderEstatisticas(registros) {
  const agora = new Date();
  const hojeStr = agora.toISOString().slice(0, 10);

  const inicioSemana = new Date(agora);
  inicioSemana.setDate(agora.getDate() - ((agora.getDay() + 6) % 7));

  const hoje = [];
  const semana = [];
  const mes = [];

  registros.forEach((r) => {
    const d = new Date(`${r.data}T${r.hora}`);
    if (r.data === hojeStr) hoje.push(r);
    if (!Number.isNaN(d.getTime()) && d >= inicioSemana) semana.push(r);
    if (!Number.isNaN(d.getTime()) && d.getMonth() === agora.getMonth()) mes.push(r);
  });

  renderResumo('stat-hoje', hoje);
  renderResumo('stat-semana', semana);
  renderResumo('stat-mes', mes);
  renderFaixas(hoje);
  renderPicos(registros);
  renderEstatisticasChart(registros);
}

function renderHistoricoChart(lista) {
  if (typeof Chart === 'undefined') return;
  const canvas = document.getElementById('graficoHistorico');
  if (!canvas) return;

  const labels = lista.map((r) => `${r.data} ${r.hora}`);
  const valores = lista.map((r) => r.percentual);

  if (chartRefs.historico) chartRefs.historico.destroy();
  chartRefs.historico = new Chart(canvas, {
    type: 'line',
    data: {
      labels,
      datasets: [{
        label: 'Nivel (%)',
        data: valores,
        tension: 0.25,
        fill: true
      }]
    },
    options: {
      responsive: true,
      plugins: { legend: { display: true } }
    }
  });
}

function renderEstatisticasChart(registros) {
  if (typeof Chart === 'undefined') return;
  const canvas = document.getElementById('graficoEstatisticas');
  if (!canvas) return;

  const ordenados = [...registros].sort((a, b) => {
    const ta = new Date(`${a.data}T${a.hora}`).getTime();
    const tb = new Date(`${b.data}T${b.hora}`).getTime();
    return ta - tb;
  });

  const labels = ordenados.map((r) => `${r.data} ${r.hora}`);
  const valores = ordenados.map((r) => r.percentual);

  if (chartRefs.estatisticas) chartRefs.estatisticas.destroy();
  chartRefs.estatisticas = new Chart(canvas, {
    type: 'line',
    data: {
      labels,
      datasets: [{
        label: 'Percentual da caixa',
        data: valores,
        tension: 0.25,
        fill: false
      }]
    },
    options: {
      responsive: true,
      scales: {
        y: {
          min: 0,
          max: 100,
          ticks: { callback: (v) => `${v}%` }
        }
      }
    }
  });
}

function renderPicos(registros) {
  const el = document.getElementById('picos-consumo');
  if (!el) return;

  if (!registros.length) {
    el.innerHTML = '<div class="text-muted">Sem dados para analise de picos.</div>';
    return;
  }

  const ordenados = [...registros].sort((a, b) => {
    const ta = new Date(`${a.data}T${a.hora}`).getTime();
    const tb = new Date(`${b.data}T${b.hora}`).getTime();
    return ta - tb;
  });

  let maiorQueda = { delta: 0, atual: null, anterior: null };
  let maiorRecuperacao = { delta: 0, atual: null, anterior: null };

  for (let i = 1; i < ordenados.length; i++) {
    const anterior = ordenados[i - 1];
    const atual = ordenados[i];
    const delta = atual.percentual - anterior.percentual;

    if (delta < maiorQueda.delta) {
      maiorQueda = { delta, atual, anterior };
    }
    if (delta > maiorRecuperacao.delta) {
      maiorRecuperacao = { delta, atual, anterior };
    }
  }

  const menorNivel = ordenados.reduce((acc, r) => {
    if (!acc || r.percentual < acc.percentual) return r;
    return acc;
  }, null);

  const porFaixa = {};
  ordenados.forEach((r) => {
    if (!porFaixa[r.faixa]) porFaixa[r.faixa] = [];
    porFaixa[r.faixa].push(r.percentual);
  });

  let faixaCritica = '-';
  let mediaCritica = Number.POSITIVE_INFINITY;
  Object.keys(porFaixa).forEach((faixa) => {
    const vals = porFaixa[faixa];
    const media = vals.reduce((s, v) => s + v, 0) / vals.length;
    if (media < mediaCritica) {
      mediaCritica = media;
      faixaCritica = faixa;
    }
  });

  const quedaTxt = maiorQueda.atual
    ? `${Math.abs(maiorQueda.delta).toFixed(1)}% em ${maiorQueda.atual.data} ${maiorQueda.atual.hora}`
    : 'Sem queda relevante';

  const recTxt = maiorRecuperacao.atual
    ? `${maiorRecuperacao.delta.toFixed(1)}% em ${maiorRecuperacao.atual.data} ${maiorRecuperacao.atual.hora}`
    : 'Sem recuperacao relevante';

  const menorTxt = menorNivel
    ? `${menorNivel.percentual.toFixed(1)}% em ${menorNivel.data} ${menorNivel.hora}`
    : '-';

  el.innerHTML = `
    <ul class="mb-0">
      <li><b>Maior queda:</b> ${quedaTxt}</li>
      <li><b>Menor nivel observado:</b> ${menorTxt}</li>
      <li><b>Maior recuperacao:</b> ${recTxt}</li>
      <li><b>Faixa mais critica (media):</b> ${faixaCritica} (${Number.isFinite(mediaCritica) ? mediaCritica.toFixed(1) : '-'}%)</li>
    </ul>
  `;
}

document.addEventListener('DOMContentLoaded', async () => {
  const cfg = window.DATA_CONFIG;
  if (!cfg?.type) return;

  try {
    const dados = await dataLoader.load();

    if (cfg.type === 'estatisticas') renderEstatisticas(dados);
    if (cfg.type === 'historico') renderHistorico(dados);

    if (typeof dataLoader.afterLoad === 'function') {
      dataLoader.afterLoad(dados);
    }
  } catch (e) {
    const target = document.querySelector(cfg.target || '#conteudo');
    if (target) {
      target.innerHTML = `<div class="alert alert-warning">Falha ao carregar dados: ${e.message}</div>`;
    }
  }
});
