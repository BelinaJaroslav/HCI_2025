# Jak hoblovat

## Branche

```shell
# Vytvoří lokálně novou branch a přepne do ní
git checkout -b "feat/lab01task01"

# Klasika dokud není konkrétní zadání hotovo
git add .
git commit -m "..."
git push origin feat/lab01task01

# Na GitHubu se udělá Pull Request & Merge do main větve
# Případně vyřešit konflikty

# Merge z GitHubu je potřeba si synchronizovat lokálně
git checkout main
git pull origin main
```

Potom, co se úspešně zmerguje, je možné již nepotřebnou branch smazat na GitHubu (ve webovém UI) a u sebe pomocí `git branch -D feat/lab01task01`.

Všechny větve, o kterých lokál ví, lze vypsat pomocí `git branch -v`.

## Code style

`všechno_snake_case`

## Jak to spustit u sebe ve VS

* Je potřeba mít:
  * Plnotučné Visual Studio se zaškrtlým _Vývoj desktopových aplikací pomocí C++_
    * Lepší vybrat si angličtinu než češtinu kvůli troubleshootingu
  * Aktivované vcpkg v cestě __`C:\HCI\vcpkg`__:

    ```powershell
    # Git bash:
    cd /c
    mkdir HCI
    cd HCI
    git clone https://github.com/microsoft/vcpkg.git

    # Potom v PowerShell:
    cd \HCI\vcpkg; .\bootstrap-vcpkg.bat
    ```

    * Alternativně by šel přidat soubor `CMakeUserPresets.json` do .gitignore a každý by měl vlastní verzi souboru s vlastní cestou k vcpkg

* Potom:
  * Otevřít VS launcher
  * _Open a local folder_ a vybrat tuhle složku (tento repozitář) &ensp;(nikoliv _Open a project or solution_)
  * Nechat to chroupat dokud se v _Output_ neobjeví `1> CMake generation finished.`
  * Teď přichází haluz: je potřeba spustit _Build_ → _Build All_, ta možnost tam ale nejspíš nebude. Její objevení jsem vyvoval tak, že jsem si otevřel `HCI.cpp`, nahoře se objeví žluté oznámení → kliknout na _Generate_, načež se objeví další oznámení → kliknout na _vcpkg_.
  * Nyní už můžeme dát _Build_ → _Build All_
  * Spustit projekt, vybrat HCI.exe
  * Měla by se objevit konzole s pozdravem

### OpenCV

Přidat do PATH:

```text
C:\HCI\opencv\build\x64\vc16\bin
```

## Jak přidávat nové balíky

* VS _Tools_ → _Command Line_ → _Developer PowerShell_

```powershell
$env:VCPKG_ROOT="C:\HCI\vcpkg"
$env:PATH="$env:VCPKG_ROOT;$env:PATH"
# vcpkg příkazy pro přidání balíků
```

Potom bude potřeba něco připsat do `CMakeLists.txt`
