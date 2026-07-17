/* Mission Editor.
 * Phase 1: edit missions/waves/enemies, place enemies on a battlefield canvas,
 *          validate, save to mission.json.
 * Phase 2: "Test in game" saves + hot-reloads the running server.
 * Phase 3: AI builder (edit ai.json — behaviors enemies reference by ai_id).
 * Schema reference: tools/mission-editor/DATA_MODEL.md
 */
"use strict";

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
const S = {
  meta: null,
  units: [],            // enemy palette [{id,name,element,rarity}]
  unitsById: new Map(),
  ais: [],              // full AiRecord[] {id,name,actions[...]}
  missions: [],         // the edited array (MissionRecord[])
  mi: -1,               // current mission index
  si: 0,                // current stage index
  ei: -1,               // selected enemy index
  dirty: false,
  aiIndex: -1,          // AI being edited in the AI builder
  aiDirty: false,
};

const $ = (id) => document.getElementById(id);
const el = (tag, cls, html) => {
  const e = document.createElement(tag);
  if (cls) e.className = cls;
  if (html != null) e.innerHTML = html;
  return e;
};
const api = {
  async get(p) { const r = await fetch(p); if (!r.ok) throw new Error(await r.text()); return r.json(); },
  async post(p, body) {
    const r = await fetch(p, { method: "POST", headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body) });
    if (!r.ok) throw new Error(await r.text());
    return r.json();
  },
};

const cur = () => (S.mi >= 0 ? S.missions[S.mi] : null);
const curStage = () => { const m = cur(); return m && m.stages[S.si]; };
const markDirty = () => { S.dirty = true; renderSaveStatus(); renderValidation(); };

// ---------------------------------------------------------------------------
// Load
// ---------------------------------------------------------------------------
async function boot() {
  try {
    const [meta, units, ais, missions] = await Promise.all([
      api.get("/api/meta"), api.get("/api/units"),
      api.get("/api/ais"), api.get("/api/missions"),
    ]);
    S.meta = meta; S.units = units; S.ais = ais; S.missions = missions;
    S.unitsById = new Map(units.map((u) => [u.id, u]));
    renderMissionList(); renderPalette(); renderSaveStatus();
    if (missions.length) selectMission(0);
  } catch (e) {
    alert("Failed to load archive:\n" + e.message);
  }
}

// ---------------------------------------------------------------------------
// Mission list
// ---------------------------------------------------------------------------
function renderMissionList() {
  const ul = $("missionList"); ul.innerHTML = "";
  S.missions.forEach((m, i) => {
    const li = el("li", i === S.mi ? "active" : "");
    li.innerHTML = `<span class="mname">${escapeHtml(m.name || "(unnamed)")}</span>
                    <span class="mid"> #${m.id} · ${m.stages ? m.stages.length : 0} wave(s)</span>`;
    li.onclick = () => selectMission(i);
    ul.appendChild(li);
  });
}

function selectMission(i) {
  S.mi = i; S.si = 0; S.ei = -1;
  $("noMission").classList.add("hidden");
  $("missionEditor").classList.remove("hidden");
  renderMissionList(); renderHeader(); renderStages(); renderCanvas();
  renderEnemyInspector(); renderValidation();
}

function newMission() {
  const nextId = S.missions.reduce((mx, m) => Math.max(mx, m.id || 0), 0) + 1;
  S.missions.push({
    id: nextId, name: "New Mission", zel: 0, karma: 0, exp: 0, energy_cost: 0,
    stages: [{ is_boss: false, first_attack_rate: 0, battle_monsters: [] }],
  });
  markDirty(); renderMissionList(); selectMission(S.missions.length - 1);
}

function duplicateMission() {
  const m = cur(); if (!m) return;
  const copy = JSON.parse(JSON.stringify(m));
  copy.id = S.missions.reduce((mx, x) => Math.max(mx, x.id || 0), 0) + 1;
  copy.name = (m.name || "Mission") + " (copy)";
  S.missions.splice(S.mi + 1, 0, copy);
  markDirty(); selectMission(S.mi + 1);
}

