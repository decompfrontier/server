### TODO(arves): THIS SCRIPT MUST BE REMOVED !!!

import subprocess
import glob
import os
from pathlib import Path

base_dir = "packet-generator/assets"
output_base = "gimuserver/packets"
kdl_file = "packet-generator/assets/all.kdl"

p_kdl_file = Path(kdl_file)
kdl_dir = p_kdl_file.parent
relative_dir = os.path.relpath(kdl_dir, base_dir)
kdl_file_real = os.path.relpath(kdl_file, "packet-generator")
target_dir = os.path.join(output_base, relative_dir)

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
