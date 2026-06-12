/* ============================================================
   BATTERIE.JS — À AJOUTER AVEC moteur / gouverne / capteurs
   ============================================================ */

/* ===================== SEUILS BATTERIE ===================== */
const BAT_SEUIL = {
  U_HAUT: 4.20,
  U_BAS: 2.50,
  U_VIGIL: 0.05,

  T_OK: 45,
  T_WARN: 60,
  T_HAUT: 75,
  T_CRIT: 80,

  DELTA_U_WARN: 0.050,

  MV_BAS: 2500,
  MV_HAUT: 4200,
  MV_VIGIL: 4100
};

/* ===================== ÉTAT BATTERIE ===================== */
let BAT_STATE = {
  comm: {
    courant: 0,
    tension: 0,
    charge: 0,
    ltc: 0,
    disques: 0
  },

  mesures: {
    courant: null,
    tension: null,
    tensionCharge: null,
    tensionDecharge: null,
    capacite: null,
    puissance: null,
    energie: null
  },

  tension: [],
  temperature: [],
  modules: [],
  disques: []
};

/* ============================================================
   POINT D’ENTRÉE PRINCIPAL
   À appeler depuis ton WebSocket :
   if (data.type === "state" && data.batterie) renderBattery(data.batterie);
   ============================================================ */
function renderBattery(batterie) {
  if (!batterie) return;

  batteryUpdateState(batterie);

  batteryRenderLeds();
  batteryRenderKpis();
  batteryRenderMetrics();
  batteryRenderModules();
  batteryRenderDiskTable();
  batteryRenderCharts();
  batteryRenderAlerts();
}

/* ============================================================
   1. CONVERSION JSON BACKEND -> BAT_STATE
   ============================================================ */
function batteryUpdateState(b) {
  const etat = b.etat || {};
  const infoelt = b.infoelt || {};
  const tempelt = b.tempelt || {};
  const tensionelt = b.tensionelt || {};

  BAT_STATE.comm.courant = Number(etat.etatcommcourant ?? 0);
  BAT_STATE.comm.tension = Number(etat.etatcommtension ?? 0);
  BAT_STATE.comm.charge = Number(etat.etatcommtensioncharge ?? 0);

  BAT_STATE.comm.ltc = batteryWorstComm(
    BAT_STATE.comm.courant,
    BAT_STATE.comm.tension
  );

  BAT_STATE.comm.disques = batteryWorstComm(
    BAT_STATE.comm.courant,
    BAT_STATE.comm.tension,
    BAT_STATE.comm.charge
  );

  BAT_STATE.mesures.courant = batteryNum(etat.courant);
  BAT_STATE.mesures.tension = batteryNum(etat.tension);
  BAT_STATE.mesures.tensionCharge = batteryNum(etat.tensioncharge);

  if (
    BAT_STATE.mesures.courant !== null &&
    BAT_STATE.mesures.tension !== null
  ) {
    BAT_STATE.mesures.puissance =
      (BAT_STATE.mesures.courant * BAT_STATE.mesures.tension) / 1000.0;
  }

  BAT_STATE.tension = [];
  for (let elt = 1; elt <= 7; elt++) {
    for (let n = 1; n <= 4; n++) {
      const key = `elt${elt}tension${n}`;
      const value = batteryNum(tensionelt[key]);
      if (value !== null) BAT_STATE.tension.push(value);
    }
  }

  BAT_STATE.temperature = [];
  for (let elt = 1; elt <= 5; elt++) {
    for (let disque = 1; disque <= 2; disque++) {
      const key = `elt${elt}tempdisque${disque}`;
      const value = batteryNum(tempelt[key]);
      if (value !== null) BAT_STATE.temperature.push(value);
    }
  }

  BAT_STATE.modules = [];
  for (let i = 1; i <= 5; i++) {
    BAT_STATE.modules.push({
      uMin: batteryNum(infoelt[`elt${i}tensionmin`]),
      uMax: batteryNum(infoelt[`elt${i}tensionmax`]),
      tMin: batteryNum(infoelt[`elt${i}tempmin`]),
      tMax: batteryNum(infoelt[`elt${i}tempmax`]),
      bms: batteryNum(infoelt[`elt${i}bmsstate`])
    });
  }

  BAT_STATE.disques = batteryBuildDisquesFromBackend();
}