function deleteMission() {
  const m = cur(); if (!m) return;
  if (!confirm(`Delete mission #${m.id} "${m.name}"?`)) return;
  S.missions.splice(S.mi, 1); markDirty();
  if (!S.missions.length) {
    S.mi = -1; $("missionEditor").classList.add("hidden");
    $("noMission").classList.remove("hidden"); renderMissionList();
  } else selectMission(Math.max(0, S.mi - 1));
}

// ---------------------------------------------------------------------------
// Header
// ---------------------------------------------------------------------------
function renderHeader() {
  const m = cur(); if (!m) return;
  $("fMid").value = m.id; $("fName").value = m.name || "";
  $("fZel").value = m.zel || 0; $("fKarma").value = m.karma || 0;
  $("fExp").value = m.exp || 0; $("fEnergy").value = m.energy_cost || 0;
}
function bindHeader() {
  const map = { fMid: "id", fName: "name", fZel: "zel", fKarma: "karma", fExp: "exp", fEnergy: "energy_cost" };
  for (const [id, key] of Object.entries(map)) {
    $(id).addEventListener("input", (e) => {
      const m = cur(); if (!m) return;
      m[key] = key === "name" ? e.target.value : intOr0(e.target.value);
      if (id === "fMid" || id === "fName") renderMissionList();
      markDirty();
    });
  }
  $("btnMstRewards").onclick = async () => {
    const m = cur(); if (!m) return;
    const r = await api.get("/api/mission-mst?id=" + encodeURIComponent(m.id));
    if (!r || r.zel == null) { alert("No mission MST entry for id " + m.id); return; }
    m.zel = r.zel; m.karma = r.karma; m.exp = r.exp; m.energy_cost = r.energy_cost;
    renderHeader(); markDirty();
  };
}

// ---------------------------------------------------------------------------
// Stages / waves
// ---------------------------------------------------------------------------
function renderStages() {
  const m = cur(); if (!m) return;
  const tabs = $("stageTabs"); tabs.innerHTML = "";
  m.stages.forEach((st, i) => {
    const t = el("div", "stage-tab" + (i === S.si ? " active" : "") + (st.is_boss ? " boss" : ""));
    t.innerHTML = `Wave ${i + 1}${st.is_boss ? " 👑" : ""} <span class="muted">(${st.battle_monsters.length})</span>`;
    const x = el("span", "x", "✕"); x.title = "Delete wave";
    x.onclick = (ev) => { ev.stopPropagation(); deleteStage(i); };
    t.appendChild(x);
    t.onclick = () => { S.si = i; S.ei = -1; renderStages(); renderCanvas(); renderEnemyInspector(); };
    tabs.appendChild(t);
  });
  const st = curStage();
  const props = $("stageProps"); props.innerHTML = "";
  if (st) {
    const boss = el("label", "", `<input type="checkbox" ${st.is_boss ? "checked" : ""}> Boss wave`);
    boss.querySelector("input").onchange = (e) => { st.is_boss = e.target.checked; renderStages(); renderCanvas(); markDirty(); };
    const fa = el("label", "", `First-attack % <input type="number" min="0" max="100" value="${st.first_attack_rate || 0}">`);
    fa.querySelector("input").oninput = (e) => { st.first_attack_rate = clamp(intOr0(e.target.value), 0, 100); markDirty(); };
    props.append(boss, fa);
  }
  $("stageLabel").textContent = st ? `— Wave ${S.si + 1}` : "";
}
function addStage() {
  const m = cur(); if (!m) return;
  m.stages.push({ is_boss: false, first_attack_rate: 0, battle_monsters: [] });
  S.si = m.stages.length - 1; S.ei = -1; markDirty();
  renderStages(); renderCanvas(); renderEnemyInspector();
}
function deleteStage(i) {
  const m = cur(); if (!m || m.stages.length <= 1) { alert("A mission needs at least one wave."); return; }
  if (!confirm(`Delete wave ${i + 1}?`)) return;
  m.stages.splice(i, 1); S.si = Math.max(0, S.si - (i <= S.si ? 1 : 0)); S.ei = -1;
  markDirty(); renderStages(); renderCanvas(); renderEnemyInspector();
}

