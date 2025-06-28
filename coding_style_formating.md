# IsoMaker Format Checker

This project includes a Python-based format checker (`scripts/check_format_isomaker.py`) to enforce core coding style rules specific to IsoMaker:

## ✅ What it checks
- `#pragma once` present at the top of all `.hpp` files
- File ends with a newline (`\n`)
- Access specifier order in classes: public → protected → private

## ❌ What it doesn't check
- Variable or function naming (e.g. camelCase)
- Indentation or brace placement
- File/folder naming structure

## Run the check

```bash
python3 scripts/check_format_isomaker.py
