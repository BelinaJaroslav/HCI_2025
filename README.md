# HCI_2025

## Jak to spustit u sebe ve VS

* Je potřeba mít:
  * Plnotučné Visual Studio se zaškrtlým _Vývoj desktopových aplikací pomocí C++_
    * Lepší vybrat si angličtinu než češtinu kvůli troubleshootingu
  * Aktivované vcpkg v cestě __`C:\HCI\vcpkg`__

    ```powershell
    # Git bash:
    cd /c
    mkdir HCI
    git clone https://github.com/microsoft/vcpkg.git

    # Potom v PowerShell:
    cd \HCI\vcpkg; .\bootstrap-vcpkg.bat
    ```

    * Alternativně by šel přidat soubor `CMakeUserPresets.json` do .gitignore a každý by měl vlastní verzi souboru s vlastní cestou k vcpkg

* Potom:
  * Otevřít VS launcher
  * _Open a local folder_ a vybrat tuhle složku &ensp;(nikoliv _Open a project or solution_)
  * //TODO

## Jak budeme přidávat nové balíky

* VS _Tools_ → _Command Line_ → _Developer PowerShell_:

```powershell
$env:VCPKG_ROOT="C:\HCI\vcpkg"
$env:PATH="$env:VCPKG_ROOT;$env:PATH"
# vcpkg příkazy pro přidání balíků
```

Potom bude potřeba něco připsat do `CMakeLists.txt`
