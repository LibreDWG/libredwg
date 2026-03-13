# AGENTS.md

Guidance for automated coding agents (and humans) working in this repository.

LibreDWG is a GNU project (GPLv3+). Please follow the existing project
conventions in `README`, `CONTRIBUTING`, and `HACKING`.

## Scope / priorities

1. Keep changes minimal and targeted.
2. Prefer fixes that address root cause and add/adjust tests when feasible.
3. Avoid reformatting or mechanical churn unless explicitly requested.

## Repository overview

- C library and tools to read/write DWG.
- Primary build system: GNU Autotools (`autogen.sh`, `configure`, `make`).
- CMake also exists (`CMakeLists.txt`) but Autotools is the canonical path.

## Bootstrapping & build

If working from a git checkout, regenerate build files with:

```sh
sh ./autogen.sh
```

Then build & test via:

```sh
./configure
make
make check
```

Notes:
- The codebase expects a C99 compiler.
- `--enable-trace` and `--enable-release` are common configure flags (see `README`).

## Tests

- Prefer `make check` for validating changes.
- When adding features/bugfixes, include or update testcases when reasonable.
- If adequate test coverage is not possible, document why in the PR/commit message
  (mirrors `CONTRIBUTING`).

## Formatting / linting

- The project formats C sources with `clang-format`.
- Use the repository helper script (see `HACKING`):

```sh
bash build-aux/clang-format-all.sh src programs examples test
```

Do **not** run formatting across unrelated files.

## Generated files

Per `CONTRIBUTING`: do **not** include generated files in patches unless requested.
Instead, mention regeneration steps in the ChangeLog/commit message (e.g. "configure:
Regenerate.").

Common generated/derived artifacts include (non-exhaustive):
- `configure`, `Makefile.in`, `aclocal.m4`, `libtool`, `autom4te.cache/`
- build directories like `.build*`

## ChangeLog / commit message conventions

The repo contains a top-level `ChangeLog`, but day-to-day commits generally do not
edit it; release ChangeLogs are generated from commit logs (see `HACKING`).

When possible, write commit messages in GNU style:

- One-line summary
- Blank line
- Optional short discussion
- Per-file bullets for non-trivial changes

If referencing Savannah PRs, include text like `PR #NNNN`.

## Legal

This package is owned by the FSF.

- Significant contributions (roughly > 15 lines of code or docs) may require FSF
  copyright assignment or an appropriate disclaimer.
- See `CONTRIBUTING` and the GNU guidance linked there.

## Security

Follow `SECURITY.md` for vulnerability reporting (email the maintainer).

## Agent-specific safety rules

- Before making write operations, inspect `git status` and avoid clobbering local
  uncommitted work.
- Do not delete/clean untracked files (e.g. `.build*`, local archives, patches)
  unless explicitly requested.
- Prefer incremental edits over rewriting entire files.
