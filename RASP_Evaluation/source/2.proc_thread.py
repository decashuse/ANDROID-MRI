import os
import zipfile
import yara
import threading
import queue
import zlib

apk_directory       = '...'
unzip_directory     = '...'
yara_rule_directory = '...'

print_lock = threading.Lock()

def unzip_apks(source_directory, destination_directory):
    for idx, filename in enumerate(os.listdir(source_directory), start=1):
        filepath = os.path.join(source_directory, filename)
        if os.path.isfile(filepath):
            try:
                foldername = os.path.splitext(filename)[0]
                folderpath = os.path.join(destination_directory, foldername)
                os.makedirs(folderpath, exist_ok=True)
            except OSError as e: 
                print(f"error {idx}'st Unzipping '{folderpath}'...")
                pass
            
		
            try: 
                if zipfile.is_zipfile(filepath):
                    with zipfile.ZipFile(filepath, 'r') as zip_ref:
                        zip_ref.extractall(folderpath)
                    print(f"{idx}'st Unzipping '{filename}'...")
            except (OSError, zlib.error, zipfile.BadZipFile) as e: 
                print(f"error {idx}'st Unzipping '{filename}'...")
                pass



def compile_yara_rules_in_directory(directory):
    print(f"Compiling YARA rules from {directory} and its subdirectories...")
    yara_rules = []
    for root, dirs, files in os.walk(directory):
        for filename in files:
            filepath = os.path.join(root, filename)
            try:
                compiled_rule = yara.compile(filepath)
                yara_rules.append(compiled_rule)
                print(f"Compiled Success YARA rule from {filepath}")
            except yara.Error as e:
                print(f"Error compiling YARA rule from {filepath}: {e}")
    return yara_rules

def compile_yara_rules_in_directory_print(directory):
    print(f"Compiling YARA rules from {directory} and its subdirectories...")
    yara_rules = []
    for root, dirs, files in os.walk(directory):
        for filename in files:
            filepath = os.path.join(root, filename)
            try:
                compiled_rules = yara.compile(filepath)
                for rule in compiled_rules:
                    tags = ', '.join(rule.tags)
                    print(tags, rule.identifier)
                    yara_rules.append(rule)
            except yara.Error as e:
                print(f"Error compiling YARA rule from {filepath}: {e}")
    return yara_rules

def process_file(file_path, compiled_rules):
    try:
        matched_rules = []
        for rule in compiled_rules:
            matches = rule.match(file_path)
            if matches:
                matched_rules.extend(matches)
        if matched_rules:
            file_path_sliced = file_path[file_path.find("...") + 5:]

            with print_lock:
                print(f"File: {file_path_sliced}", end='')
                for match in matched_rules:
                    print(f" {match.tags} ", end='')
                print()
    except Exception as e:
        print(f"Error processing file {file_path}: {e}")

def worker(q, compiled_rules):
    while True:
        item = q.get()
        if item is None:
            break
        process_file(item, compiled_rules)
        q.task_done()

def apply_yara_rules_to_directory(directory, compiled_rules):
    print(f"Applying YARA rules to files in {directory}...")
    q = queue.Queue()
    threads = []
    for _ in range(16):  
        thread = threading.Thread(target=worker, args=(q, compiled_rules))
        thread.start()
        threads.append(thread)

    try:
        dir_count = 0
        for root, dirs, files in os.walk(directory):
            if root.startswith(directory) and os.path.dirname(root) == directory:
                dir_count += 1
            for filename in files:
                file_path = os.path.join(root, filename)
                q.put(file_path)

        q.join()  

    finally:
        for _ in range(len(threads)):
            q.put(None)
        for thread in threads:
            thread.join()


# YARA rule compile
compiled_rules = compile_yara_rules_in_directory(yara_rule_directory)

# YARA rule matching
apply_yara_rules_to_directory(unzip_directory, compiled_rules)

