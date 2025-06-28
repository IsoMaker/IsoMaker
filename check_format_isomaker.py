import os
import re
from pathlib import Path

ROOT_DIR = "."

CPP_EXTENSIONS = (".cpp", ".hpp")

class_regex = re.compile(r"class\s+([A-Za-z0-9_]+)")
function_regex = re.compile(r"\b([a-zA-Z_][a-zA-Z0-9_]*)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^;]*\)\s*[{;]")
camel_case = re.compile(r"^[a-z]+(?:[A-Z][a-z0-9]*)*$")
pascal_case = re.compile(r"^[A-Z][a-zA-Z0-9]*$")


def enforce_pragmas_and_newlines(filepath: Path):
    with filepath.open("r+", encoding="utf-8") as f:
        lines = f.readlines()
        changed = False

        if filepath.suffix == ".hpp" and not any("#pragma once" in l for l in lines[:5]):
            lines.insert(0, "#pragma once\n")
            changed = True

        # Vérifier \n à la fin
        if lines and not lines[-1].endswith("\n"):
            lines[-1] += "\n"
            changed = True

        if changed:
            f.seek(0)
            f.writelines(lines)
            f.truncate()
            print(f"[UPDATED] {filepath.relative_to(ROOT_DIR)}")


def check_naming_conventions(filepath: Path):
    content = filepath.read_text(encoding="utf-8")
    warnings = []

    for match in class_regex.finditer(content):
        class_name = match.group(1)
        if not pascal_case.match(class_name):
            warnings.append(f"Class `{class_name}` should be UpperCamelCase.")

    for return_type, func_name in function_regex.findall(content):
        if func_name in ("if", "for", "while", "switch", "return"):
            continue
        if not camel_case.match(func_name):
            warnings.append(f"Function `{func_name}` should be lowerCamelCase.")

    if warnings:
        print(f"[WARNINGS] {filepath.relative_to(ROOT_DIR)}")
        for warn in warnings:
            print("  -", warn)


def walk_and_process():
    for root, _, files in os.walk(ROOT_DIR):
        for file in files:
            if file.endswith(CPP_EXTENSIONS):
                path = Path(root) / file
                enforce_pragmas_and_newlines(path)
                check_naming_conventions(path)


if __name__ == "__main__":
    print("🧹 Checkint C++ coding style...")
    walk_and_process()
    print("\n✅ Finished checking.")
