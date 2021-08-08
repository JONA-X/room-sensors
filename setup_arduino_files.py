import os
import shutil

source_folders = ["coffee-machine-esp", "sensor-box-esp"]
general_code_folders = ["esp-general"]
arduino_upload_folder_prefix = "arduino-upload"



def copy_from_dir1_to_dir2(dir1_name, dir2_name):
    src_files = os.listdir(dir1_name)
    for file_name in src_files:
        full_file_name = os.path.join(dir1_name, file_name)
        if os.path.isfile(full_file_name):
            dest = os.path.join(dir2_name, file_name)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            shutil.copy(full_file_name, dest)
    print("Copied everything from " + dir1_name + " to folder " + dir2_name)


for folder in source_folders:
    dest_folder = os.path.join(arduino_upload_folder_prefix, folder)
    copy_from_dir1_to_dir2(folder, dest_folder)
    for general_folder in general_code_folders:
        copy_from_dir1_to_dir2(general_folder, dest_folder)

print("All copying done.")