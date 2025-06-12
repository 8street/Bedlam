#!/bin/bash

# Delete previous version
rm bedlam

mkdir o
cd o || exit 1
rm -rf -- *

echo                                          
echo ============= run clang compiler =============
clang -m32 -c ../../libsmacker-1.2.0/*.c
clang++ -m32 -c -I ../../libsmacker-1.2.0/ ../../CPP_sources/*.cpp -std=c++17 $(pkg-config --cflags SDL2_mixer sdl2)

echo                                          
echo ============= run jwasm compiler =============
jwasm -elf -c -zcw ../../ASM_sources/bedlam.asm

cd ..

echo                                          
echo ============= run ld linker =============
clang++ -m32 o/*.o -o bedlam -lstdc++fs $(pkg-config --libs SDL2_mixer sdl2)

chmod 755 bedlam
