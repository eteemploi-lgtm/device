const BATTERY_TEST_JSON = {
  type: "state",
  batterie: {
    status: {
      etatcommCyclable: 1,
      etatcommIVT: 1,
      etatcommLTC: 1,
      etatcommDisques: 1
    },

    etat: {
      courant: 100.0,
      tension: 100.0,
      tensioncharge: 100.0
    },

    infoelt: {
      elt1tensionmin: 4058,
      elt1tensionmax: 4070,
      elt1tempmin: 22,
      elt1tempmax: 26,
      elt1bmsstate: 1,

      elt2tensionmin: 4056,
      elt2tensionmax: 4072,
      elt2tempmin: 23,
      elt2tempmax: 27,
      elt2bmsstate: 1,

      elt3tensionmin: 3980,
      elt3tensionmax: 4075,
      elt3tempmin: 24,
      elt3tempmax: 74,
      elt3bmsstate: 2,

      elt4tensionmin: 4060,
      elt4tensionmax: 6553,
      elt4tempmin: 22,
      elt4tempmax: 74,
      elt4bmsstate: 3,

      elt5tensionmin: 4062,
      elt5tensionmax: 6553,
      elt5tempmin: 23,
      elt5tempmax: 74,
      elt5bmsstate: 3
    },

    tempelt: {
      elt1tempdisque1: 22,
      elt1tempdisque2: 23,
      elt2tempdisque1: 22,
      elt2tempdisque2: 24,
      elt3tempdisque1: 25,
      elt3tempdisque2: 74,
      elt4tempdisque1: 23,
      elt4tempdisque2: 74,
      elt5tempdisque1: 24,
      elt5tempdisque2: 74
    },

    tensionelt: {
      "Disque1.1": {
        elt1tension: 4067, elt2tension: 4067, elt3tension: 4067, elt4tension: 4068,
        elt5tension: 4068, elt6tension: 4068, elt7tension: 4067, elt8tension: 4067,
        elt9tension: 4062, elt10tension: 4062, elt11tension: 4067, elt12tension: 4067,
        elt13tension: 4068, elt14tension: 4067
      },

      "Disque1.2": {
        elt15tension: 4068, elt16tension: 4068, elt17tension: 4064, elt18tension: 4063,
        elt19tension: 4063, elt20tension: 4057, elt21tension: 4067, elt22tension: 4063,
        elt23tension: 4063, elt24tension: 4063, elt25tension: 4067, elt26tension: 4068,
        elt27tension: 4067
      },

      "Disque2.1": {
        elt1tension: 4066, elt2tension: 4067, elt3tension: 4067, elt4tension: 4067,
        elt5tension: 4067, elt6tension: 4067, elt7tension: 4067, elt8tension: 4067,
        elt9tension: 4062, elt10tension: 4062, elt11tension: 4066, elt12tension: 4067,
        elt13tension: 4068, elt14tension: 4067
      },

      "Disque2.2": {
        elt15tension: 4067, elt16tension: 4067, elt17tension: 4067, elt18tension: 4063,
        elt19tension: 4062, elt20tension: 4056, elt21tension: 4067, elt22tension: 4062,
        elt23tension: 4062, elt24tension: 4062, elt25tension: 4067, elt26tension: 4068,
        elt27tension: 4067
      },

      "Disque3.1": {
        elt1tension: 993, elt2tension: 1144, elt3tension: 1177, elt4tension: 862,
        elt5tension: 1137, elt6tension: 573, elt7tension: 474, elt8tension: 810,
        elt9tension: 268, elt10tension: 440, elt11tension: 1084, elt12tension: 824,
        elt13tension: 1098, elt14tension: 964
      },

      "Disque3.2": {
        elt15tension: 4068, elt16tension: 4067, elt17tension: 4063, elt18tension: 4063,
        elt19tension: 4062, elt20tension: 4057, elt21tension: 4067, elt22tension: 4062,
        elt23tension: 4062, elt24tension: 4062, elt25tension: 4067, elt26tension: 4067,
        elt27tension: 4067
      },

      "Disque4.1": {
        elt1tension: 6553, elt2tension: 6553, elt3tension: 6553, elt4tension: 6553,
        elt5tension: 6553, elt6tension: 6553, elt7tension: 6553, elt8tension: 6553,
        elt9tension: 6553, elt10tension: 6553, elt11tension: 6553, elt12tension: 6553,
        elt13tension: 6553, elt14tension: 6553
      },

      "Disque4.2": {
        elt15tension: 6553, elt16tension: 6553, elt17tension: 6553, elt18tension: 6553,
        elt19tension: 6553, elt20tension: 6553, elt21tension: 6553, elt22tension: 6553,
        elt23tension: 6553, elt24tension: 6553, elt25tension: 6553, elt26tension: 6553,
        elt27tension: 0
      },

      "Disque5.1": {
        elt1tension: 6553, elt2tension: 6553, elt3tension: 6553, elt4tension: 6553,
        elt5tension: 6553, elt6tension: 6553, elt7tension: 6553, elt8tension: 6553,
        elt9tension: 6553, elt10tension: 6553, elt11tension: 6553, elt12tension: 6553,
        elt13tension: 6553, elt14tension: 6553
      },

      "Disque5.2": {
        elt15tension: 6553, elt16tension: 6553, elt17tension: 6553, elt18tension: 6553,
        elt19tension: 6553, elt20tension: 6553, elt21tension: 6553, elt22tension: 6553,
        elt23tension: 6553, elt24tension: 6553, elt25tension: 6553, elt26tension: 6553,
        elt27tension: 0
      }
    },

    elements: [
      { name: "Uel1 / Uel15", min: 993, moy: 4713, max: 6553 },
      { name: "Uel2 / Uel16", min: 1144, moy: 4728, max: 6553 },
      { name: "Uel3 / Uel17", min: 1177, moy: 4715, max: 6553 },
      { name: "Uel4 / Uel18", min: 862, moy: 4684, max: 6553 },
      { name: "Uel5 / Uel19", min: 1137, moy: 4703, max: 6553 },
      { name: "Uel6 / Uel20", min: 573, moy: 4650, max: 6553 },
      { name: "Uel7 / Uel21", min: 474, moy: 4644, max: 6553 },
      { name: "Uel8 / Uel22", min: 810, moy: 4663, max: 6553 },
      { name: "Uel9 / Uel23", min: 268, moy: 4604, max: 6553 },
      { name: "Uel10 / Uel24", min: 440, moy: 4620, max: 6553 },
      { name: "Uel11 / Uel25", min: 1084, moy: 4707, max: 6553 },
      { name: "Uel12 / Uel26", min: 824, moy: 4684, max: 6553 },
      { name: "Uel13 / Uel27", min: 1098, moy: 4707, max: 6553 },
      { name: "Uel14", min: 964, moy: 4693, max: 6553 }
    ],

    MoyenDisque1: {
      tensionMin: 4057,
      tensionMax: 4068,
      tensionMoyen: 4065,
      Delta: 11
    },

    MoyenDisque2: {
      tensionMin: 4056,
      tensionMax: 4068,
      tensionMoyen: 4065,
      Delta: 12
    },

    MoyenDisque3: {
      tensionMin: 268,
      tensionMax: 4068,
      tensionMoyen: 2395,
      Delta: 3800
    },

    MoyenDisque4: {
      tensionMin: 6553,
      tensionMax: 6553,
      tensionMoyen: 6553,
      Delta: 0
    },

    MoyenDisque5: {
      tensionMin: 6553,
      tensionMax: 6553,
      tensionMoyen: 6553,
      Delta: 0
    }
  }
};



