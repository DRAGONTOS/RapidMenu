# Agent guide

RapidMenu is a small CLI tool that turns TOML config files into menus for a runner like rofi, dmenu or bemenu: it parses the config, prints the action names, pipes them into the runner and executes the command of whatever the user picks. It can also emit a tiny bash wrapper script (`-b`) so a config can be launched as its own command. Written in C++26 on CMake and Ninja, parses TOML with cpptoml (tomlplusplus is a declared dep too). Packaged as a deb, an rpm and an Arch `PKGBUILD`. Contributors: DRAGONTOS, maukkis.

Cross-session handoff notes for this project live in `~/.local/share/agents/rapidmenu-handoff.md`. Read that file first when resuming after a context reset, and update it when state changes (commits, queue changes, new gotchas).

## Architecture

| Part | Where | Notes |
|---|---|---|
| Entry and arg parsing | `src/main.cpp` | Loops over argv, dispatches flags: `-c` config (exactly one arg), `-b` build executable (exactly two), `-h` help (stdout, exit 0). Fills the `cmd` struct (from `Strings.hpp`) and calls `config` or `executable`. Unknown flags print USAGE and return 1; a bare run creates the config dir when missing, else prints USAGE. |
| UI strings and shared types | `src/includes/Strings.hpp` | `USAGE` (short, errors) and `HELP` (fuller, `-h`, stdout) raw string literals, the `cmd` struct and the `invalidvalue` string. Keep them in sync with the flags in `main.cpp`. |
| Config runner | `src/config.cpp` | `setupconfigdir` creates `~/.config/RapidMenu` when missing (`-c` exits after a first-run setup; a bare run creates it and exits 0). `config` parses the TOML with cpptoml once, builds the newline-joined `namesList` (in **reversed** table order — intentional, documented in the code), builds the runner command as a shell string (`printf '%s' <quoted names> | <rcommand> <quoted rname> <rtheme>`) and hands command + parsed table to `runnercmd`. `runnercmd` popen's it, matches the picked name against the config's tables and runs the matching `command` through `system`, printing the `description` first. |
| Executable builder | `src/executable.cpp` | `executable` makes sure `~/.local/bin` exists, validates the config file, prompts on overwrite (`y/N` loops to a new name, both prompt reads checked against EOF) and writes a two-line bash wrapper (`RapidMenu -c '<config>'`, shellquoted) into `~/.local/bin/<exename>` with owner+group exec bits. |

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
