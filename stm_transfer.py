import shutil
import sys
import os
import re

DIRS_TO_COPY = ["Application", "HighLevelDrivers", "LowLevelDrivers/inc", "LowLevelDrivers/STM_src"]
DRIVER_FILENAMES = ["adc", "gpio", "i2c", "pwm", "uart", "sleep"]

stm_project_path = sys.argv[1]
if not os.path.exists(stm_project_path):
    print(f"Error: The project path '{stm_project_path}' does not exist.")
    sys.exit(1)


# mass copy directories
for dir_name in DIRS_TO_COPY:
    src_dir = dir_name
    dest_dir = os.path.join(stm_project_path, dir_name)

    if not os.path.exists(src_dir):
        print(f"Warning: Source directory '{src_dir}' does not exist. Skipping.")
        continue

    if os.path.exists(dest_dir):
        shutil.rmtree(dest_dir)
    shutil.copytree(src_dir, dest_dir)


# rename files in LowLevelDrivers/STM_src and LowLevelDrivers/inc/ to avoid name conflicts with STM auto-generated files
dir_path = os.path.join(stm_project_path, "LowLevelDrivers", "STM_src")
for filename in os.listdir(dir_path):
    os.rename(
        os.path.join(dir_path, filename),
        os.path.join(dir_path, filename[:-2] + "_lld.c")
    )
dir_path = os.path.join(stm_project_path, "LowLevelDrivers", "inc")
for filename in os.listdir(dir_path):
    os.rename(
        os.path.join(dir_path, filename),
        os.path.join(dir_path, filename[:-2] + "_lld.h")
    )


# copy hardware init file
shutil.copy(os.path.join("STM_init", "hardware_init.c"), 
            os.path.join(stm_project_path, "Core", "Src", "hardware_init.c"))
shutil.copy(os.path.join("STM_init", "hardware_init.h"), 
            os.path.join(stm_project_path, "Core", "Inc", "hardware_init.h"))


# update includes for new name of LowLevelDrivers files
def replace_in_file(file_path):
    pattern = r'#include\s+"(?:.*/)?(BASES).h"'.replace("BASES", "|".join(DRIVER_FILENAMES))

    with open(file_path, 'r', encoding="utf-8") as file:
        content = file.read()

    content = re.sub(pattern, lambda m: m.group(0)[0:-3] + "_lld.h\"", content)

    with open(file_path, 'w', encoding="utf-8") as file:
        file.write(content)

for directory in DIRS_TO_COPY:
    dir_path = os.path.join(stm_project_path, directory)
    for root, _, files in os.walk(dir_path):
        for file in files:
            if file.endswith(('.c', '.h')):
                replace_in_file(os.path.join(root, file))
replace_in_file(os.path.join(stm_project_path, "Core", "Src", "hardware_init.c"))
replace_in_file(os.path.join(stm_project_path, "Core", "Inc", "hardware_init.h"))


# copy over the IOC file, but DON'T change ProjectManager.ProjectFileName or ProjectManager.ProjectName
src_ioc_path = os.path.join("STM_init", "McDapBots_STM.ioc")
dest_ioc_path = os.path.join(stm_project_path, [f for f in os.listdir(stm_project_path) if f.endswith('.ioc')][0])

with open(dest_ioc_path, 'r', encoding="utf-8") as file:
    content = file.read()
ProjectManager_ProjectFileName = re.search(r'ProjectManager\.ProjectFileName=(.+)', content).group(1)
ProjectManager_ProjectName = re.search(r'ProjectManager\.ProjectName=(.+)', content).group(1)

shutil.copy2(src_ioc_path, dest_ioc_path)
with open(dest_ioc_path, 'r', encoding="utf-8") as file:
    content = file.read()
content = re.sub(r'ProjectManager\.ProjectFileName=.+', f'ProjectManager.ProjectFileName={ProjectManager_ProjectFileName}', content)
content = re.sub(r'ProjectManager\.ProjectName=.+', f'ProjectManager.ProjectName={ProjectManager_ProjectName}', content)
with open(dest_ioc_path, 'w', encoding="utf-8") as file:
    file.write(content)


# end
print("Done")
print("🚨🚨 Remember to add 'Application', 'HighLevelDrivers', and 'LowLevelDrivers' to the STM32CubeIDE project include paths and source locations. 🚨🚨")