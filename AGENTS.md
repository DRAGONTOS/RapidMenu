# Agent guide

RapidMenu is a small CLI tool that turns TOML config files into menus for a runner like rofi, dmenu or bemenu: it parses the config, prints the action names, pipes them into the runner and executes the command of whatever the user picks. It can also emit a tiny bash wrapper script (`-b`) so a config can be launched as its own command. Written in C++26 on CMake and Ninja, parses TOML with cpptoml (tomlplusplus is a declared dep too). Packaged as a deb, an rpm and an Arch `PKGBUILD`. Contributors: DRAGONTOS, maukkis.

Cross-session handoff notes for this project live in `~/.local/share/agents/rapidmenu-handoff.md`. Read that file first when resuming after a context reset, and update it when state changes (commits, queue changes, new gotchas).

## Architecture

| Part | Where | Notes |
|---|---|---|
| Entry and arg parsing | `src/main.cpp` | Loops over argv, dispatches flags: `-c` config, `-b` build executable, `-h` help. Fills the `cmd` struct (from `Strings.hpp`) and calls `config` or `executable`. Anything unparsed falls through to a bare `Arguments: N` print. |
| UI strings and shared types | `src/includes/Strings.hpp` | `USAGE` and `HELP` raw string literals (currently identical), the `cmd` struct and the `invalidvalue`/`invalidconfig` strings. Keep them in sync with the flags in `main.cpp`. |
| Config runner | `src/config.cpp` | `config` makes sure `~/.config/RapidMenu` exists (creates it and exits on first run), parses the TOML with cpptoml, builds the newline-joined `namesList` (in **reversed** table order), builds the runner command as a shell string (`printf '<names>' | <rcommand> '<rname>' <rtheme>`) and hands it to `runnercmd`. `runnercmd` popen's it, matches the picked name against the config's tables and runs the matching `command` through `system`, printing the `description`. |
| Executable builder | `src/executable.cpp` | `executable` makes sure `~/.local/bin` exists, validates the config file, prompts on overwrite (`y/N` loops to a new name) and writes a two-line bash wrapper (`RapidMenu -c <config>`) into `~/.local/bin/<exename>` with owner+group exec bits. |

## Build and install

```sh
make all
sudo make install
```

`make all` clears `build/` and reconfigures with Ninja. Note the `release` target actually passes `-DCMAKE_BUILD_TYPE:STRING=Debug`. Packaging:

```sh
cd build && cpack -G DEB   # or RPM
makepkg -si PKGBUILD       # Arch
```

| Command | Use |
|---|---|
| `make all` | Fresh cmake configure + build into `build/RapidMenu`. |
| `RapidMenu` | Bare run: sets up `~/.config/RapidMenu` if missing, else prints usage. |
| `RapidMenu -c <config>` | Parse the config, show the menu through its `[runner]` and run the picked action. |
| `RapidMenu -b <config> <exename>` | Write an executable wrapper for the config into `~/.local/bin/<exename>`. |
| `RapidMenu -h` | Help (stderr, exit 1). |

Every config needs a `[runner]` table (`rname`, `rtheme`, `rcommand` — the last defaults to `rofi -dmenu -p`); each other table is an action with `names`, optional `description` and `command`.

## Hard rules

- Never push (git or a release) without the owner's explicit approval.
- Never run `sudo` or `make install` on your own; building and running the binary from `build/` is fine.
- GPLv3, keep the LICENSE.
- Commit subjects are lowercase with a type prefix and no body: `fix:`, `added:`, `bump:`, `cleanup:`, `major:`, `pkgbuild:`. The repo also has older `pull:`/`core:` merges — don't reuse those.
- The version in `props.json` is the single source of truth; bump it and `line-count` together.
- Don't add a comment where the code can say it itself; the codebase does comment, but sparingly — follow the surrounding file.

## Before pushing

- `make all` builds clean.
- A manual `-c` run against a real config opens the runner and executes an action.
- `USAGE` and `HELP` in `src/includes/Strings.hpp` still match the flags actually implemented.
- `props.json`, `line-count` and any packaging metadata agree on the version.

## Code style

- C++26 (`set(CMAKE_CXX_STANDARD 26)`), target `RapidMenu`, sources globbed from `src/` recursively — a new file needs no CMake edit.
- Indentation is a mix of 2 and 4 spaces across files; match the file you are editing rather than reformatting.
- Errors are handled with plain `exit(1)` and stderr prints, plus a few `try`/`catch` blocks around TOML parsing. Keep new errors consistent with their file.
- No test suite exists. Verification is manual, via the commands above.

## Gotchas

- CMake only `pkg_check_modules`s **tomlplusplus**, but the code parses with **cpptoml** (`cpptoml.h`). It works only because cpptoml is header-only and installed; both packages must be present (the PKGBUILD and README list both).
- `namesList` is built by `std::reverse`ing the table iteration order — the menu shows actions bottom-to-top of the config file. If listing order ever looks wrong, that reverse is why.
- The runner command is a single concatenated shell string passed through `popen`, and action commands go through `system`. `namesList` and `rname` are `shellquote`d and names go through `printf '%s'` (a `'` or `%` in a name is safe), but `rcommand` and `rtheme` stay raw on purpose — they are shell text by design, so config values there are still effectively shell-executed. Don't "simplify" by trusting them.
- `config` `exit(1)`s after creating the config dir, so a first `-c` run only sets the dir up and the user must invoke again; a bare run creates it and exits 0.
- Success paths return 0 since the 2026-11-18 fixes: `-h`, `-c`, `-b` and `executable`'s wrapper write; failure paths (bad config, no match, failed action command, unknown flag) return 1.
- The Makefile `release` target builds with `CMAKE_BUILD_TYPE=Debug`, and both the Makefile and PKGBUILD do `chmod -R 777 ./build`.
- The PKGBUILD runs `sudo make install` inside `package()` — never run it in a sandbox.
- `main.cpp` arg parsing: each flag branch checks `argc` itself (`-c` wants exactly one arg, `-b` exactly two); unknown flags print USAGE and return 1. Watch these checks when adding flags or reordering args.
- The `-h` argc checks (`argc < 3 || argv[2][0] == '-'`) that used to be copied into every branch are gone — flags now validate their own arg counts.
