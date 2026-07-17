"""
One-shot porter: copies high-value F_*_MST.json files from the decoded
reference directory to deploy/system/ with proper wrapper keys.

Run once.  Output: deploy/system/<name>_mst.json files, each shaped
{ "<wrapper_key>": [ ...rows ] }.

Wrapper keys are sourced from
`BraveFrontier IDA readParam and Handler exports and scripts/response_mappings.json`
under each MstResponse class's response_key.  See handbook §3.3.1 and
§8.36 for the discovery workflow.

Idempotent: skips destination files that already exist.  Re-run with
`force = True` to overwrite (not recommended; will mask future edits).
"""
import json
import os
import glob

SRC_DIR = r'C:\Users\Evan\BF\21900\mst-decoded\2-mst-decoded'
DST_DIR = r'C:\Users\Evan\BF\BF-WorkingDirRust\deploy\system'

# (source_glob, wrapper_key, dest_filename, identity_label, response_class)
PORTS = [
    # Skill cluster
    ('F_SKILL_MST_*.json',                   '8aiBoHg5', 'skill_mst.json',
        'F_SKILL_MST (9 parts merged)',      'UnitSkillMstResponse'),
    ('F_SKILL_LEVEL_MST_*.json',             '3ktU0D2e', 'skill_level_mst.json',
        'F_SKILL_LEVEL_MST (4 parts merged)','UnitSkillLevelMstResponse'),
    ('F_LEADER_SKILL_MST_Ver*.json',         '2bGtP7vV', 'leader_skill_mst.json',
        'F_LEADER_SKILL_MST',                'LeaderSkillMstResponse'),
    # Item / Shop / Medal
    ('F_ITEM_MST_Ver*.json',                 '2C7LDzYk', 'item_mst.json',
        'F_ITEM_MST',                        'ItemMstResponse'),
    ('F_SHOP_ITEM_MST_Ver*.json',            '5DXlaudE', 'shop_item_mst.json',
        'F_SHOP_ITEM_MST',                   'ShopItemMstResponse'),
    ('F_MEDAL_MST_Ver*.json',                'GHUsyi76', 'medal_mst.json',
        'F_MEDAL_MST',                       'MedalMstResponse'),
    # Unit family
    ('F_UNIT_CGS_MST_Ver*.json',             '07bx6EXN', 'unit_cgs_mst.json',
        'F_UNIT_CGS_MST',                    'UnitCgsMstResponse'),
    ('F_UNIT_COMMENT_MST_Ver*.json',         'gkVH5p8r', 'unit_comment_mst.json',
        'F_UNIT_COMMENT_MST',                'UnitCommentMstResponse'),
    ('F_UNIT_EP3_MST_Ver*.json',             'XFD1gtfo', 'unit_ep3_mst.json',
        'F_UNIT_EP3_MST',                    'UnitEp3MstResponse'),
    ('F_UNIT_EVO_MST_Ver*.json',             'yY3Mvp51', 'unit_evo_mst.json',
        'F_UNIT_EVO_MST',                    'UnitEvoMstResponse'),
    ('F_UNIT_EVO_OMNI_MST_Ver*.json',        'K4d9sl4s', 'unit_evo_omni_mst.json',
        'F_UNIT_EVO_OMNI_MST',               'UnitOmniEvoMstResponse'),
    ('F_UNIT_EVO_OMNI_RECIPE_MST_Ver*.json', 'Odks40sl', 'unit_evo_omni_recipe_mst.json',
        'F_UNIT_EVO_OMNI_RECIPE_MST',        'UnitOmniEvoRecipeMstResponse'),
    ('F_UNIT_EVO_OMNI_TYPE_MST_Ver*.json',   '34d0Slkf', 'unit_evo_omni_type_mst.json',
        'F_UNIT_EVO_OMNI_TYPE_MST',          'UnitOmniEvoCostMstResponse'),
    ('F_UNIT_EXT_MST_Ver*.json',             '0H1hV6iD', 'unit_ext_mst.json',
        'F_UNIT_EXT_MST',                    'UnitExtMstResponse'),
    ('F_UNIT_FE_CATEGORY_MST_Ver*.json',     'kvSPgNAk', 'unit_fe_category_mst.json',
        'F_UNIT_FE_CATEGORY_MST',            'UnitFEGategoryMstResponse'),
    ('F_UNIT_FE_SKILL_MST_Ver*.json',        'kXes8fSi', 'unit_fe_skill_mst.json',
        'F_UNIT_FE_SKILL_MST',               'UnitFESkillMstResponse'),
    ('F_UNIT_TYPE_MST_Ver*.json',            'LBY10TPU', 'unit_type_mst.json',
        'F_UNIT_TYPE_MST',                   'UnitTypeMstResponse'),
    # Area / Mission / Gate / Dungeon
    ('F_AREA_MST_Ver*.json',                 '3SG2wX0R', 'area_mst.json',
        'F_AREA_MST',                        'AreaMstResponse'),
    ('F_MISSION_EP3_MST_Ver*.json',          'vE0Qdg33', 'mission_ep3_mst.json',
        'F_MISSION_EP3_MST',                 'MissionEp3MstResponse'),
    ('F_GATE_MST_Ver*.json',                 'uYHZWC73', 'gate_mst.json',
        'F_GATE_MST',                        'GateMstResponse'),
    ('F_HELP_DETAIL_MST_Ver*.json',          'a3IgzZ0q', 'help_detail_mst.json',
        'F_HELP_DETAIL_MST',                 'HelpDetailMstResponse'),
    ('F_DUNGEON_MST_Ver*.json',              '01pzrTCm', 'dungeon_mst.json',
        'F_DUNGEON_MST',                     'DungeonMstResponse'),
    # PvP — identified via UNKNOWN audit pass (handbook §8.36)
    ('F_UNKNOWN_gyY4TCm8_MST_Ver*.json',     'KFJBwYbR', 'pvp_fixed_setting_mst.json',
        'F_PVP_FIXED_SETTING_MST (was F_UNKNOWN_gyY4TCm8)',
        'PvpFixedSettingMstResponse'),
]


