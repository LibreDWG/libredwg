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

# Builds

Convenient build dirs are

- `.build-asan` to check OOB and leak errors. It is very slow though.
- `.build-tcc` to check any logical errors. It is very fast.

## Tests

- Prefer `make check` for validating changes.
- When adding features/bugfixes, include or update testcases when reasonable.
- If adequate test coverage is not possible, document why in the PR/commit message
  (mirrors `CONTRIBUTING`).
- ODA roundtrips are checked with `../json-check dwgfile` and `../dxf-check dwgfile`
  ODAFileConverter Command Line Format is:
  - Quoted Input Folder
  - Quoted Output Folder (must be different)
  - Output_version {"ACAD9","ACAD10","ACAD12","ACAD13","ACAD14","ACAD2000","ACAD2004","ACAD2007","ACAD2010","ACAD2013","ACAD2018"}
  - Output File type {"DWG","DXF","DXB"}
  - Recurse Input Folder {"0","1"}
  - Audit each file {"0","1"}
  [optional] Input files filter
    (default: "*.DWG;*.DXF")
  We set the first arg to the path of the input file, the second arg to some other path,
  Recurse to 0, Audit to 1 and the input file to the basename.

Best is to use and fix just dxf-check. json roundtrips are only helpful for <=r2000 files.
With dxf-check the original read is `.log`
The encoder log is `.dxf.log`
The resulting encoded file read is `.log.new`
You only need to read these logs, not stdout.
All the shell helpers, like log or dxf-check do not write to stdout, inspect the log files instead.
With dxf-check changed sources are automatically rebuilt, so no need call make.

## Formatting / linting

- The project formats C sources with `clang-format`.
- Use the repository helper script (see `HACKING`):

```sh
build-aux/clang-format.sh changed-file
```

Do **not** run formatting across unrelated files. If the formatting cannot
be fixed, e.g. in macros, surround the macro with `// clang-format off` and
`// clang-format on` markers. clang-format is not perfect.

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
For Github issues reference it like `GH #NNNN`.

## Legal

This package is owned by the FSF.

- Significant contributions (roughly > 15 lines of code or docs) may require FSF
  copyright assignment or an appropriate disclaimer.
- See `CONTRIBUTING` and the GNU guidance linked there.

## Agent-specific safety rules

- Before making write operations, inspect `git status` and avoid clobbering local
  uncommitted work.
- Do not delete/clean untracked files (e.g. `.build*`, local archives, patches)
  unless explicitly requested.
- Prefer incremental edits over rewriting entire files.
