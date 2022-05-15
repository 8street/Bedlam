
# Bedlam

[![Lines of Code](https://tokei.rs/b1/github/8street/Bedlam?category=code)](https://github.com/XAMPPRocky/tokei)

Source code for Bedlam (1996 year) executable file. To preserve the cultural heritage. Working on modern Windows.

This repository does not contain any original game resources.

# Project State

The sources can be compiled and fully playable. Rare crashes are possible.

# List of changes

* Port to SDL (working on Windows)
* Fix some bugs

# Contributing

Contributors, welcome. Create a fork of this repository, then create a branch. Modify the source code in your branch. Finally, create a pull request to merge with my main branch.
Patches and improvements are welcome.

IDA reverse file of the original Bedlam.exe [here](https://www.dropbox.com/sh/h0yyypzx8gjkn9c/AABBuR-fwNthYnOYPf7VH-Q7a?dl=0).

# Compiling

1. Open solution vs2019/Bedlam.sln in Visual Studio 2019
2. Compile x86 platform
3. Get original DOS or Windows95 game Bedlam (1996)
4. Put compiled .exe, SDL2.dll, SDL2_mixer.dll files in your original Bedlam folder. Or put Bedlam files in VS output directory
5. Run (You may also need to find the SMACKW32.DLL file and put them in Bedlam folder, near executable file)

You can download executable and dll files from [releases](https://github.com/8street/Bedlam/releases). You may need to install "Microsoft Visual C++ 2015 Redistributable x86" to run game.
