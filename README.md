# PZ-Injector
Project Zomboid Injector using SetWindowsHookEx.

Made for [Emil, from Esd.GHJ](https://github.com/Emilprivate/Esd.GHJ/tree/main/Project%20Zomboid), a project we was working on.

## Usage
- Open the game.
- Open the loader (in menu or in game)
- If everything it's okay, you'll get notified, have fun!

### Configuration
Before using the injector, make sure to set up the configuration file (`config.ini`) with the desired DLL pattern and window title. The `config.ini` file should be placed in the same directory as the injector executable.

Here's an example `config.ini` file content:

```ini
# Configuration file.
DllPattern=Prototype*.dll
WindowTitle=Project Zomboid