/* ============================================================
   2. LEDs COMMUNICATION
   ============================================================ */
function batteryRenderLeds() {
  const ledBar = document.getElementById("ledBar");
  if (!ledBar) return;

  const items = [
    {
      name: "LTC",
      code: BAT_STATE.comm.ltc
    },
    {
      name: "IVT",
      code: batteryWorstComm(BAT_STATE.comm.courant, BAT_STATE.comm.tension)
    },
    {
      name: "BANC CYCLAGE",
      code: BAT_STATE.comm.charge
    },
    {
      name: "DISQUES",
      code: BAT_STATE.comm.disques
    }
  ];

  ledBar.innerHTML = items.map(item => {
    const cls = batteryCommClass(item.code);
    return `
      <div class="led-item">
        <span class="led ${cls}"></span>
        <div>
          <div class="nm">${item.name}</div>
          <div class="st">${batteryCommText(cls)}</div>
        </div>
      </div>
    `;
  }).join("");
}

function batteryCommClass(code) {
  if (code === 1) return "green";
  if (code === 2) return "orange";
  if (code === 3) return "red";
  return "grey";
}

function batteryCommText(cls) {
  if (cls === "green") return "Valide";
  if (cls === "orange") return "Dégradée";
  if (cls === "red") return "Perdue";
  return "Non démarré";
}

function batteryWorstComm(...codes) {
  if (codes.includes(3)) return 3;
  if (codes.includes(2)) return 2;
  if (codes.includes(1)) return 1;
  return 0;
}

/* ============================================================
   3. MESURES BATTERIE
   ============================================================ */
function batteryRenderKpis() {
  const kpiBar = document.getElementById("kpiBar");
  if (!kpiBar) return;

  const m = BAT_STATE.mesures;

  const rows = [
    ["U Batt", m.tension, "V", 3],
    ["I Mesuré", m.courant, "A", 3],
    ["U Charge", m.tensionCharge, "V", 3],
    ["U Décharge", m.tensionDecharge, "V", 3],
    ["Capacité", m.capacite, "Ah", 3],
    ["Puissance", m.puissance, "kW", 3],
    ["Énergie", m.energie, "kWh", 3]
  ];

  kpiBar.innerHTML = rows.map(([label, value, unit, dec]) => `
    <div class="kpi">
      <span class="lbl">${label}</span>
      ${batteryFormatValue(value, unit, dec)}
    </div>
  `).join("");
}

/* ============================================================
   4. MÉTRIQUES TENSIONS / TEMPÉRATURES
   ============================================================ */
function batteryRenderMetrics() {
  const U = BAT_STATE.tension;
  const T = BAT_STATE.temperature;

  if (U.length > 0) {
    const uMin = Math.min(...U);
    const uMax = Math.max(...U);
    const delta = uMax - uMin;
    const ratio = uMin !== 0 ? (delta / uMin) * 100 : 0;

    batterySetHtml("uMin", `${uMin.toFixed(3)} <small>V</small>`);
    batterySetHtml("uMax", `${uMax.toFixed(3)} <small>V</small>`);
    batterySetHtml("uDelta", `${(delta * 1000).toFixed(0)} <small>mV</small>`);
    batterySetHtml("uCible", `${uMin.toFixed(3)} <small>V</small>`);
    batterySetHtml("uRatio", `${ratio.toFixed(2)} <small>%</small>`);

    const bar = document.getElementById("uRatioBar");
    if (bar) bar.style.width = Math.min(ratio * 6, 100) + "%";
  }

  if (T.length > 0) {
    const tMin = Math.min(...T);
    const tMax = Math.max(...T);
    const delta = tMax - tMin;

    batterySetHtml("tMin", `${tMin.toFixed(1)} <small>°C</small>`);
    batterySetHtml("tMax", `${tMax.toFixed(1)} <small>°C</small>`);
    batterySetHtml("tDelta", `${delta.toFixed(1)} <small>°C</small>`);
  }
}