/* ============================================================
   ONMESSAGE PRINCIPAL
   ============================================================ */

ws.onmessage = function(evt) {
    try {
        const data = JSON.parse(evt.data);

        if (data.type !== "state") return;

        if (data.motor) {
            renderMotor(data.motor);
        }

        if (data.sensors) {
            renderSensors(data.sensors);
        }

        if (data.gouverne) {
            renderGouv(data.gouverne);
        }

        if (data.batterie) {
            console.log("BATTERIE =", data.batterie);
            renderBattery(data.batterie);
        }

    } catch (err) {
        console.error("Erreur JSON WebSocket :", err);
        console.log("Message reçu :", evt.data);
    }
};


/* ============================================================
   ÉTAT GLOBAL BATTERIE
   ============================================================ */

let BAT_STATE = {
    comm: {
        cyclable: 0,
        ivt: 0,
        ltc: 0,
        disques: 0
    },

    mesures: {
        courant: null,
        tension: null,
        tensionCharge: null,
        puissance: null
    },

    modules: [],
    temperatures: [],
    disques: [],
    moyens: []
};


/* ============================================================
   FONCTION PRINCIPALE
   ============================================================ */

function renderBattery(batterie) {
    if (!batterie) return;

    batteryUpdateState(batterie);

    batteryRenderLeds();
    batteryRenderKpis();
    batteryRenderModules();
    batteryRenderDiskTable();
    batteryRenderCharts();
    batteryRenderAlerts();
}