// ---------------------------------------------------------------------------
// Battlefield canvas
// ---------------------------------------------------------------------------
function fieldToPct(x, y) {
  const f = S.meta.field;
  return { left: ((x - f.xMin) / (f.xMax - f.xMin)) * 100,
           top: ((y - f.yMin) / (f.yMax - f.yMin)) * 100 };
}
function pctToField(px, py) {
  const f = S.meta.field;
  return { x: Math.round(clamp(f.xMin + px * (f.xMax - f.xMin), f.xMin, f.xMax)),
           y: Math.round(clamp(f.yMin + py * (f.yMax - f.yMin), f.yMin, f.yMax)) };
}
function parsePos(p) { const [x, y] = String(p || "0:0").split(":").map(Number); return { x: x || 0, y: y || 0 }; }

function renderCanvas() {
  const bf = $("battlefield");
  bf.querySelectorAll(".token").forEach((t) => t.remove());
  const st = curStage();
  const empty = $("fieldEmpty");
  if (!st || !st.battle_monsters.length) { empty.classList.remove("hidden"); return; }
  empty.classList.add("hidden");
  st.battle_monsters.forEach((mon, idx) => {
    const { x, y } = parsePos(mon.position);
    const { left, top } = fieldToPct(x, y);
    const u = S.unitsById.get(mon.unit_id);
    const t = el("div", `token el-${u ? u.element : 0}` + (idx === S.ei ? " selected" : "") + (st.is_boss ? " boss" : ""));
    t.style.left = left + "%"; t.style.top = top + "%";
    t.innerHTML = `<span class="tname">${escapeHtml(mon.name || "?")}</span><span class="thp">${mon.hp || 0} HP</span>`;
    t.onmousedown = (ev) => startDrag(ev, idx);
    bf.appendChild(t);
  });
}

function startDrag(ev, idx) {
  ev.preventDefault();
  S.si; S.ei = idx; renderCanvas(); renderEnemyInspector();
  const bf = $("battlefield");
  const mon = curStage().battle_monsters[idx];
  let moved = false;
  const move = (e) => {
    const r = bf.getBoundingClientRect();
    const px = clamp((e.clientX - r.left) / r.width, 0, 1);
    const py = clamp((e.clientY - r.top) / r.height, 0, 1);
    const f = pctToField(px, py);
    mon.position = `${f.x}:${f.y}`;
    moved = true;
    renderCanvas(); const posInput = $("efPos"); if (posInput) posInput.value = mon.position;
  };
  const up = () => {
    document.removeEventListener("mousemove", move);
    document.removeEventListener("mouseup", up);
    if (moved) markDirty();
  };
  document.addEventListener("mousemove", move);
  document.addEventListener("mouseup", up);
}

// ---------------------------------------------------------------------------
// Enemy palette + add
// ---------------------------------------------------------------------------
function renderPalette() {
  const box = $("unitPalette"); box.innerHTML = "";
  const q = ($("paletteSearch").value || "").toLowerCase();
  S.units
    .filter((u) => !q || u.name.toLowerCase().includes(q) || String(u.id).includes(q))
    .forEach((u) => {
      const row = el("div", "unit");
      row.innerHTML = `<span class="dot el-${u.element}"></span>
        <span class="uname">${escapeHtml(u.name)}</span>
        <span class="star">${"★".repeat(u.rarity || 0)}</span>
        <span class="uid">#${u.id}</span>`;
      row.title = "Add to current wave";
      row.onclick = () => addEnemy(u);
      box.appendChild(row);
    });
}

