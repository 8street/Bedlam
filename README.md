
# Bedlam

Source code for Bedlam (1996 year) executable file. To preserve the cultural heritage. Working on modern Windows.

This repository does not contain any original game resources.

---

![Bedlam Preview](https://www.old-games.ru/forum/attachments/bedlam_preview-png.265924/)

---

# Project State

The sources can be compiled and fully playable in single player mode. All known crashes has been fixed.

# List of changes

* Port to SDL (working on Windows. Linux also should work but not tested)
* Improved Audio (44.1kHz Mixer. More channels. Sounds a little better than original 11kHz)
* Fix some bugs

# Contributing

Contributors, welcome. Create a fork of this repository, then create a branch. Modify the source code in your branch. Finally, create a pull request to merge with my main branch.
Patches and improvements are welcome.

IDA reverse file of the original Bedlam.exe [here](https://www.dropbox.com/sh/h0yyypzx8gjkn9c/AABBuR-fwNthYnOYPf7VH-Q7a?dl=0).

# Compiling

### Windows

1. Open solution vs2019/Bedlam.sln in Visual Studio 2022
2. Compile x86 platform
3. Get original DOS or Windows95 game Bedlam (1996). The Windows95 version is better because it contain all .SMK movies and various language files.
4. Put compiled .exe, SDL2.dll, SDL2_mixer.dll files in your original Bedlam folder. Or put Bedlam files in VS output directory
5. Run

You can download executable and dll files from [releases](https://github.com/8street/Bedlam/releases). You may need to install "Microsoft Visual C++ 2015 Redistributable x86" to run game.
If game crashes, try run debug.exe and read errors in console.

### Linux

Please read [linux/readme.md](https://github.com/8street/Bedlam/blob/master/linux/readme.md).

You can download binary file from [releases](https://github.com/8street/Bedlam/releases). Put compiled binary file in your original DOS or Windows95 (better) Bedlam folder with game files.
You need to install SDL i686 v2.0.12 or above and SDL_mixer i686 to run game. How to do this, see the readme above.

To work save files run "chmod 755 %folder%" for SAVES folder in Bedlam directory. This folder will created after first run bedlam game.
