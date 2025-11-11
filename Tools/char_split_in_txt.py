#!/usr/bin/env python3
"""
char_split_in_txt.py

Splits .txt files into parts of N characters.

Usage:
  ./char_split_in_txt.py [chunk_size] [directory] [--no-recursive]

Arguments:
  chunk_size   - (optional) number of characters per part. If the first argument is a number,
                 it will be used as chunk_size. Default is 10000.
  directory    - (optional) directory to scan. If not provided → current directory.
                 If it starts with '/', it's treated as a subdirectory of the current directory.
  --no-recursive - (optional) only scan the specified directory, not its subdirectories.

Examples:
  ./char_split_in_txt.py                  # chunk_size=10000, directory=.
  ./char_split_in_txt.py 15000            # chunk_size=15000, directory=.
  ./char_split_in_txt.py /tmp             # chunk_size=10000, directory=./tmp
  ./char_split_in_txt.py 12000 /tmp       # chunk_size=12000, directory=./tmp
"""

from pathlib import Path
import argparse
import os
import re

DEFAULT_CHUNK_SIZE = 10000
PART_PATTERN = re.compile(r'(.+)_p(\d+)$', re.IGNORECASE)  # matches files ending with _p<number>

def read_text_file(path: Path) -> str:
    try:
        return path.read_text(encoding='utf-8')
    except UnicodeDecodeError:
        return path.read_text(encoding='latin-1')

def split_into_chunks(text: str, size: int):
    for i in range(0, len(text), size):
        yield text[i:i+size]

def is_already_part(path: Path) -> bool:
    """Checks if a file is already a part (ends with _p<number>.txt)"""
    stem = path.stem
    return bool(PART_PATTERN.match(stem))

def write_part_file_if_missing(orig_path: Path, part_index: int, content: str):
    """
    Writes a part file only if it doesn't already exist.
    Returns: (new_path, written: bool)
    """
    parent = orig_path.parent
    stem = orig_path.stem
    suffix = orig_path.suffix or '.txt'
    new_name = f"{stem}_p{part_index}{suffix}"
    new_path = parent / new_name

    if new_path.exists():
        return new_path, False

    try:
        new_path.write_text(content, encoding='utf-8')
        return new_path, True
    except Exception as e:
        print(f"[ERROR] Failed to write {new_path}: {e}")
        return new_path, False

def process_file(path: Path, chunk_size: int):
    if is_already_part(path):
        print(f"Skipped (already a part): {path}")
        return

    try:
        text = read_text_file(path)
    except Exception as e:
        print(f"[ERROR] Failed to read {path}: {e}")
        return

    total_chars = len(text)
    if total_chars <= chunk_size:
        print(f"Not split (<= {chunk_size} characters): {path} → {total_chars} characters")
        return

    parts_info = []
    for idx, chunk in enumerate(split_into_chunks(text, chunk_size), start=1):
        new_path, written = write_part_file_if_missing(path, idx, chunk)
        parts_info.append((new_path, written))

    written_count = sum(1 for _, w in parts_info if w)
    skipped_count = sum(1 for _, w in parts_info if not w)

    print(f"Split: {path} → {total_chars} characters into {len(parts_info)} parts (written: {written_count}, skipped existing: {skipped_count}):")
    for p, w in parts_info:
        status = "written" if w else "skipped (already exists)"
        print(f"  - {p} → {status}")

def iterate_txt_files(directory: Path, recursive: bool):
    if recursive:
        for root, _, files in os.walk(directory):
            for name in files:
                if name.lower().endswith('.txt'):
                    yield Path(root) / name
    else:
        for p in directory.iterdir():
            if p.is_file() and p.suffix.lower() == '.txt':
                yield p

def parse_target_directory(arg_value: str) -> Path:
    cwd = Path.cwd()
    if not arg_value:
        return cwd
    cleaned = arg_value.lstrip('/')
    if cleaned == "":
        return cwd
    return cwd / Path(cleaned)

def parse_args():
    parser = argparse.ArgumentParser(description="Splits .txt files into parts of N characters.")
    parser.add_argument("arg1", nargs='?', help="First argument: chunk_size or directory (optional)")
    parser.add_argument("arg2", nargs='?', help="Second argument: directory (optional if arg1 is chunk_size)")
    parser.add_argument("--no-recursive", action="store_true", help="Do not scan directories recursively")
    return parser.parse_args()

def main():
    args = parse_args()

    # Determine if first argument is chunk_size or directory
    chunk_size = DEFAULT_CHUNK_SIZE
    directory_arg = None

    if args.arg1 is not None:
        a1 = args.arg1.strip()
        if a1.lstrip('+-').isdigit():
            try:
                parsed = int(a1)
            except ValueError:
                parsed = None
            if parsed is not None and parsed > 0:
                chunk_size = parsed
                directory_arg = args.arg2  # second argument becomes directory
            else:
                print(f"Error: chunk_size must be a positive integer. Provided: {a1}")
                return
        else:
            # arg1 is directory
            directory_arg = args.arg1

    target_dir = parse_target_directory(directory_arg)
    if not target_dir.exists() or not target_dir.is_dir():
        print(f"Error: '{target_dir}' does not exist or is not a directory.")
        return

    if chunk_size <= 0:
        print("Error: chunk_size must be > 0.")
        return

    recursive = not args.no_recursive
    files = list(iterate_txt_files(target_dir, recursive))
    if not files:
        print("No .txt files found to process.")
        return

    print(f"Started: directory='{target_dir}', chunk_size={chunk_size}, recursive={recursive}")
    for f in files:
        process_file(f, chunk_size)

if __name__ == "__main__":
    main()
