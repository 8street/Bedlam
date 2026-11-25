# AGENTS.md

Instructions for AI coding agents and contributors working in this repository.
Read this file before making changes.

## 1. Project purpose and scope

- This repository contains reverse-engineered source code for the 1996 game
  **Bedlam**, reconstructed from the original executable for cultural
  preservation and to make the game playable on modern operating systems.
- The original game's behavior is the specification. Treat differences from
  the original as bugs unless they are deliberate, documented improvements
  (bug fixes, audio quality, SDL port, resolution options).
- Current state: single-player is fully playable and known crashes are fixed;
  multiplayer has not been restored. Do not claim otherwise.
- The repository contains **no original game assets**. Never add, generate, or
  commit original data files, music, movies, or the original `Bedlam.exe`.

## 2. Repository layout

| Path | What it is | How to treat it |
| --- | --- | --- |
| `ASM_sources/bedlam.asm` | ~96k lines disassembled from C sources compiled by Watcom in 1995-96 with optimizations enabled; MASM syntax, IDA-style names and address comments | Authoritative game logic |
| `ASM_sources/bedlam.inc` | `extrn` declarations: C++ functions called from ASM and ASM globals used by C++ | Keep in sync with the C++ headers |
| `ASM_sources/bedlam_data.inc` | ~570k lines of generated data tables | Generated data; never reformat or rewrite wholesale |
| `CPP_sources/` | ~30 `.h`/`.cpp` pairs implementing the SDL2 port and functions called from ASM | Main area for new work; C++17 |
| `libsmacker-1.2.0/` | Vendored third-party Smacker video decoder | Do not modify unless fixing a specific integration bug |
| `SDL2/`, `SDL2_mixer/` | Bundled Windows headers and libraries | Third-party; never edit |
| `vs2019/` | Visual Studio solution and project, Win32/x86 only, MASM enabled | Register every new C++ file here |
| `linux/` | `prepare.sh`, `compile.sh`, Linux build documentation | Second supported build system |
| `.github/workflows/` | CI for Windows (MSBuild) and Linux (i386) builds, triggered on push and PR to `master` | Keep both workflows working |
| `README.md` | User-facing build and run instructions | Update when build or run steps change |

## 3. Assembly/C++ boundary (read before touching either side)

- ASM calls into C++ through `extern "C"` symbols declared in `bedlam.inc`;
  C++ declares the same symbols in headers such as `bedlam.h`,
  `bedlam_draw.h`, `keyboard.h`, `mouse.h`, `main.h`, and others.
  Any signature change must be applied on **both** sides, including exact
  symbol spelling (`option casemap:none` means names are case-sensitive).
- Many ASM entry points are `__fastcall` wrappers: they save
  `EBX/ECX/EDX/ESI/EDI`, push arguments onto the stack, call the C++
  function, then restore registers and return a value in `EAX`.
  Do not change register usage or stack cleanup without inspecting every
  caller. Comments like `; eax = Smack *` document the return value.
- Globals shared with ASM are often declared `volatile` in C++
  (`screen_x_pos`, `game_is_playing`, `MOUSE_BUTTONS_STATE1`, etc.).
  Preserve that qualifier; these values change outside the C++ call stack.
- This is a **32-bit x86** codebase only. Pointers and hardcoded offsets
  assume 32-bit; do not start a 64-bit port as part of an unrelated change.
- Rendering code contains hardcoded geometry and a 640-byte screen stride
  (for example, `&buffer[640 * y + x]`). Audit all buffer arithmetic before
  changing resolution, screen width, or buffer layout constants.
- IDA-generated names (`loc_401000`, `v6`, `v7`) and hexadecimal address
  comments (`// 00401E39`) are intentional cross-references to the original
  binary. Keep them when moving or porting code; do not rename them for style.
- The disassembly comes from optimized Watcom C output. A `proc` does not
  necessarily end with a local `retn`: the compiler merged common epilogues,
  so a function may leave through a `jmp` to a shared tail elsewhere in the
  file. Trace such jumps before changing control flow or extracting a
  routine.

## 4. Building and verifying

### Supported environments

Neither platform is mandatory locally: use the build system you have and let
GitHub Actions verify the other one on the pull request.