/* ============================================================
   5. TABLEAU MODULES BMS
   ============================================================ */
function batteryRenderModules() {
  const modTable = document.getElementById("modTable");
  if (!modTable) return;

  const M = BAT_STATE.modules;

  const head = `
    <tr>
      <th></th>
      ${M.map((_, i) => `<th>Module ${i + 1}</th>`).join("")}
    </tr>
  `;

  const row = (label, key, unit = "") => `
    <tr>
      <th>${label}</th>
      ${M.map(m => `<td>${batteryCell(m[key], unit)}</td>`).join("")}
    </tr>
  `;

  modTable.innerHTML = `
    <table class="mod-table">
      <thead>${head}</thead>
      <tbody>
        ${row("Uel min", "uMin", "V")}
        ${row("Uel max", "uMax", "V")}
        ${row("T°C min", "tMin", "°C")}
        ${row("T°C max", "tMax", "°C")}
        ${row("BMS state", "bms", "")}
      </tbody>
    </table>
  `;
}

/* ============================================================
   6. TABLEAU CENTRAL DES DISQUES
   ============================================================ */
function batteryRenderDiskTable() {
  const batTable = document.getElementById("batTable");
  if (!batTable) return;

  const D = BAT_STATE.disques;
  if (!D.length) {
    batTable.innerHTML = `<p style="padding:12px;color:var(--txt-dim)">Aucune donnée disque disponible</p>`;
    return;
  }

  let rows = "";

  for (let r = 0; r < 14; r++) {
    const label = r < 13 ? `Uel${r + 1} / Uel${r + 15}` : "Uel14";

    rows += `
      <tr>
        <th>${label}</th>
        ${D.map(d => {
          const value = d.cells[r];
          return `<td class="${batteryClassTensionMv(value)}">${value ?? ""}</td>`;
        }).join("")}
      </tr>
    `;
  }

  rows += `
    <tr class="sep">
      <th>UelMin / UelMax</th>
      ${D.map(d => `<td class="c-minblue">${batteryMin(d.cells)}</td>`).join("")}
    </tr>
    <tr>
      <th>UelMoy / Delta</th>
      ${D.map(d => `<td class="c-yellow">${batteryDelta(d.cells)}</td>`).join("")}
    </tr>
    <tr class="sep">
      <th>T°C 1/2</th>
      ${D.map(d => `<td>${batteryCell(d.temp, "°C")}</td>`).join("")}
    </tr>
    <tr>
      <th>T°C BMS/LTC</th>
      ${D.map(d => `<td>${batteryCell(d.tempBms, "°C")}</td>`).join("")}
    </tr>
    <tr>
      <th>Défauts Disque/CAN</th>
      ${D.map(d => `<td>${d.defaut ?? "–"}</td>`).join("")}
    </tr>
  `;

  const head = `
    <tr>
      <th></th>
      ${D.map(d => `<th>Disque ${d.name}</th>`).join("")}
    </tr>
  `;

  batTable.innerHTML = `
    <table class="disk-table">
      <thead>${head}</thead>
      <tbody>${rows}</tbody>
    </table>
  `;
}

function batteryBuildDisquesFromBackend() {
  const D = [];

  for (let i = 1; i <= 5; i++) {
    const module = BAT_STATE.modules[i - 1] || {};
    const temp1 = BAT_STATE.temperature[(i - 1) * 2] ?? null;
    const temp2 = BAT_STATE.temperature[(i - 1) * 2 + 1] ?? null;

    D.push({
      name: `${i}.1`,
      cells: batteryFakeCellsFromModule(module),
      temp: temp1,
      tempBms: module.tMax,
      defaut: module.bms
    });

    D.push({
      name: `${i}.2`,
      cells: batteryFakeCellsFromModule(module),
      temp: temp2,
      tempBms: module.tMax,
      defaut: module.bms
    });
  }

  return D;
}

