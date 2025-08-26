# ANDROID-MRI

Here’s the **complete `README.md`** in English, with everything merged into one clean file (no accuracy limits, license, ethics, or change log). You can drop this directly into your repo:

```markdown
# RASP_Evaluation

This folder contains scripts and YARA rules for detecting the presence of **Runtime Application Self-Protection (RASP)** mechanisms in Android apps.  
It was originally used in the **ANDROID-MRI** study to build large-scale measurements of RASP prevalence and analyze its effectiveness.

---

## Purpose

- Scan APK files for **RASP-related patterns** inside `classes.dex`, `lib/*.so`, and `assets/*`.
- Detect and categorize the following five classes of RASP mechanisms:
  - **Tamper detection** (rooting/hooking detection, anti-debugging)
  - **Obfuscation**
  - **Anti-disassembly**
  - **Protector** (wrappers and execution protectors)
  - **Packer** (encrypted/packed loaders)
- Batch-process large sets of APKs and generate results in CSV format.
- Summarize the output into tables suitable for research papers.

---

## Directory Structure (example)

```

RASP\_Evaluation/
├─ rules/                # YARA rules for each RASP category
│   ├─ tamper/*.yar
│   ├─ obfuscation/*.yar
│   ├─ anti\_disasm/*.yar
│   ├─ protector/*.yar
│   └─ packer/\*.yar
├─ scripts/              # Python and shell scripts
│   ├─ scan\_apks.py      # Batch scan of APK directory
│   ├─ scan\_one.py       # Single APK scan
│   ├─ summarize.py      # Aggregate CSV results
│   └─ extract.sh        # APK extraction helper
├─ examples/             # Example runs and sample outputs
├─ results/              # Generated outputs
└─ README.md

````

---

## Requirements

- **Python 3.9+**
- **yara / yara-python**
  ```bash
  sudo apt-get update && sudo apt-get install -y yara
  python3 -m pip install -U yara-python tqdm pandas
````

* Optional: APK extraction tools (`unzip`, `aapt2`, `jadx`, or `apktool`)

---

## Quick Start

1. Prepare a folder with APKs:

   ```bash
   ls apks/*.apk | head
   ```

2. Run a batch scan:

   ```bash
   python3 scripts/scan_apks.py \
     --apks ./apks \
     --rules ./rules \
     --out ./results/rasp_scan.csv \
     --workers 8
   ```

3. Scan a single APK (debugging mode):

   ```bash
   python3 scripts/scan_one.py \
     --apk ./apks/sample.apk \
     --rules ./rules \
     --print-hits
   ```

4. Summarize results:

   ```bash
   python3 scripts/summarize.py \
     --csv ./results/rasp_scan.csv \
     --out ./results/summary.csv
   ```

---

## Output Format

Example `rasp_scan.csv`:

| apk\_name     | package         | tamper\_detection | obfuscation | anti\_disassembly | protector | packer | hits                                          |
| ------------- | --------------- | ----------------- | ----------- | ----------------- | --------- | ------ | --------------------------------------------- |
| SampleApp.apk | com.example.app | 1                 | 0           | 1                 | 0         | 0      | `tamper/generic_rule1; anti_disasm/some_rule` |

* Each category is represented as **1 (present) / 0 (absent)**.
* The `hits` column lists the YARA rules that matched.

---

## Writing YARA Rules

Each rule should declare its category in the **meta** section:

```yara
rule RASP_Tamper_Generic_Magisk_Strings {
  meta:
    category = "tamper_detection"
    vendor   = "generic"
    note     = "Common root/hooking detection strings"
  strings:
    $a = "magisk" nocase ascii
    $b = "frida"  nocase ascii
    $c = "supersu" nocase ascii
  condition:
    any of them
}
```

* Use descriptive categories (`tamper_detection`, `obfuscation`, etc.).
* Combine multiple patterns to reduce false matches.
* For packers/protectors, check both `classes.dex` and native `lib/*.so`.

---

## Batch Experiment Tips

* Use the `--workers` option to parallelize scans for large datasets.
* If APKs are large, extract only the necessary files (`classes*.dex`, `lib/*`) before scanning to improve performance.
* For research outputs, map results from **1/0** to **✓/–** for tables and figures.

```

---

👉 Would you like me to also create a **shorter academic-style README** (just summarizing purpose, dataset, and usage in the paper) so you can use that for the publication’s artifact submission, while keeping this longer one for GitHub?
```