| Environment | Build locally | Verify the other platform |
| --- | --- | --- |
| Windows only | `vs2019/Bedlam.sln`, Win32/x86 | LinuxBuild CI via PR |
| Linux only | `cd linux && ./compile.sh` (i386 toolchain and libs) | MSBuild CI via PR |
| Both | run both builds | CI still runs both on PR |

### Windows

1. Open `vs2019/Bedlam.sln` in Visual Studio 2022.
2. Build the **Win32/x86** configuration (toolset v143, MASM customizations
   are imported by the project). Both Debug and Release must compile.
3. CI equivalent: restore NuGet packages, then
   `msbuild /m /p:Configuration=Release ./vs2019/Bedlam.sln`
   (see `.github/workflows/msbuild.yml`).
4. Debug builds define `_DEBUG` and disable stdout/stderr buffering; run the
   debug executable to read console diagnostics when a change fails.

### Linux

1. Dependencies: clang, cmake, pkg-config, gcc/g++ multilib,
   libc6-dev-i386, JWasm (MASM syntax), 32-bit SDL2 >= 2.0.12 and
   SDL2_mixer. `./linux/prepare.sh` can install them but is heavy; the
   details are in `linux/readme.md`.
2. Build with `cd linux && chmod +x compile.sh && ./compile.sh`;
   the binary is `linux/bedlam`.
3. `compile.sh` compiles `libsmacker-1.2.0/*.c` and `CPP_sources/*.cpp`
   with `clang -m32 -std=c++17`, assembles `bedlam.asm` with
   `jwasm -elf -c -zcw`, and links with `clang++ -m32`.
4. Because Windows uses MASM and Linux uses JWasm with a different linker,
   verify ABI-affecting changes **on both platforms** before opening a PR —
   locally where possible, otherwise through the corresponding CI job.

### CI verification

- Both workflows (`.github/workflows/msbuild.yml` and
  `.github/workflows/linuxbuild.yml`) run on pushes and pull requests
  targeting `master` in the repository that contains the workflow.
- Pushing a feature branch alone does **not** trigger CI: open a PR to
  `master`. Contributors working in a fork can open a PR to the `master` of
  their fork to see both jobs in their own Actions tab; a PR to upstream
  `master` runs the workflows in the upstream repository. Workflows from
  first-time fork contributors may need maintainer approval before running.
- Windows CI is self-contained: the SDL2/SDL2_mixer x86 `.lib` files are
  committed under `SDL2/lib/x86` and `SDL2_mixer/lib/x86`, so no local
  Windows machine is needed for that job.
- Check results on the Actions tab of the relevant repository or through the
  GitHub API (`GET /repos/8street/Bedlam/actions/runs?event=pull_request`);
  adjust the repository path for a fork. The `gh` CLI is not required.
- If a job fails, fix the code and push another commit to the same branch;
  the PR re-runs CI automatically.

### Running

- Place the built binary next to an original Bedlam installation. The
  Windows 95 version is preferred because it includes all `.SMK` movies and
  language files.
- Startup checks require original files, for example
  `GAMEGFX/GAMEPAL.PAL` and `EDITOR/ZONEA/MISSIONA.BIN`; a standalone
  binary cannot run.
- On Linux, make the `SAVES` folder writable (`chmod 755 SAVES`) so that
  save files work.
- Prefer testing with real game data: menu navigation, a game start, sound,
  palette, and a movie are the areas most easily broken by port changes.
- Runtime testing requires a platform where you have those original files;
  a Linux-only contributor can run the Linux binary locally but will only get
  a compile check for the Windows build in CI.

## 5. Code style

- Follow `.clang-format`: 4-space indentation, 128-column limit, custom
  brace wrapping (braces on their own line), `PointerAlignment: Right`,
  no tabs.
- Run clang-format only on C++ files you touched. Do not reformat the whole
  codebase, the ASM sources, generated data, or vendored libraries.
- Keep fixed-width types (`int32_t`, `uint8_t`, ...) where the original data
  width matters; do not replace them with plain `int` or `long`.
- C++17 is the baseline. Avoid newer language/library features unless both
  build configurations are updated to support them.
- When adding a C++ source file, add it to `vs2019/Bedlam.vcxproj` (and
  `Bedlam.vcxproj.filters`). The Linux script already globs
  `CPP_sources/*.cpp`, so no change is needed there for ordinary sources.