function nextSlot(st) {
  const slots = S.meta.slots;
  const used = new Set(st.battle_monsters.map((m) => m.position));
  for (const s of slots) { const p = `${s.x}:${s.y}`; if (!used.has(p)) return p; }
  // fall back to a jittered center
  const f = S.meta.field;
  return `${Math.round((f.xMin + f.xMax) / 2)}:${Math.round((f.yMin + f.yMax) / 2)}`;
}

function addEnemy(u) {
  const st = curStage();
  if (!st) { alert("Create/select a wave first."); return; }
  if (st.battle_monsters.length >= S.meta.limits.maxMonstersPerStage) {
    alert(`A wave can hold at most ${S.meta.limits.maxMonstersPerStage} enemies.`); return;
  }
  st.battle_monsters.push({
    id: u.id, name: u.name, unit_id: u.id, position: nextSlot(st),
    hp: 1000, atk: 300, def: 100,
    ai_id: S.ais.length ? S.ais[0].id : 1, act_min: 1, act_max: 1, wait: 5,
    unit_drop_id: 0, unit_drop_level: 0, unit_drop_type: 1, unit_drop_chance: 0,
    zel_max_drop: 10, zel_drop_count: 5, karma_max_drop: 8, karma_drop_count: 5,
    treasure_chest_chance: 0, treasure_drops: [],
  });
  S.ei = st.battle_monsters.length - 1; markDirty();
  renderStages(); renderCanvas(); renderEnemyInspector();
}

// ---------------------------------------------------------------------------
// Enemy inspector
// ---------------------------------------------------------------------------
function renderEnemyInspector() {
  const insp = $("enemyInspector"); const box = $("enemyFields");
  const st = curStage();
  if (!st || S.ei < 0 || S.ei >= st.battle_monsters.length) { insp.classList.add("hidden"); return; }
  insp.classList.remove("hidden"); box.innerHTML = "";
  const m = st.battle_monsters[S.ei];

  const num = (id, key, label, opts = {}) => field(box, id, label, "number", m[key], (v) => {
    m[key] = clamp(intOr0(v), opts.min ?? 0, opts.max ?? Infinity);
    if (key === "hp") renderCanvas(); markDirty();
  });
  const txt = (id, key, label) => field(box, id, label, "text", m[key], (v) => { m[key] = v; markDirty(); if (key==="name"){renderCanvas();renderStages();} });

  box.append(hdr("Identity"));
  txt("efName", "name", "Name");
  num("efMonId", "id", "Monster ID");
  // unit picker
  const wrap = el("label", "", "Unit (visuals)");
  const sel = el("select");
  S.units.forEach((u) => { const o = el("option"); o.value = u.id;
    o.textContent = `${u.name} (#${u.id})`; if (u.id === m.unit_id) o.selected = true; sel.appendChild(o); });
  sel.onchange = (e) => { m.unit_id = intOr0(e.target.value); renderCanvas(); markDirty(); };
  wrap.appendChild(sel); box.appendChild(wrap);
  field(box, "efPos", "Position (x:y)", "text", m.position, (v) => { m.position = v; renderCanvas(); markDirty(); });

  box.append(hdr("Combat"));
  num("efHp", "hp", "HP"); num("efAtk", "atk", "ATK"); num("efDef", "def", "DEF");
  // AI picker
  const aiWrap = el("label", "", "AI behavior");
  const aiSel = el("select");
  S.ais.forEach((a) => { const o = el("option"); o.value = a.id;
    o.textContent = `${a.name} (#${a.id})`; if (a.id === m.ai_id) o.selected = true; aiSel.appendChild(o); });
  aiSel.onchange = (e) => { m.ai_id = intOr0(e.target.value); markDirty(); };
  aiWrap.appendChild(aiSel); box.appendChild(aiWrap);
  num("efActMin", "act_min", "Acts min", { min: 1 }); num("efActMax", "act_max", "Acts max", { min: 1 });
  num("efWait", "wait", "Wait");

  box.append(hdr("Unit drop"));
  num("efDropId", "unit_drop_id", "Unit ID (0=none)");
  num("efDropLvl", "unit_drop_level", "Level");
  typeSelect(box, "Type", m.unit_drop_type, S.meta.unitTypes, (v) => { m.unit_drop_type = v; markDirty(); });
  num("efDropChance", "unit_drop_chance", "Chance %", { max: 100 });

  box.append(hdr("Zel / Karma drop"));
  num("efZelMax", "zel_max_drop", "Zel max"); num("efZelCnt", "zel_drop_count", "Zel rolls");
  num("efKarMax", "karma_max_drop", "Karma max"); num("efKarCnt", "karma_drop_count", "Karma rolls");

  box.append(hdr("Treasure chest"));
  num("efTreasChance", "treasure_chest_chance", "Chest %", { max: 100 });
  const tnote = el("div", "full muted small", `${m.treasure_drops.length} reward(s) — treasure table editor in a later phase`);
  box.appendChild(tnote);
}

