### TODO(arves): THIS SCRIPT MUST BE REMOVED !!!

import subprocess
import glob
import os
from pathlib import Path

targets = [
    ("packet-generator/assets/all.kdl", "gimuserver/packets"),
    ("packet-generator/assets/archive.kdl", "gimuserver/archive"),
]

for kdl_file, target_dir in targets:
    kdl_file_real = os.path.relpath(kdl_file, "packet-generator")
    os.makedirs(target_dir, exist_ok=True)

    try:
        subprocess.run([
            "cargo", "run", "--",
            "generate", "--cxx", "--glaze",
            "-i", kdl_file_real,
            "-o", f"../{target_dir}"
        ], cwd="packet-generator", check=True)
    except subprocess.CalledProcessError:
        pass
