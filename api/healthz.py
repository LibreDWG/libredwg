"""
GET /api/healthz — proves whether this Vercel Python Function can execute the
LibreDWG `dwg2dxf` binary built from source during the Vercel build.

Returns ONLY a redacted shape:
  {"ok": true,  "converter": "available"}    (HTTP 200)  — `dwg2dxf --version` exited 0
  {"ok": false, "converter": "unavailable"}  (HTTP 503)  — anything else

Never exposes command output, stderr, file paths, environment variables, or
secrets. This is a feasibility spike — no /convert, no app integration.
"""

import json
import os
import shutil
import subprocess  # nosec B404 - fixed argv, no shell, bounded timeout
import tempfile
from http.server import BaseHTTPRequestHandler

# The binary is staged next to this file under bin/ by build.sh and bundled via
# vercel.json functions.includeFiles.
_BUNDLED_BIN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "bin", "dwg2dxf")


def _converter_available():
    """True iff the bundled dwg2dxf executes and `--version` exits 0."""
    if not os.path.exists(_BUNDLED_BIN):
        return False
    # The function filesystem may be read-only or drop the execute bit, so copy
    # the binary to a writable temp dir and mark it executable before exec.
    tmpdir = tempfile.mkdtemp(prefix="dwg-")
    try:
        runnable = os.path.join(tmpdir, "dwg2dxf")
        shutil.copy2(_BUNDLED_BIN, runnable)
        os.chmod(runnable, 0o755)
        env = dict(os.environ)
        # If the binary is not fully static, let the loader find a co-bundled
        # libredwg.so next to the original binary.
        env["LD_LIBRARY_PATH"] = (
            os.path.dirname(_BUNDLED_BIN) + ":" + env.get("LD_LIBRARY_PATH", "")
        )
        proc = subprocess.run(  # nosec B603 - fixed argv, no shell
            [runnable, "--version"],
            stdin=subprocess.DEVNULL,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,  # never surface converter output
            timeout=5,
            env=env,
            check=False,
        )
        return proc.returncode == 0
    except Exception:
        return False
    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)


class handler(BaseHTTPRequestHandler):
    def do_GET(self):
        ok = _converter_available()
        body = json.dumps(
            {"ok": True, "converter": "available"}
            if ok
            else {"ok": False, "converter": "unavailable"}
        ).encode("utf-8")
        self.send_response(200 if ok else 503)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *args, **kwargs):
        # Suppress default request logging (could echo paths). Redacted by design.
        return
