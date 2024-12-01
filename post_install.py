import os
import platform
import subprocess

current_path = os.getenv('TESTPATH') or ""

print("current_path:", current_path)

new_directory = "C:/Program Files/obj2bin/bin"

separator = ';' if platform.system() == 'Windows' else ":"
separator = "" if current_path == "" else separator

new_path = current_path + separator + new_directory

os.environ['TESTPATH'] = new_path

# os.environ['TESTPATH'] = "C:/Program Files/Notepad++"

# if platform.system() == 'Windows':
#     subprocess.run(f'setx PATH "%PATH%;{new_path}"', shell=True)
# else:
#     shell_config_file = os.path.expanduser("~/.bashrc")
#     with open(shell_config_file, 'a') as file:
#         file.write(f'\nexport PATH="$PATH:{new_path}"\n')

print("new_path", os.getenv('PATH'))
