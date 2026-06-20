# SaveVault 

SaveVault is a blazing-fast, native C++ desktop application designed to automatically detect, organize, and back up PC game saves. 

Built specifically for DRM-free games and standard emulator hubs (RUNE, Goldberg, CODEX, FLT), SaveVault replaces tedious manual folder-digging with a smart, database-driven backup engine. 

##  Core Features

* **Hybrid Deep Scanner:** Instantly detects game saves in standard cracker hubs and dynamically crawls `AppData` and `Documents`. It uses a strict blacklist to actively avoid massive software cache folders (like Adobe, Discord, and IDE workspaces) preventing system hangs.
* **Steam API Translator:** Automatically contacts Valve's official Steam API to translate raw Steam AppIDs (e.g., `1627720`) into clean, official game titles (e.g., `Lies of P`).
* **SQLite Database Engine:** Logs every game, original file paths, and customized backup vault locations to ensure no duplicate backups or lost data.
* **QML Dark Mode Dashboard:** A lightweight, hardware-accelerated user interface built with Qt Quick.
* **Zero-Setup Backup:** Secures all physical save files directly to a designated vault with one click.

##  Prerequisites & Installation

To compile and run SaveVault, you need the Qt framework and a C++ compiler.

1. **Install Qt Creator:** Download the open-source Qt Online Installer from the [official Qt website](https://www.qt.io/download-open-source).
2. **Select Components:** During installation, ensure you install **Qt 6.x** (MinGW or MSVC) and check the following modules:
   * Qt Core
   * Qt GUI / QML / Quick
   * Qt SQL (for SQLite)
   * Qt Network (for Steam API HTTP requests)
3. **Clone the Repository:**
   ```bash
   git clone [https://github.com/NihalBambale11/SaveVault.git](https://github.com/NihalBambale11/SaveVault.git)

1.  **Build the Project:** \* Open SaveVault.pro or CMakeLists.txt in Qt Creator.
    
    *   Click **Build -> Run CMake** (if using CMake).
        
    *   Press **Ctrl + R** to compile and launch the application.
        

 How to Use (The 1-2-3 Workflow)
-----------------------------------

Because RUNE and Goldberg emulators use Steam ID numbers for folders, the app follows a strict 3-step workflow to ensure your backups are named beautifully.

1.  **Auto-Detect Games (The Scout):** Click this to let the C++ engine hunt through your AppData and Cracker Hubs. It finds the save files and writes their locations into the SQLite database as raw IDs (e.g., "Cracked Game (1627720)"). _No files are copied yet._
    
2.  **Translate Names (The Polish):** Click this to fire an asynchronous HTTP GET request to the Steam API. It translates the ID numbers into real titles (e.g., "Dying Light The Beast") and permanently updates the backup paths in your database.
    
3.  **Run Master Backup (The Muscle):** Click this to read the translated database. The app physically copies the actual save files to your Backups folder into perfectly named directories.