def strip_annotation_comments(text):
    """Strip // line comments outside strings — used by annotated UNKNOWN files."""
    out_lines = []
    for line in text.splitlines():
        in_str = False
        escape = False
        cut = None
        for i, c in enumerate(line):
            if escape:
                escape = False
                continue
            if c == '\\':
                escape = True
                continue
            if c == '"':
                in_str = not in_str
            if not in_str and c == '/' and i + 1 < len(line) and line[i+1] == '/':
                cut = i
                break
        out_lines.append(line[:cut] if cut is not None else line)
    return '\n'.join(out_lines)


def load_mst_array(path):
    with open(path, 'r', encoding='utf-8') as f:
        text = f.read()
    return json.loads(strip_annotation_comments(text))


def main():
    n_ok, n_skip, n_fail = 0, 0, 0
    for pattern, wrap_key, dest, label, cls in PORTS:
        matches = sorted(glob.glob(os.path.join(SRC_DIR, pattern)))
        if not matches:
            print("SKIP  %s: no source match for %s" % (dest, pattern))
            n_skip += 1
            continue
        dest_path = os.path.join(DST_DIR, dest)
        if os.path.exists(dest_path):
            print("SKIP  %s: already exists in deploy/system" % dest)
            n_skip += 1
            continue

        merged = []
        ok = True
        for m in matches:
            try:
                arr = load_mst_array(m)
            except Exception as e:
                print("FAIL  %s: parse error in %s: %s" % (
                    dest, os.path.basename(m), e))
                ok = False
                break
            if not isinstance(arr, list):
                print("FAIL  %s: %s is not an array (type=%s)" % (
                    dest, os.path.basename(m), type(arr).__name__))
                ok = False
                break
            merged.extend(arr)
        if not ok:
            n_fail += 1
            continue

        with open(dest_path, 'w', encoding='utf-8') as f:
            f.write('{\n')
            f.write('  "%s": [\n' % wrap_key)
            for i, row in enumerate(merged):
                line = '    ' + json.dumps(row, ensure_ascii=False,
                                            sort_keys=False)
                if i < len(merged) - 1:
                    line += ','
                f.write(line + '\n')
            f.write('  ]\n')
            f.write('}\n')
        n_ok += 1
        parts_note = " (%d parts)" % len(matches) if len(matches) > 1 else ""
        print("OK    %s: %5d rows from %s%s -> wrapper %s" % (
            dest, len(merged), os.path.basename(matches[0]),
            parts_note, wrap_key))

    print()
    print("Ported: %d  Skipped: %d  Failed: %d" % (n_ok, n_skip, n_fail))


if __name__ == '__main__':
    main()