function typeSelect(box, label, val, options, onchange) {
  const wrap = el("label", "", label);
  const sel = el("select");
  for (const [k, name] of Object.entries(options)) {
    const o = el("option"); o.value = k; o.textContent = `${k} · ${name}`;
    if (Number(k) === Number(val)) o.selected = true; sel.appendChild(o);
  }
  sel.onchange = (e) => onchange(intOr0(e.target.value));
  wrap.appendChild(sel); box.appendChild(wrap);
}

function field(box, id, label, type, value, oninput) {
  const wrap = el("label"); wrap.textContent = label;
  const inp = el("input"); inp.id = id; inp.type = type; inp.value = value ?? "";
  inp.addEventListener("input", (e) => oninput(e.target.value));
  wrap.appendChild(inp); box.appendChild(wrap); return inp;
}
function hdr(t) { return el("h4", null, t); }

// Inline label+input (no id) for dynamically-rendered rows.
function inlineInput(parent, label, type, value, oninput) {
  const wrap = el("label"); wrap.textContent = label;
  const inp = el("input"); inp.type = type; inp.value = value ?? "";
  inp.addEventListener("input", (e) => oninput(e.target.value));
  wrap.appendChild(inp); parent.appendChild(wrap); return inp;
}
const numInline = (p, l, v, f) => inlineInput(p, l, "number", v, f);
const txtInline = (p, l, v, f) => inlineInput(p, l, "text", v, f);

// Text input with a datalist of suggestions — pick a known value or type a custom one.
function comboField(parent, label, value, suggestions, oninput) {
  const wrap = el("label"); wrap.textContent = label;
  const inp = el("input"); inp.type = "text"; inp.value = value ?? "";
  const listId = "dl_" + Math.random().toString(36).slice(2);
  inp.setAttribute("list", listId);
  const dl = el("datalist"); dl.id = listId;
  (suggestions || []).forEach((s) => { const o = el("option"); o.value = s; dl.appendChild(o); });
  inp.addEventListener("input", (e) => oninput(e.target.value));
  wrap.append(inp, dl); parent.appendChild(wrap); return inp;
}