/* ============================================================
   UPDATE STATE
   ============================================================ */

function batteryUpdateState(batterie) {
    const status = batterie.status || {};
    const etat = batterie.etat || {};
    const infoelt = batterie.infoelt || {};
    const tempelt = batterie.tempelt || {};
    const tensionelt = batterie.tensionelt || {};

    BAT_STATE.comm.cyclable = batteryNum(status.etatcommCyclable) ?? 0;
    BAT_STATE.comm.ivt = batteryNum(status.etatcommIVT) ?? 0;
    BAT_STATE.comm.ltc = batteryNum(status.etatcommLTC) ?? 0;
    BAT_STATE.comm.disques = batteryNum(status.etatcommDisques) ?? 0;

    BAT_STATE.mesures.courant = batteryNum(etat.courant);
    BAT_STATE.mesures.tension = batteryNum(etat.tension);
    BAT_STATE.mesures.tensionCharge = batteryNum(etat.tensioncharge);

    if (BAT_STATE.mesures.courant !== null && BAT_STATE.mesures.tension !== null) {
        BAT_STATE.mesures.puissance =
            (BAT_STATE.mesures.courant * BAT_STATE.mesures.tension) / 1000.0;
    } else {
        BAT_STATE.mesures.puissance = null;
    }

    BAT_STATE.modules = [];

    for (let i = 1; i <= 5; i++) {
        BAT_STATE.modules.push({
            name: `Module ${i}`,
            uMin: batteryNum(infoelt[`elt${i}tensionmin`]),
            uMax: batteryNum(infoelt[`elt${i}tensionmax`]),
            tMin: batteryNum(infoelt[`elt${i}tempmin`]),
            tMax: batteryNum(infoelt[`elt${i}tempmax`]),
            bms: batteryNum(infoelt[`elt${i}bmsstate`])
        });
    }

    BAT_STATE.temperatures = [];

    for (let i = 1; i <= 5; i++) {
        BAT_STATE.temperatures.push({
            name: `${i}.1`,
            value: batteryNum(tempelt[`elt${i}tempdisque1`])
        });

        BAT_STATE.temperatures.push({
            name: `${i}.2`,
            value: batteryNum(tempelt[`elt${i}tempdisque2`])
        });
    }

    BAT_STATE.moyens = [];

    for (let i = 1; i <= 5; i++) {
        const m = batterie[`MoyenDisque${i}`] || {};

        BAT_STATE.moyens.push({
            name: `${i}`,
            tensionMin: batteryNum(m.tensionMin),
            tensionMax: batteryNum(m.tensionMax),
            tensionMoyen: batteryNum(m.tensionMoyen),
            delta: batteryNum(m.Delta)
        });
    }

    BAT_STATE.disques = [];

    for (let i = 1; i <= 5; i++) {
        const d1 = tensionelt[`Disque${i}.1`] || {};
        const d2 = tensionelt[`Disque${i}.2`] || {};
        const moyen = BAT_STATE.moyens[i - 1] || {};

        BAT_STATE.disques.push({
            name: `${i}.1`,
            cells: batteryExtractCells(d1, 1, 14),
            temp: batteryNum(tempelt[`elt${i}tempdisque1`]),
            stats: moyen
        });

        BAT_STATE.disques.push({
            name: `${i}.2`,
            cells: batteryExtractCells(d2, 15, 27),
            temp: batteryNum(tempelt[`elt${i}tempdisque2`]),
            stats: moyen
        });
    }

    console.log("BAT_STATE =", BAT_STATE);
}


