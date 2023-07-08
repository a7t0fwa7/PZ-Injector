# PZ-Injector
Project Zomboid Injector using SetWindowsHookEx.

Made for [Emil, from Esd.GHJ](https://github.com/Emilprivate/Esd.GHJ/tree/main/Project%20Zomboid), a project we was working on.

## Usage
1. Download the latest release.
2. Launch the injector executable (`Loader.exe`).
3. The loader will try to find a config file (`config.ini`), if does not find it, it will make one with default values.
4. If the injection is successful, the console will display a message confirming the successful injection.
5. The console window will be hidden automatically after 2 seconds.
6. The injector will continue to run in the background, waiting for the Project Zomboid game window to close.
7. To unload the injected DLL and exit the injector, simply close the Project Zomboid game window.

### Configuration
Before using the injector, make sure to set up the configuration file (`config.ini`) with the desired DLL pattern and window title. The `config.ini` file should be placed in the same directory as the injector executable.

Here's an example `config.ini` file content:

```ini
# Configuration file.
DllPattern=Prototype*.dll
WindowTitle=Project Zomboid
```
- DllPattern (default: Prototype*.dll): Pattern match for the DLL file to inject. Use asterisks (*) as wildcards for file name matching.
- WindowTitle (default: Project Zomboid): Game window title.