/*
IMPORTANT :
Ton backend actuel donne :
- 7 x 4 tensions dans batterie.tensionelt
- 5 modules infoelt
- 5 x 2 températures disques

Mais il ne donne pas encore directement le tableau complet 10 disques x 14 lignes.
Donc ici, batteryFakeCellsFromModule() remplit provisoirement le tableau avec les min/max module.

Quand ton backend publiera vraiment :
batterie.disques[0].cells[0..13]
on remplacera cette fonction.
*/
function batteryFakeCellsFromModule(module) {
  const cells = [];

  const min = module.uMin ?? null;
  const max = module.uMax ?? null;

  for (let i = 0; i < 14; i++) {
    if (i === 0) cells.push(batteryVoltToMv(min));
    else if (i === 13) cells.push(batteryVoltToMv(max));
    else if (min !== null && max !== null) {
      const val = min + ((max - min) * i / 13);
      cells.push(batteryVoltToMv(val));
    } else {
      cells.push(null);
    }
  }

  return cells;
}

/* ============================================================
   7. GRAPHES
   ============================================================ */
function batteryRenderCharts() {
  batteryRenderStemChart("chartTension", {
    values: BAT_STATE.tension,
    yMin: 2.4,
    yMax: 4.3,
    seuils: [
      { value: BAT_SEUIL.U_HAUT, label: "4.20 V", color: "var(--danger)" },
      { value: BAT_SEUIL.U_BAS, label: "2.50 V", color: "var(--warn)" }
    ],
    unit: "V",
    type: "tension"
  });

  batteryRenderStemChart("chartTemp", {
    values: BAT_STATE.temperature,
    yMin: 20,
    yMax: 85,
    seuils: [
      { value: BAT_SEUIL.T_HAUT, label: "75 °C", color: "var(--warn)" },
      { value: BAT_SEUIL.T_CRIT, label: "80 °C", color: "var(--danger)" }
    ],
    unit: "°C",
    type: "temperature"
  });
}

function batteryRenderStemChart(targetId, options) {
  const target = document.getElementById(targetId);
  if (!target) return;

  const values = options.values || [];
  if (!values.length) {
    target.innerHTML = `<p style="padding:12px;color:var(--txt-dim)">Aucune donnée</p>`;
    return;
  }

  const W = 560;
  const H = 300;
  const M = { top: 18, right: 60, bottom: 38, left: 44 };
  const iw = W - M.left - M.right;
  const ih = H - M.top - M.bottom;

  const y = v => M.top + ih * (1 - (v - options.yMin) / (options.yMax - options.yMin));
  const x = i => M.left + iw * ((i + 0.5) / values.length);

  let svg = `<svg viewBox="0 0 ${W} ${H}">`;

  for (let k = 0; k <= 6; k++) {
    const val = options.yMin + ((options.yMax - options.yMin) * k / 6);
    const yy = y(val);

    svg += `
      <line class="gridline" x1="${M.left}" x2="${W - M.right}" y1="${yy}" y2="${yy}" />
      <text class="tick-txt" x="${M.left - 7}" y="${yy + 3}" text-anchor="end">${val.toFixed(1)}</text>
    `;
  }

  svg += `
    <line class="axis" x1="${M.left}" y1="${M.top}" x2="${M.left}" y2="${M.top + ih}" />
    <line class="axis" x1="${M.left}" y1="${M.top + ih}" x2="${W - M.right}" y2="${M.top + ih}" />
  `;

  options.seuils.forEach(s => {
    const yy = y(s.value);

    svg += `
      <line class="refline" stroke="${s.color}" x1="${M.left}" x2="${W - M.right}" y1="${yy}" y2="${yy}" />
      <text class="reftxt" fill="${s.color}" x="${W - M.right + 5}" y="${yy + 3}">${s.label}</text>
    `;
  });

  values.forEach((v, i) => {
    const xx = x(i);
    const yy = y(v);
    const color = options.type === "temperature"
      ? batteryColorTemp(v)
      : batteryColorTension(v);

    svg += `
      <line class="stem" stroke="${color}" x1="${xx}" y1="${M.top + ih}" x2="${xx}" y2="${yy}" />
      <circle class="marker" fill="${color}" cx="${xx}" cy="${yy}" r="4" />
    `;
  });

  svg += `</svg>`;

  target.innerHTML = svg;
}