/* ============================================================
   EXTRACTION DES TENSIONS DISQUE
   ============================================================ */

function batteryExtractCells(obj, start, end) {
    const cells = [];

    for (let i = start; i <= end; i++) {
        const key = `elt${i}tension`;
        cells.push(batteryNum(obj[key]));
    }

    if (start === 15) {
        cells.push(null);
    }

    return cells;
}


/* ============================================================
   LEDS COMMUNICATION
   ============================================================ */

function batteryRenderLeds() {
    const ledBar = document.getElementById("ledBar");
    if (!ledBar) return;

    const items = [
        { name: "LTC", code: BAT_STATE.comm.ltc },
        { name: "IVT", code: BAT_STATE.comm.ivt },
        { name: "BANC CYCLAGE", code: BAT_STATE.comm.cyclable },
        { name: "DISQUES", code: BAT_STATE.comm.disques }
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


/* ============================================================
   KPI MESURES BATTERIE
   ============================================================ */

function batteryRenderKpis() {
    const kpiBar = document.getElementById("kpiBar");
    if (!kpiBar) return;

    const m = BAT_STATE.mesures;

    const rows = [
        ["U Batt", m.tension, "V"],
        ["I Mesuré", m.courant, "A"],
        ["U Charge", m.tensionCharge, "V"],
        ["Puissance", m.puissance, "kW"],
        ["U Décharge", null, "V"],
        ["Capacité", null, "Ah"],
        ["Énergie", null, "kWh"]
    ];

    kpiBar.innerHTML = rows.map(([label, value, unit]) => `
        <div class="kpi">
            <span class="lbl">${label}</span>
            ${batteryFormatValue(value, unit, 2)}
        </div>
    `).join("");
}


/* ============================================================
   MODULES BMS
   ============================================================ */

function batteryRenderModules() {
    const modTable = document.getElementById("modTable");
    if (!modTable) return;

    const M = BAT_STATE.modules;

    const head = `
        <tr>
            <th></th>
            ${M.map(m => `<th>${m.name}</th>`).join("")}
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
                ${row("Uel min", "uMin", "mV")}
                ${row("Uel max", "uMax", "mV")}
                ${row("T°C min", "tMin", "°C")}
                ${row("T°C max", "tMax", "°C")}
                ${row("BMS state", "bms", "")}
            </tbody>
        </table>
    `;
}


/* ============================================================
   TABLEAU DES DISQUES
   ============================================================ */

function batteryRenderDiskTable() {
    const batTable = document.getElementById("batTable");
    if (!batTable) return;

    const D = BAT_STATE.disques || [];

    if (!D.length) {
        batTable.innerHTML =
            `<p style="padding:12px;color:var(--txt-dim)">Aucune donnée disque disponible</p>`;
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
                    return `
                        <td class="${batteryClassTensionMv(value)}">
                            ${value ?? ""}
                        </td>
                    `;
                }).join("")}
            </tr>
        `;
    }

    rows += `
        <tr class="sep">
            <th>UelMin / UelMax</th>
            ${D.map(d => `
                <td class="c-minblue">
                    ${d.stats?.tensionMin ?? "–"} / ${d.stats?.tensionMax ?? "–"}
                </td>
            `).join("")}
        </tr>

        <tr>
            <th>UelMoy / Delta</th>
            ${D.map(d => `
                <td class="c-yellow">
                    ${d.stats?.tensionMoyen ?? "–"} / ${d.stats?.delta ?? "–"}
                </td>
            `).join("")}
        </tr>

        <tr class="sep">
            <th>T°C 1/2</th>
            ${D.map(d => `<td>${batteryCell(d.temp, "°C")}</td>`).join("")}
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


/* ============================================================
   GRAPHES
   ============================================================ */

function batteryRenderCharts() {
    batteryRenderTensionChart();
    batteryRenderTempChart();
}

function batteryRenderTensionChart() {
    const target = document.getElementById("chartTension");
    if (!target) return;

    const values = [];

    BAT_STATE.moyens.forEach(m => {
        values.push(m.tensionMin);
        values.push(m.tensionMoyen);
        values.push(m.tensionMax);
    });

    batteryRenderSimpleChart(target, values, 2400, 4300, "mV");
}

function batteryRenderTempChart() {
    const target = document.getElementById("chartTemp");
    if (!target) return;

    const values = BAT_STATE.temperatures.map(t => t.value);

    batteryRenderSimpleChart(target, values, 0, 80, "°C");
}

function batteryRenderSimpleChart(target, values, yMin, yMax, unit) {
    values = values.filter(v => v !== null && v !== undefined);

    if (!values.length) {
        target.innerHTML =
            `<p style="padding:12px;color:var(--txt-dim)">Aucune donnée</p>`;
        return;
    }

    const W = 560;
    const H = 260;
    const M = { top: 20, right: 30, bottom: 30, left: 45 };
    const iw = W - M.left - M.right;
    const ih = H - M.top - M.bottom;

    const x = i => M.left + iw * ((i + 0.5) / values.length);
    const y = v => M.top + ih * (1 - (v - yMin) / (yMax - yMin));

    let svg = `<svg viewBox="0 0 ${W} ${H}">`;

    for (let k = 0; k <= 5; k++) {
        const val = yMin + ((yMax - yMin) * k / 5);
        const yy = y(val);

        svg += `
            <line class="gridline" x1="${M.left}" x2="${W - M.right}" y1="${yy}" y2="${yy}" />
            <text class="tick-txt" x="${M.left - 6}" y="${yy + 3}" text-anchor="end">${val.toFixed(0)}</text>
        `;
    }

    values.forEach((v, i) => {
        const xx = x(i);
        const yy = y(v);

        svg += `
            <line class="stem" x1="${xx}" y1="${M.top + ih}" x2="${xx}" y2="${yy}" />
            <circle class="marker" cx="${xx}" cy="${yy}" r="4" />
        `;
    });

    svg += `</svg>`;

    target.innerHTML = svg;
}


/* ============================================================
   ALERTES
   ============================================================ */

function batteryRenderAlerts() {
    const zone = document.getElementById("alertZone");
    if (!zone) return;

    zone.className = "alert-zone ok";
    zone.innerHTML = `<span class="ico">✓</span><span>Aucune alerte</span>`;
}


/* ============================================================
   OUTILS
   ============================================================ */

function batteryNum(v) {
    if (v === undefined || v === null || v === "") return null;
    const n = Number(v);
    return Number.isFinite(n) ? n : null;
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

    return `${Number(value).toFixed(0)} ${unit}`;
}

function batteryClassTensionMv(v) {
    if (v === null || v === undefined || v === 0) return "c-grey";
    if (v < 2500) return "c-blue";
    if (v > 4200) return "c-red";
    if (v > 4100) return "c-yellow";
    return "c-green";
}