// ---------------------------------------------------------------------------
// Validation (DATA_MODEL.md §8)
// ---------------------------------------------------------------------------
function validate() {
  const issues = [];
  const m = cur(); if (!m) return issues;
  const ids = S.missions.map((x) => x.id);
  if (ids.filter((x) => x === m.id).length > 1) issues.push(["err", `Mission id ${m.id} is used by another mission.`]);
  if (!m.stages.length) issues.push(["err", "Mission has no waves."]);
  m.stages.forEach((st, si) => {
    const n = st.battle_monsters.length;
    if (n < 1) issues.push(["err", `Wave ${si + 1} has no enemies.`]);
    if (n > S.meta.limits.maxMonstersPerStage) issues.push(["err", `Wave ${si + 1} exceeds ${S.meta.limits.maxMonstersPerStage} enemies.`]);
    st.battle_monsters.forEach((mon, mi) => {
      const where = `W${si + 1}·${mon.name || "enemy"}`;
      if (!S.unitsById.has(mon.unit_id)) issues.push(["err", `${where}: unit_id ${mon.unit_id} is not in the unit palette.`]);
      if (!S.ais.some((a) => a.id === mon.ai_id)) issues.push(["err", `${where}: ai_id ${mon.ai_id} not in ai.json.`]);
      if (!/^\d+:\d+$/.test(String(mon.position))) issues.push(["warn", `${where}: position "${mon.position}" isn't x:y.`]);
      [["unit_drop_chance"], ["treasure_chest_chance"]].forEach(([k]) => {
        if ((mon[k] || 0) > 100) issues.push(["warn", `${where}: ${k} > 100.`]);
      });
    });
  });
  return issues;
}
function renderValidation() {
  const ul = $("validationList"); ul.innerHTML = "";
  const issues = validate();
  if (!issues.length) { ul.appendChild(el("li", "ok", "✓ No problems.")); return; }
  issues.forEach(([kind, msg]) => ul.appendChild(el("li", kind, escapeHtml(msg))));
}

// ---------------------------------------------------------------------------
// Save
// ---------------------------------------------------------------------------
function renderSaveStatus() {
  const s = $("saveStatus");
  s.textContent = S.dirty ? "● Unsaved changes" : "Saved";
  s.className = "save-status " + (S.dirty ? "dirty" : "saved");
}
async function save() {
  const errs = validate().filter(([k]) => k === "err");
  if (errs.length && !confirm(`There are ${errs.length} error(s). Save anyway?`)) return;
  try {
    await api.post("/api/missions", S.missions);
    S.dirty = false; renderSaveStatus();
    flash("Saved to mission.json — restart the server (or use Test, later) to load it.");
  } catch (e) { alert("Save failed:\n" + e.message); }
}
function flash(msg) {
  const s = $("saveStatus"); s.textContent = "✓ " + msg; s.className = "save-status saved";
  setTimeout(renderSaveStatus, 4000);
}

async function testInGame() {
  const m = cur(); if (!m) return;
  const errs = validate().filter(([k]) => k === "err");
  if (errs.length && !confirm(`There are ${errs.length} error(s). Test anyway?`)) return;
  try {
    const r = await api.post("/api/test", S.missions);
    S.dirty = false; renderSaveStatus();
    const rl = r.reload || {};
    if (rl.reached && rl.result && rl.result.ok) {
      alert(`Saved and hot-reloaded the game server (${rl.result.missions} missions, ${rl.result.ais} AIs loaded).\n\n` +
            `Now play mission #${m.id} in the client. If it isn't on the map yet, run "unlockmap" in the server debug CLI first.`);
      flash("Reloaded into the running server.");
    } else {
      alert(`Saved to mission.json, but the game server wasn't reloaded:\n\n${rl.error || "unknown"}\n\n` +
            `Start the game server (it should listen on 127.0.0.1:9960), then click Test again — or just restart it to load the mission.`);
      flash("Saved (game server not reached).");
    }
  } catch (e) { alert("Test failed:\n" + e.message); }
}

// ---------------------------------------------------------------------------
// AI builder (Phase 3) — edits ai.json; enemies reference an AI by ai_id.
// ---------------------------------------------------------------------------
const curAi = () => (S.aiIndex >= 0 && S.aiIndex < S.ais.length ? S.ais[S.aiIndex] : null);

function newAiAction(priority) {
  return {
    priority: priority || 1, percent: 100, act_target: 2, search_term: "random",
    self_conditions: [], party_conditions: [],
    action: { type: "attack", flag_changes: [], unknown_bool: true, unknown_int_1: 0, unknown_int_2: 0 },
  };
}

