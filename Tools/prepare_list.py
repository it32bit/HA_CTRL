
import os
import sys
import hashlib

def load_ignore_paths(ignore_file):
    ignored = set()
    if ignore_file and os.path.isfile(ignore_file):
        with open(ignore_file, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#"):
                    ignored.add(os.path.normpath(line))
    return ignored

def append_extension_to_filename(filename, extension):
    ext_clean = extension.lstrip(".")
    base, ext = os.path.splitext(filename)
    return f"{base}_{ext_clean}.txt"

def find_and_merge_unique_files(start_path, extension, output_file=None, ignore_file=None):
    if not extension.startswith("."):
        extension = "." + extension

    if not os.path.isdir(start_path):
        print(f"Provided path does not exist or is not a directory: {start_path}")
        return

    # Always append extension to output filename
    if output_file is None:
        output_file = f"output_{extension[1:]}.txt"
    else:
        output_file = append_extension_to_filename(output_file, extension)

    ignored_paths = load_ignore_paths(ignore_file)
    unique_hashes = set()

    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    with open(output_file, "w", encoding="utf-8") as out:
        for root, _, files in os.walk(start_path):
            relative_root = os.path.relpath(root, start_path)
            if any(relative_root.startswith(ig) for ig in ignored_paths):
                continue

            for file in files:
                if file.endswith(extension):
                    full_path = os.path.join(root, file)
                    try:
                        with open(full_path, "r", encoding="utf-8") as f:
                            content = f.read()
                            content_hash = hashlib.sha256(content.encode("utf-8")).hexdigest()
                            if content_hash not in unique_hashes:
                                unique_hashes.add(content_hash)
                                out.write(f"\n\n--- From file: {full_path} ---\n")
                                out.write(content)
                            else:
                                print(f"Skipped duplicate: {full_path}")
                    except Exception as e:
                        print(f"Failed to read file {full_path}: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python script.py /path/to/folder extension [output_file] [ignore_file]")
    else:
        start_path = sys.argv[1]
        extension = sys.argv[2]
        output_file = sys.argv[3] if len(sys.argv) > 3 else None
        ignore_file = sys.argv[4] if len(sys.argv) > 4 else None
        find_and_merge_unique_files(start_path, extension, output_file, ignore_file)
