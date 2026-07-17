# Brave Frontier — Mission Editor

A local, no-install tool for the community to **recreate Grand Gaia mission data**
that was lost when the servers went dark. You compose waves, place enemies from
the game's unit pool, set their stats/AI/drops, and save — no programming needed.

The tool reads and writes the server's curated archive (`deploy/archive/mission.json`),
which the game server loads at boot. See **[DATA_MODEL.md](DATA_MODEL.md)** for what
every field means.

## Run it

You only need **Python 3** (already required by the project's other tools) and a
web browser. No `pip install`, no Node, no build step.

```bash
python tools/mission-editor/server.py
```

Then open **http://localhost:8777** in your browser.

- To point at a different archive folder: set `BF_ARCHIVE_DIR`.
- To use a different port: set `BF_EDITOR_PORT`.

## What you can do (Phase 1)

- Browse / create / duplicate / delete missions
- Edit mission info (name, zel/karma/exp rewards, energy cost) — "Defaults from MST"
  fills rewards from the original mission data for that ID
- Build **waves** (stages), mark the boss wave, set the first-attack chance
- Add enemies from the **palette**, **drag them on the battlefield** to position them,
  and set HP/ATK/DEF, AI, actions-per-turn, and drops (unit / zel / karma)
- Live **validation** flags problems (missing units/AI, too many enemies, bad ids)
- **Save** writes `mission.json` (a timestamped backup is kept in `backups/`)

After saving, restart the game server to load the new mission (one-click "Test in
game" is a later phase). Enemies are limited to the units in `deploy/archive/unit.json`
(the palette); growing that pool and a visual AI builder are later phases.

## Notes

- Backups of `mission.json` are written to `tools/mission-editor/backups/` on every save.
- Phase roadmap and open data questions live in [DATA_MODEL.md](DATA_MODEL.md) §9.