/* ============================================================
   8. ALERTES
   ============================================================ */
function batteryRenderAlerts() {
  const zone = document.getElementById("alertZone");
  if (!zone) return;

  const alerts = [];

  BAT_STATE.tension.forEach((v, i) => {
    if (v > BAT_SEUIL.U_HAUT) {
      alerts.push(`Surtension élément ${i + 1} : ${v.toFixed(3)} V`);
    }
    if (v < BAT_SEUIL.U_BAS) {
      alerts.push(`Sous-tension élément ${i + 1} : ${v.toFixed(3)} V`);
    }
  });

  BAT_STATE.temperature.forEach((v, i) => {
    if (v > BAT_SEUIL.T_CRIT) {
      alerts.push(`Température critique capteur ${i + 1} : ${v.toFixed(1)} °C`);
    } else if (v > BAT_SEUIL.T_HAUT) {
      alerts.push(`Température élevée capteur ${i + 1} : ${v.toFixed(1)} °C`);
    }
  });

  if (!alerts.length) {
    zone.className = "alert-zone ok";
    zone.innerHTML = `<span class="ico">✓</span><span>Aucune alerte</span>`;
    return;
  }

  zone.className = "alert-zone bad";
  zone.innerHTML = `
    <span class="ico">⚠</span>
    <div class="alert-list">
      ${alerts.map(a => `<span>${a}</span>`).join("")}
    </div>
  `;
}

/* ============================================================
   9. OUTILS
   ============================================================ */
function batteryNum(v) {
  if (v === undefined || v === null || v === "") return null;
  const n = Number(v);
  return Number.isFinite(n) ? n : null;
}

function batteryVoltToMv(v) {
  if (v === null || v === undefined) return null;
  return Math.round(Number(v) * 1000);
}

function batteryFormatValue(value, unit, dec = 2) {
  if (value === null || value === undefined || !Number.isFinite(Number(value))) {
    return `<span class="v na">n/d</span>`;
  }

  return `<span class="v">${Number(value).toFixed(dec)}<small>${unit}</small></span>`;
}

function batteryCell(value, unit = "") {
  if (value === null || value === undefined || !Number.isFinite(Number(value))) {
    return "–";
  }

  return `${Number(value).toFixed(2)} ${unit}`;
}

function batterySetHtml(id, html) {
  const el = document.getElementById(id);
  if (el) el.innerHTML = html;
}

function batteryMin(values) {
  const clean = values.filter(v => v !== null && v !== undefined && v !== 0);
  if (!clean.length) return "–";
  return Math.min(...clean);
}

function batteryDelta(values) {
  const clean = values.filter(v => v !== null && v !== undefined && v !== 0);
  if (!clean.length) return "–";
  return Math.max(...clean) - Math.min(...clean);
}

function batteryClassTensionMv(v) {
  if (v === null || v === undefined || v === 0) return "c-grey";
  if (v < BAT_SEUIL.MV_BAS) return "c-blue";
  if (v > BAT_SEUIL.MV_HAUT) return "c-red";
  if (v > BAT_SEUIL.MV_VIGIL) return "c-yellow";
  return "c-green";
}

function batteryColorTension(v) {
  if (v > BAT_SEUIL.U_HAUT || v < BAT_SEUIL.U_BAS) return "var(--danger)";
  if (v > BAT_SEUIL.U_HAUT - BAT_SEUIL.U_VIGIL || v < BAT_SEUIL.U_BAS + BAT_SEUIL.U_VIGIL) return "var(--warn)";
  return "var(--ok)";
}

function batteryColorTemp(v) {
  if (v > BAT_SEUIL.T_WARN) return "var(--danger)";
  if (v > BAT_SEUIL.T_OK) return "var(--warn)";
  return "var(--ok)";
}
