# Agent guide

RapidMenu is a small CLI tool that turns TOML config files into menus for a runner like rofi, dmenu or bemenu: it parses the config, prints the action names, pipes them into the runner and executes the command of whatever the user picks. It can also emit a tiny bash wrapper script (`-b`) so a config can be launched as its own command. Written in C++26 on CMake and Ninja, parses TOML with the vendored cpptoml (`external/`). Packaged as a deb, an rpm and an Arch `PKGBUILD`. Contributors: DRAGONTOS, maukkis.

Cross-session handoff notes for this project live in `~/.local/share/agents/rapidmenu-handoff.md`. Read that file first when resuming after a context reset, and update it when state changes (commits, queue changes, new gotchas).

## Architecture

| Part | Where | Notes |
|---|---|---|
| Entry and arg parsing | `src/main.cpp` | Loops over argv, dispatches flags: `-c` config (exactly one arg), `-b` build executable (exactly two), `-h` help (stdout, exit 0). Fills the `cmd` struct (from `Strings.hpp`) and calls `config` or `executable`. Unknown flags print USAGE and return 1; a bare run creates the config dir when missing, else prints USAGE. |
| UI strings and shared types | `src/includes/Strings.hpp` | `USAGE` (short, errors) and `HELP` (fuller, `-h`, stdout) raw string literals, the `cmd` struct and the `invalidvalue` string. Keep them in sync with the flags in `main.cpp`. |
| Config runner | `src/config.cpp` | `setupconfigdir` creates `~/.config/RapidMenu` when missing (`-c` exits after a first-run setup; a bare run creates it and exits 0). `config` parses the TOML with cpptoml once, builds the newline-joined `namesList` (in **reversed** table order — intentional, documented in the code), builds the runner command as a shell string (`printf '%s' <quoted names> | <rcommand> <quoted rname> <rtheme>`) and hands command + parsed table to `runnercmd`. `runnercmd` popen's it, matches the picked name against the config's tables and runs the matching `command` through `system`, printing the `description` first. |
| Executable builder | `src/executable.cpp` | `executable` makes sure `~/.local/bin` exists, validates the config file, prompts on overwrite (`y/N` loops to a new name, both prompt reads checked against EOF) and writes a two-line bash wrapper (`RapidMenu -c '<config>'`, shellquoted) into `~/.local/bin/<exename>` with owner+group exec bits. |
| Version | `props.json` | Single `"version"` key, read by CMake at configure time and by the PKGBUILD's `pkgver()`. Bump it for every release, and update the `line-count` file (cloc over `src/`) in the same commit. |

## Build and install

```sh
make all
sudo make install
```

`make all` clears `build/` and configures with Ninja, building Release with `-O2`. Packaging:

```sh
cd build && cpack -G DEB   # or RPM
makepkg -si PKGBUILD       # Arch
```

`.github/workflows/builds.yml` builds master and tags on ubuntu-24.04 with g++-14 and publishes the binary, a deb and an rpm to the releases: a rolling `latest` prerelease replaced on every master push, and a pinned release per `v*` tag whose notes list the commits since the previous tag. cpptoml is vendored in `external/`, so the runner needs no packages beyond ninja and g++-14. It needs no secrets beyond the workflow token.

| Command | Use |
|---|---|
| `make all` | Fresh cmake configure + build into `build/RapidMenu`. |
| `RapidMenu` | Bare run: sets up `~/.config/RapidMenu` if missing, else prints usage. |
| `RapidMenu -c <config>` | Parse the config, show the menu through its `[runner]` and run the picked action. |
| `RapidMenu -b <config> <exename>` | Write an executable wrapper for the config into `~/.local/bin/<exename>`. |
| `RapidMenu -h` | Help (stdout, exit 0). |

Every config needs a `[runner]` table (`rname`, `rtheme`, `rcommand` — the last defaults to `rofi -dmenu -p`); each other table is an action with `names`, optional `description` and `command`.

## Hard rules

- Never push (git or a release) without the owner's explicit approval.
- Never run `sudo` or `make install` on your own; building and running the binary from `build/` is fine.
- GPLv3, keep the LICENSE.
- Commit subjects are lowercase with a type prefix and no body: `fix:`, `added:`, `bump:`, `cleanup:`, `major:`, `pkgbuild:`. The repo also has older `pull:`/`core:` merges — don't reuse those.
- The version in `props.json` is the single source of truth; bump it and `line-count` together.
- **Every tagged release shows what changed**: tag *after* the fixes/changes land (move the tag with `git tag -f` + force-push if fixes come after tagging), and the release notes list the changes — the builds workflow generates them from the commits since the previous tag. Never publish a release with stale notes or a stale tag.
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

- cpptoml is vendored in `external/` (single header + MIT license file, by Chase Geigle) and found through the `external/` include dir — no system package, no pkg-config file, nothing linked. Don't `#include` system cpptoml; if the vendored copy ever needs updating, refresh the header and its license file together.
- The runner command is a single concatenated shell string passed through `popen`, and action commands go through `system`. `namesList` and `rname` are `shellquote`d and names go through `printf '%s'` (a `'` or `%` in a name is safe), but `rcommand` and `rtheme` stay raw on purpose — they are shell text by design, so config values there are still effectively shell-executed. Don't "simplify" by trusting them.
- `config` `exit(1)`s after creating the config dir, so a first `-c` run only sets the dir up and the user must invoke again; a bare run creates it and exits 0.
- Success paths return 0 since the 2026-11-18 fixes: `-h`, `-c`, `-b` and `executable`'s wrapper write; failure paths (bad config, no match, failed action command, unknown flag) return 1.
- `main.cpp` arg parsing: each flag branch checks `argc` itself (`-c` wants exactly one arg, `-b` exactly two); unknown flags print USAGE and return 1. Watch these checks when adding flags or reordering args.
