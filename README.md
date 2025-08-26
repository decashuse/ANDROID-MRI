# ANDROID-MRI
A# ANDROID-MRI — RASP Evaluation Pipeline (4 Scripts)

This README documents the four scripts — **`1.unzip.sh`**, **`2.proc_thread.py`**, **`3.filter.sh`**, and **`4.make_csv.sh`** — as a single, end-to-end pipeline that turns an APK batch into a **tag-matrix CSV** using YARA rules.

> **At a glance**
>
> 1) **Extract** APK contents → 2) **Match** YARA rules (multi-threaded) →  
> 3) **Aggregate** tags per app → 4) **Export** one-hot CSV  
> **Artifacts:** `log.txt`, `output.txt`, `output.csv`

---

## Requirements

- **Linux/macOS** with bash  
- **7-Zip CLI** (`7z`) — e.g., Ubuntu: `sudo apt-get install p7zip-full`  
- **YARA** — e.g., Ubuntu: `sudo apt-get install yara`  
- **Python 3.9+** and `yara-python` — `pip install yara-python`  
- **GNU grep** with PCRE (`-P`), plus `awk`, `sort`, `uniq`, `sed`, `tr`  

> Prepare two inputs before you start:  
> - A directory containing the APK files  
> - A directory containing your YARA rules (`.yar` files)

---

## Files in this pipeline

```
1.unzip.sh         # Step 1: extract APKs to per-app subfolders
2.proc_thread.py   # Step 2: multi-thread YARA scan over extracted files (prints matches)
3.filter.sh        # Step 3: aggregate tags per app from the scan log
4.make_csv.sh      # Step 4: build a one-hot CSV across all discovered tags
```

---

## Quick start

```bash
# 1) Extract APKs
bash 1.unzip.sh

# 2) Run YARA and capture matches
python3 2.proc_thread.py > log.txt

# 3) Aggregate by app
bash 3.filter.sh

# 4) Build CSV
bash 4.make_csv.sh
```

**Results:** `log.txt`, `output.txt`, `output.csv`

---

## Step 1 — Extract APKs (`1.unzip.sh`)

**What it does**  
- Iterates all `*.apk` inside `SOURCE_DIR`  
- Creates `OUTPUT_DIR/<apk_basename>/` for each APK  
- Extracts **only** selected file types (`*.apk`, `*.dex`, `*.so`) via 7-Zip  
- Uses multi-threaded 7-Zip (`-mmt=<THREADS>`) by default  

**Edit these variables first**
```bash
SOURCE_DIR="/path/to/apk_batch"
OUTPUT_DIR="/path/to/unzipped"
THREADS=16   # adjust for your machine
```

**Run**
```bash
bash 1.unzip.sh
```

**Output structure (example)**
```
OUTPUT_DIR/
  ├─ com.example.app1/
  │    ├─ classes.dex
  │    ├─ lib/arm64-v8a/*.so
  │    └─ ...
  └─ com.example.app2/
       └─ ...
```

---

## Step 2 — Multi-thread YARA scan (`2.proc_thread.py`)

**What it does**  
- Compiles **all** YARA rules found under `yara_rule_directory`  
- Recursively walks the `unzip_directory` and scans every file with a **thread pool**  
- Prints one line per match to **STDOUT** with the file path and **rule tags**

**Edit these variables first (top of the file)**
```python
apk_directory       = '...'  # optional, not required by the pipeline
unzip_directory     = '...'  # set to OUTPUT_DIR from Step 1
yara_rule_directory = '...'  # directory containing .yar files
```

**Run (and capture log)**
```bash
python3 2.proc_thread.py > log.txt
```

**Expected log line (example)**
```
File: /.../com.example.app/lib/arm64-v8a/libfoo.so ['rasp','hooking']
```

> **Notes**  
> - A file can match **multiple rules** and **multiple tags**.  
> - The script slices paths around a literal `...` for display. For robust relative paths, replace it with:
>   ```python
>   file_path_rel = os.path.relpath(file_path, unzip_directory)
>   print(f"File: {file_path_rel}", end="")
>   ```

---

## Step 3 — Aggregate tags per app (`3.filter.sh`)

**What it does**  
- Reads `log.txt`  
- Groups matches by **top-level folder name** (treated as the *app ID*)  
- Collects tags for each app, **sorts**, and **de-duplicates**  
- Writes an app-to-tags summary to `output.txt`

**Run**
```bash
bash 3.filter.sh
```

**Output (`output.txt`, example)**
```
com.example.app ['hooking'] ['rasp'] ...
```

---

## Step 4 — One-hot CSV over all tags (`4.make_csv.sh`)

**What it does**  
- Scans `output.txt` to discover the **full tag vocabulary** (CSV columns)  
- For each app line, marks `o` if a tag is present, `X` otherwise  
- Produces `output.csv`

**Run**
```bash
bash 4.make_csv.sh
```

**Output (`output.csv`, example)**
```csv
app name,hooking,rasp,root,packer
com.example.app,o,o,X,X
other.app,X,o,o,X
```

---

## Output files

- **`log.txt`** — raw, file-level YARA matches (`File: <path> [tags]` per line, may include multiple tag lists)  
- **`output.txt`** — app-level, de-duplicated tag summary (space-separated lists like `['tag1'] ['tag2']`)  
- **`output.csv`** — one-hot app×tag matrix (`o` / `X` values)

---

## Verification (how each script behaves)

- **`1.unzip.sh`**  
  - Loops over `SOURCE_DIR/*.apk`, makes per-APK subfolders under `OUTPUT_DIR`  
  - Calls `7z x -y -mmt=<THREADS> -ir!*.apk -ir!*.so -ir!*.dex`  
  - Prints success/failure per APK  

- **`2.proc_thread.py`**  
  - Compiles rules from `yara_rule_directory`  
  - Walks `unzip_directory` with a worker queue and thread pool  
  - On each match: prints `File: <path> [rule.tags]`  

- **`3.filter.sh`**  
  - Parses lines beginning with `File:` in `log.txt`  
  - Uses path’s first component (top-level folder) as the app key  
  - Token-collects tags, sorts, de-duplicates, writes one line per app to `output.txt`  

- **`4.make_csv.sh`**  
  - Greps all bracketed tag lists from `output.txt` to build the **set of all tags**  
  - For each app line: fills `o`/`X` per tag and writes `output.csv`  

---

## Tips & known caveats

1. **GNU grep (PCRE) required**  
   `4.make_csv.sh` uses `grep -oP`. On macOS, install GNU grep (or replace the extraction with `perl`).  

2. **Tag tokens vs. bracket groups**  
   Because `3.filter.sh` sorts tokens, bracketed groups like `['a','b']` may be split.  
   `4.make_csv.sh` expects bracketed lists (regex: `\['.*?'\]`).  
   For stability, either:  
   - Standardize Step 2 tag output to comma-only without spaces, or  
   - Rewrite Step 3 as a small Python aggregator that parses brackets explicitly.  

3. **Relative paths**  
   Replace the `...` slicing in Step 2 with `os.path.relpath` for consistent, repository-relative paths.  

4. **Performance tuning**  
   Adjust `THREADS` in Step 1 and the thread-pool size in Step 2 to fit your CPU/I/O.  

---

## Customization

- Different APK batch → change `SOURCE_DIR` in **`1.unzip.sh`**  
- Different YARA rules → change `yara_rule_directory` in **`2.proc_thread.py`**  
- Different artifact names/paths → tweak variables at the top of **`3.filter.sh`** and **`4.make_csv.sh`**