function openAiModal() {
  $("aiModal").classList.remove("hidden");
  if (S.aiIndex < 0 && S.ais.length) S.aiIndex = 0;
  renderAiSaveStatus(); renderAiList(); renderAiEditor();
}
function closeAiModal() {
  if (S.aiDirty && !confirm("Close without saving AI changes?")) return;
  $("aiModal").classList.add("hidden");
}
function markAiDirty() { S.aiDirty = true; renderAiSaveStatus(); }
function renderAiSaveStatus() {
  const s = $("aiSaveStatus");
  s.textContent = S.aiDirty ? "● Unsaved" : "Saved";
  s.className = "save-status " + (S.aiDirty ? "dirty" : "saved");
}

function renderAiList() {
  const ul = $("aiList"); ul.innerHTML = "";
  S.ais.forEach((a, i) => {
    const li = el("li", i === S.aiIndex ? "active" : "");
    li.innerHTML = `<span class="mname">${escapeHtml(a.name || "(unnamed)")}</span>
                    <span class="mid"> #${a.id} · ${(a.actions || []).length} action(s)</span>`;
    li.onclick = () => { S.aiIndex = i; renderAiList(); renderAiEditor(); };
    ul.appendChild(li);
  });
}

function newAi() {
  const nextId = S.ais.reduce((mx, a) => Math.max(mx, a.id || 0), 0) + 1;
  S.ais.push({ id: nextId, name: "New AI", actions: [newAiAction(1)] });
  S.aiIndex = S.ais.length - 1; markAiDirty(); renderAiList(); renderAiEditor();
}
function deleteAi() {
  const a = curAi(); if (!a) return;
  const used = usedByEnemies(a.id);
  const warn = used ? `\n\nWarning: ${used} enemy(ies) reference this AI and will break.` : "";
  if (!confirm(`Delete AI #${a.id} "${a.name}"?${warn}`)) return;
  S.ais.splice(S.aiIndex, 1);
  S.aiIndex = Math.min(S.aiIndex, S.ais.length - 1);
  markAiDirty(); renderAiList(); renderAiEditor();
}
function usedByEnemies(aiId) {
  let n = 0;
  S.missions.forEach((m) => (m.stages || []).forEach((st) =>
    st.battle_monsters.forEach((mon) => { if (mon.ai_id === aiId) n++; })));
  return n;
}

