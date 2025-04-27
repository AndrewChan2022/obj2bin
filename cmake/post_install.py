import os
import platform
import subprocess

current_path = os.getenv('TESTPATH') or ""
if platform.system() == 'Windows':
    current_path = current_path.replace("/", "\\")
print("current_path:", current_path)

new_directory = "C:/Program Files/obj2bin/bin"
if platform.system() == 'Windows':
    new_directory = new_directory.replace("/", "\\")

separator = ';' if platform.system() == 'Windows' else ":"
if current_path == "":
    separator = ""
if current_path.endswith(";") or current_path.endswith(":"):
    separator = ""

if new_directory not in current_path:
    new_path = current_path + separator + new_directory
else:
    new_path = current_path

os.environ['TESTPATH'] = new_path

# os.environ['TESTPATH'] = "C:/Program Files/Notepad++"

if platform.system() == 'Windows':
    # subprocess.run(f'setx TESTPATH "{new_path}"', shell=True)
    subprocess.run(f'setx TESTPATH "{new_path}"', shell=True)
else:
    ...
    # shell_config_file = os.path.expanduser("~/.bashrc")
    # with open(shell_config_file, 'a') as file:
    #     file.write(f'\nexport TESTPATH="$TESTPATH:{new_path}"\n')

print("new_path", os.getenv('TESTPATH'))
