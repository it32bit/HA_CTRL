import os
import re
import sys

def remove_comments(text):
    # Remove block comments /* ... */
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    # Remove line comments // ...
    text = re.sub(r'//.*', '', text)
    # Remove CMake-style comments # ...
    text = re.sub(r'#.*', '', text)
    # Remove empty lines and trailing spaces
    lines = [line.rstrip() for line in text.splitlines() if line.strip()]
    return '\n'.join(lines)

def process_txt_file(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        cleaned = remove_comments(content)
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(cleaned)
        print(f"Cleaned: {os.path.basename(file_path)}")
    except Exception as e:
        print(f"Error processing {file_path}: {e}")

def process_directory(relative_path):
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Remove leading slash and resolve relative to script directory
    normalized = relative_path.lstrip('/')
    target_dir = os.path.normpath(os.path.join(script_dir, normalized))

    if not os.path.isdir(target_dir):
        print(f"Directory does not exist: {target_dir}")
        return

    print(f"Processing directory: {target_dir}")

    for root, _, files in os.walk(target_dir):
        for filename in files:
            if filename.endswith('.txt'):
                full_path = os.path.join(root, filename)
                process_txt_file(full_path)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 remove_comments.py /relative_path")
    else:
        process_directory(sys.argv[1])