- Write comments in English. Explain *why* a workaround exists (original
  engine quirk, SDL limitation, IDA artifact) rather than restating code.
- Keep changes focused: no drive-by cleanup or bulk renames of decompiled
  code.

### Tests

- Write automated tests only for C++ code in `CPP_sources/`.
- Do not write tests for the decompiled assembly in `ASM_sources/` or for
  generated `*.inc` data; validate ASM changes through both build
  configurations and manual runs with original game files.
- Do not introduce test frameworks, test projects, or CI test jobs unless
  explicitly requested.

### Diagnostics

- Report errors and ASM/C++ comparison mismatches with `std::cout`. The Debug
  configuration uses the Console subsystem and `main.cpp` disables
  stdout/stderr buffering, so messages (and the final `PAUSE`) are visible
  when running the debug build.
- The Release configuration uses the Windows subsystem and has no console;
  do not rely on console output there. Use the existing SDL message boxes for
  errors that must be shown to players.

## 6. Common change patterns

- **Porting an ASM routine to C++:** the long-term goal is to move game logic
  into `CPP_sources/` gradually while touching `ASM_sources/` as little as
  possible. Never delete the original ASM function during a port; keep its
  `extern "C"` entry point. Validate the new C++ implementation in Debug
  builds — compare simple results such as return values and report mismatches
  with `std::cout`. For functions with side effects (buffers, globals, SDL,
  sound, files), run one implementation at a time behind a Debug switch and
  compare observable behavior in the game; do not add complex
  state-duplication scaffolding unless explicitly needed. Copy the original
  address comment into the new function, keep the exact signature and calling
  convention, and update both `bedlam.inc` and the matching header when the
  symbol set changes.
- **Fixing a crash:** reproduce it with original game files first, note the
  game state and screen (menu, briefing, mission), then use the Debug
  build's console output. Do not "fix" a crash by silently skipping game
  logic when the real cause is still unknown.
- **Changing audio or video:** the port uses a 44.1 kHz SDL_mixer setup and
  libsmacker. Keep option values and the on-disk save/hiscore formats
  compatible (`CPP_sources/options.cpp`, `save.cpp`). Do not add new bundled
  binary dependencies.
- **Adding an SDL feature:** isolate platform-specific code in its own
  `CPP_sources/sdl_*.cpp` module, expose a narrow C++ interface, and call it
  from ASM only through an `extern "C"` function declared in `bedlam.inc`.
- **Changing resolution constants:** `resolutions.cpp`,
  `render_position.cpp`, and `bedlam_draw.cpp` share assumptions about
  buffer sizes; change them together and test the game view, map, and
  sidebar.

## 7. External references

- `README.md` links to the IDA database of the original `Bedlam.exe`; use it
  to verify addresses and calling conventions when decompiled code is
  ambiguous.
- Do not download, generate, or commit the IDA database, proprietary game
  files, or the original executable into this repository.

## 8. Contribution workflow

- Work in a branch and open a pull request against `master`; CI runs on
  pushes and pull requests to `master`.
- In the PR description, state what changed, why, how you tested it, which
  builds succeeded, and which game version/files you used.
- Keep commits small and self-contained, with imperative commit messages.
- Never commit binaries, `.dll`/`.exe` files, IDE user settings, build
  output, or game assets. Most of these are already covered by
  `.gitignore`; check `git status` before committing.
- Update `README.md` and/or `linux/readme.md` whenever build or run
  instructions change.

## 9. Review checklist

- [ ] Both Win32 and Linux i386 builds pass — locally where available and via
      GitHub Actions on the PR — or the PR explains why one was unavailable.
- [ ] Every `extern "C"` declaration matches between C++ headers and `bedlam.inc`.
- [ ] No unintended edits to `bedlam_data.inc`, `SDL2/`, `SDL2_mixer/`, or `libsmacker-1.2.0/`.
- [ ] New source files are registered in the Visual Studio project.
- [ ] Touched C++ files match `.clang-format`; ASM/data files have minimal diffs.
- [ ] Behavior changes are documented and tested with original game files.
- [ ] No generated files, binaries, or proprietary assets are staged.