function renderAiEditor() {
  const a = curAi();
  $("aiEditor").classList.toggle("hidden", !a);
  $("aiNone").classList.toggle("hidden", !!a);
  if (!a) return;
  $("aiName").value = a.name || ""; $("aiId").value = a.id;
  $("aiName").oninput = (e) => { a.name = e.target.value; renderAiList(); markAiDirty(); };
  $("aiId").oninput = (e) => { a.id = intOr0(e.target.value); renderAiList(); markAiDirty(); };
  renderAiActions();
}
function renderAiActions() {
  const a = curAi(); const box = $("aiActions"); box.innerHTML = "";
  (a.actions || []).forEach((act, i) => box.appendChild(aiActionCard(a, act, i)));
}
function aiActionCard(a, act, idx) {
  const card = el("div", "ai-action");
  const top = el("div", "ai-action-top");
  numInline(top, "Priority", act.priority, (v) => { act.priority = intOr0(v); markAiDirty(); });
  numInline(top, "Chance %", act.percent, (v) => { act.percent = clamp(intOr0(v), 0, 100); markAiDirty(); });
  comboField(top, "Action", act.action.type, S.meta.aiVocab.actionTypes, (v) => { act.action.type = v; markAiDirty(); });
  comboField(top, "Target search", act.search_term, S.meta.aiVocab.searchTerms, (v) => { act.search_term = v; markAiDirty(); });
  numInline(top, "Act target", act.act_target, (v) => { act.act_target = intOr0(v); markAiDirty(); });
  const del = el("button", "chip-btn", "✕"); del.title = "Delete action";
  del.onclick = () => { a.actions.splice(idx, 1); markAiDirty(); renderAiActions(); };
  top.appendChild(del);
  card.appendChild(top);
  card.appendChild(condGroup("Self conditions (about the enemy)", act.self_conditions, "self"));
  card.appendChild(condGroup("Party conditions (about your team)", act.party_conditions, "party"));
  return card;
}
function condGroup(title, list, kind) {
  const g = el("div", "ai-cond-group");
  const h = el("h5", null, title);
  const add = el("button", "chip-btn", "+ add");
  add.onclick = () => {
    if (kind === "self") list.push({ type: "hp_pr_under", parameter: 50 });
    else list.push({ target_id: 0, target_parameter: "non", type: "non", parameters: "non" });
    markAiDirty(); renderAiActions();
  };
  h.appendChild(add); g.appendChild(h);
  list.forEach((c, ci) => {
    const row = el("div", "ai-cond-row");
    if (kind === "self") {
      comboField(row, "Type", c.type, S.meta.aiVocab.selfConditionTypes, (v) => { c.type = v; markAiDirty(); });
      numInline(row, "Param", c.parameter, (v) => { c.parameter = intOr0(v); markAiDirty(); });
    } else {
      numInline(row, "Target id", c.target_id, (v) => { c.target_id = intOr0(v); markAiDirty(); });
      txtInline(row, "Target param", c.target_parameter, (v) => { c.target_parameter = v; markAiDirty(); });
      txtInline(row, "Type", c.type, (v) => { c.type = v; markAiDirty(); });
      txtInline(row, "Params", c.parameters, (v) => { c.parameters = v; markAiDirty(); });
    }
    const del = el("button", "chip-btn", "✕");
    del.onclick = () => { list.splice(ci, 1); markAiDirty(); renderAiActions(); };
    row.appendChild(del); g.appendChild(row);
  });
  return g;
}
async function saveAis() {
  try {
    await api.post("/api/ais", S.ais);
    S.aiDirty = false; renderAiSaveStatus();
    if (cur()) renderEnemyInspector();  // AI names may have changed
    const s = $("aiSaveStatus"); s.textContent = "✓ Saved — Test in game to load.";
    s.className = "save-status saved";
  } catch (e) { alert("Save AIs failed:\n" + e.message); }
}

// ---------------------------------------------------------------------------
// Utils + wiring
// ---------------------------------------------------------------------------
function intOr0(v) { const n = parseInt(v, 10); return Number.isFinite(n) ? n : 0; }
function clamp(v, lo, hi) { return Math.min(Math.max(v, lo), hi); }
function escapeHtml(s) { return String(s).replace(/[&<>"]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c])); }

function wire() {
  $("btnNewMission").onclick = newMission;
  $("btnNewMission2").onclick = newMission;
  $("btnDupMission").onclick = duplicateMission;
  $("btnDeleteMission").onclick = deleteMission;
  $("btnAddStage").onclick = addStage;
  $("btnSave").onclick = save;
  $("btnTest").onclick = testInGame;
  $("btnReload").onclick = () => { if (!S.dirty || confirm("Discard unsaved changes and reload?")) boot(); };
  $("btnDeleteEnemy").onclick = () => {
    const st = curStage(); if (!st || S.ei < 0) return;
    st.battle_monsters.splice(S.ei, 1); S.ei = -1; markDirty();
    renderStages(); renderCanvas(); renderEnemyInspector();
  };
  $("paletteSearch").addEventListener("input", renderPalette);
  // AI builder
  $("btnManageAi").onclick = openAiModal;
  $("btnCloseAi").onclick = closeAiModal;
  $("btnNewAi").onclick = newAi;
  $("btnDeleteAi").onclick = deleteAi;
  $("btnSaveAis").onclick = saveAis;
  $("btnAddAiAction").onclick = () => {
    const a = curAi(); if (!a) return;
    a.actions.push(newAiAction((a.actions.length || 0) + 1));
    markAiDirty(); renderAiActions();
  };
  bindHeader();
  window.addEventListener("beforeunload", (e) => { if (S.dirty) { e.preventDefault(); e.returnValue = ""; } });
}

wire();
boot();
