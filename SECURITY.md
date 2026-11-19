# Security Policy

## Supported Versions

The last 2 releases.

## Hardening Status and Untrusted Input

See the [Security Advisory chapter of the manual](doc/LibreDWG.texi)
(`@ref{Security Advisory}`; rendered at
https://www.gnu.org/software/libredwg/manual/LibreDWG.html#Security-Advisory):
LibreDWG is a C parser with a huge attack vector, constantly fuzzed and
AI-checked, but not hardened enough to accept untrusted input in
non-hardened environments, e.g. WASM converters on the open web or JSON
upload endpoints.

## Reporting a Vulnerability

Report a vulnerability by email to the maintainer. Or, preferred, via the WebUI
https://github.com/LibreDWG/libredwg/security/advisories
which can generate a CVE automatically.
