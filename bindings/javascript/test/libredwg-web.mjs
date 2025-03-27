var Zo = Object.defineProperty;
var Qo = (h, p, Y) => p in h ? Zo(h, p, { enumerable: !0, configurable: !0, writable: !0, value: Y }) : h[p] = Y;
var U = (h, p, Y) => Qo(h, typeof p != "symbol" ? p + "" : p, Y);
function ea(h) {
  return h && h.__esModule && Object.prototype.hasOwnProperty.call(h, "default") ? h.default : h;
}
function ra(h) {
  if (Object.prototype.hasOwnProperty.call(h, "__esModule")) return h;
  var p = h.default;
  if (typeof p == "function") {
    var Y = function O() {
      return this instanceof O ? Reflect.construct(p, arguments, this.constructor) : p.apply(this, arguments);
    };
    Y.prototype = p.prototype;
  } else Y = {};
  return Object.defineProperty(Y, "__esModule", { value: !0 }), Object.keys(h).forEach(function(O) {
    var R = Object.getOwnPropertyDescriptor(h, O);
    Object.defineProperty(Y, O, R.get ? R : {
      enumerable: !0,
      get: function() {
        return h[O];
      }
    });
  }), Y;
}
var dr = { exports: {} };
const ta = {}, na = /* @__PURE__ */ Object.freeze(/* @__PURE__ */ Object.defineProperty({
  __proto__: null,
  default: ta
}, Symbol.toStringTag, { value: "Module" })), at = /* @__PURE__ */ ra(na);
var it;
function oa() {
  return it || (it = 1, function(h, p) {
    var Y = (() => {
      var R;
      var O = typeof document < "u" ? (R = document.currentScript) == null ? void 0 : R.src : void 0;
      return typeof __filename < "u" && (O = O || __filename), async function(V = {}) {
        var J, u = V, vr, Ue, _t = new Promise((e, r) => {
          vr = e, Ue = r;
        }), lt = typeof window == "object", me = typeof WorkerGlobalScope < "u", ge = typeof process == "object" && typeof process.versions == "object" && typeof process.versions.node == "string" && process.type != "renderer", Tr = Object.assign({}, u), je = "./this.program", He = (e, r) => {
          throw r;
        }, B = "";
        function ut(e) {
          return u.locateFile ? u.locateFile(e, B) : B + e;
        }
        var Ae, fe;
        if (ge) {
          var Ve = at;
          B = __dirname + "/", fe = (e) => {
            e = Ye(e) ? new URL(e) : e;
            var r = Ve.readFileSync(e);
            return r;
          }, Ae = async (e, r = !0) => {
            e = Ye(e) ? new URL(e) : e;
            var t = Ve.readFileSync(e, r ? void 0 : "utf8");
            return t;
          }, !u.thisProgram && process.argv.length > 1 && (je = process.argv[1].replace(/\\/g, "/")), process.argv.slice(2), He = (e, r) => {
            throw process.exitCode = e, r;
          };
        } else (lt || me) && (me ? B = self.location.href : typeof document < "u" && document.currentScript && (B = document.currentScript.src), O && (B = O), B.startsWith("blob:") ? B = "" : B = B.slice(0, B.replace(/[?#].*/, "").lastIndexOf("/") + 1), me && (fe = (e) => {
          var r = new XMLHttpRequest();
          return r.open("GET", e, !1), r.responseType = "arraybuffer", r.send(null), new Uint8Array(r.response);
        }), Ae = async (e) => {
          if (Ye(e))
            return new Promise((t, n) => {
              var a = new XMLHttpRequest();
              a.open("GET", e, !0), a.responseType = "arraybuffer", a.onload = () => {
                if (a.status == 200 || a.status == 0 && a.response) {
                  t(a.response);
                  return;
                }
                n(a.status);
              }, a.onerror = n, a.send(null);
            });
          var r = await fetch(e, { credentials: "same-origin" });
          if (r.ok)
            return r.arrayBuffer();
          throw new Error(r.status + " : " + r.url);
        });
        var Ke = u.print || console.log.bind(console), Q = u.printErr || console.error.bind(console);
        Object.assign(u, Tr), Tr = null, u.arguments && u.arguments, u.thisProgram && (je = u.thisProgram);
        var ye = u.wasmBinary, we, Oe = !1, Ce, W, F, j, Ee, v, g, Xe, K, hr, ze, Ye = (e) => e.startsWith("file://");
        function pr() {
          var e = we.buffer;
          u.HEAP8 = W = new Int8Array(e), u.HEAP16 = j = new Int16Array(e), u.HEAPU8 = F = new Uint8Array(e), u.HEAPU16 = Ee = new Uint16Array(e), u.HEAP32 = v = new Int32Array(e), u.HEAPU32 = g = new Uint32Array(e), u.HEAPF32 = Xe = new Float32Array(e), u.HEAPF64 = ze = new Float64Array(e), u.HEAP64 = K = new BigInt64Array(e), u.HEAPU64 = hr = new BigUint64Array(e);
        }
        function ct() {
          if (u.preRun)
            for (typeof u.preRun == "function" && (u.preRun = [u.preRun]); u.preRun.length; )
              gt(u.preRun.shift());
          mr(Ar);
        }
        function ft() {
          !u.noFSInit && !o.initialized && o.init(), A.__wasm_call_ctors(), o.ignorePermissions = !1;
        }
        function Et() {
          if (u.postRun)
            for (typeof u.postRun == "function" && (u.postRun = [u.postRun]); u.postRun.length; )
              mt(u.postRun.shift());
          mr(gr);
        }
        var ee = 0, de = null;
        function Dr(e) {
          var r;
          ee++, (r = u.monitorRunDependencies) == null || r.call(u, ee);
        }
        function Je(e) {
          var t;
          if (ee--, (t = u.monitorRunDependencies) == null || t.call(u, ee), ee == 0 && de) {
            var r = de;
            de = null, r();
          }
        }
        function ve(e) {
          var t;
          (t = u.onAbort) == null || t.call(u, e), e = "Aborted(" + e + ")", Q(e), Oe = !0, e += ". Build with -sASSERTIONS for more info.";
          var r = new WebAssembly.RuntimeError(e);
          throw Ue(r), r;
        }
        var qe;
        function dt() {
          return ut("libredwg-web.wasm");
        }
        function vt(e) {
          if (e == qe && ye)
            return new Uint8Array(ye);
          if (fe)
            return fe(e);
          throw "both async and sync fetching of the wasm failed";
        }
        async function Tt(e) {
          if (!ye)
            try {
              var r = await Ae(e);
              return new Uint8Array(r);
            } catch {
            }
          return vt(e);
        }
        async function ht(e, r) {
          try {
            var t = await Tt(e), n = await WebAssembly.instantiate(t, r);
            return n;
          } catch (a) {
            Q(`failed to asynchronously prepare wasm: ${a}`), ve(a);
          }
        }
        async function pt(e, r, t) {
          if (!e && typeof WebAssembly.instantiateStreaming == "function" && !Ye(r) && !ge)
            try {
              var n = fetch(r, { credentials: "same-origin" }), a = await WebAssembly.instantiateStreaming(n, t);
              return a;
            } catch (i) {
              Q(`wasm streaming compile failed: ${i}`), Q("falling back to ArrayBuffer instantiation");
            }
          return ht(r, t);
        }
        function Dt() {
          return { env: nt, wasi_snapshot_preview1: nt };
        }
        async function Pt() {
          function e(i, s) {
            return A = i.exports, we = A.memory, pr(), Or = A.__indirect_function_table, Je(), A;
          }
          Dr();
          function r(i) {
            return e(i.instance);
          }
          var t = Dt();
          if (u.instantiateWasm)
            return new Promise((i, s) => {
              u.instantiateWasm(t, (_, l) => {
                e(_), i(_.exports);
              });
            });
          qe ?? (qe = dt());
          try {
            var n = await pt(ye, qe, t), a = r(n);
            return a;
          } catch (i) {
            return Ue(i), Promise.reject(i);
          }
        }
        class Pr {
          constructor(r) {
            U(this, "name", "ExitStatus");
            this.message = `Program terminated with exit(${r})`, this.status = r;
          }
        }
        var mr = (e) => {
          for (; e.length > 0; )
            e.shift()(u);
        }, gr = [], mt = (e) => gr.unshift(e), Ar = [], gt = (e) => Ar.unshift(e), yr = u.noExitRuntime || !0;
        function At(e, r, t = "i8") {
          switch (t.endsWith("*") && (t = "*"), t) {
            case "i1":
              W[e] = r;
              break;
            case "i8":
              W[e] = r;
              break;
            case "i16":
              j[e >> 1] = r;
              break;
            case "i32":
              v[e >> 2] = r;
              break;
            case "i64":
              K[e >> 3] = BigInt(r);
              break;
            case "float":
              Xe[e >> 2] = r;
              break;
            case "double":
              ze[e >> 3] = r;
              break;
            case "*":
              g[e >> 2] = r;
              break;
            default:
              ve(`invalid type for setValue: ${t}`);
          }
        }
        var yt = (e) => Xo(e), wt = () => Jo(), wr = typeof TextDecoder < "u" ? new TextDecoder() : void 0, ie = (e, r = 0, t = NaN) => {
          for (var n = r + t, a = r; e[a] && !(a >= n); ) ++a;
          if (a - r > 16 && e.buffer && wr)
            return wr.decode(e.subarray(r, a));
          for (var i = ""; r < a; ) {
            var s = e[r++];
            if (!(s & 128)) {
              i += String.fromCharCode(s);
              continue;
            }
            var _ = e[r++] & 63;
            if ((s & 224) == 192) {
              i += String.fromCharCode((s & 31) << 6 | _);
              continue;
            }
            var l = e[r++] & 63;
            if ((s & 240) == 224 ? s = (s & 15) << 12 | _ << 6 | l : s = (s & 7) << 18 | _ << 12 | l << 6 | e[r++] & 63, s < 65536)
              i += String.fromCharCode(s);
            else {
              var c = s - 65536;
              i += String.fromCharCode(55296 | c >> 10, 56320 | c & 1023);
            }
          }
          return i;
        }, re = (e, r) => e ? ie(F, e, r) : "", Ot = (e, r, t, n) => ve(`Assertion failed: ${re(e)}, at: ` + [r ? re(r) : "unknown filename", t, n ? re(n) : "unknown function"]), Re = [], Or, Cr = (e) => {
          var r = Re[e];
          return r || (e >= Re.length && (Re.length = e + 1), Re[e] = r = Or.get(e)), r;
        }, Ct = (e, r) => Cr(e)(r);
        class Yr {
          constructor(r) {
            this.excPtr = r, this.ptr = r - 24;
          }
          set_type(r) {
            g[this.ptr + 4 >> 2] = r;
          }
          get_type() {
            return g[this.ptr + 4 >> 2];
          }
          set_destructor(r) {
            g[this.ptr + 8 >> 2] = r;
          }
          get_destructor() {
            return g[this.ptr + 8 >> 2];
          }
          set_caught(r) {
            r = r ? 1 : 0, W[this.ptr + 12] = r;
          }
          get_caught() {
            return W[this.ptr + 12] != 0;
          }
          set_rethrown(r) {
            r = r ? 1 : 0, W[this.ptr + 13] = r;
          }
          get_rethrown() {
            return W[this.ptr + 13] != 0;
          }
          init(r, t) {
            this.set_adjusted_ptr(0), this.set_type(r), this.set_destructor(t);
          }
          set_adjusted_ptr(r) {
            g[this.ptr + 16 >> 2] = r;
          }
          get_adjusted_ptr() {
            return g[this.ptr + 16 >> 2];
          }
        }
        var Se = 0, Ze = 0, Yt = (e, r, t) => {
          var n = new Yr(e);
          throw n.init(r, t), Se = e, Ze++, Se;
        }, Ge = () => {
          var e = v[+S.varargs >> 2];
          return S.varargs += 4, e;
        }, se = Ge, w = { isAbs: (e) => e.charAt(0) === "/", splitPath: (e) => {
          var r = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
          return r.exec(e).slice(1);
        }, normalizeArray: (e, r) => {
          for (var t = 0, n = e.length - 1; n >= 0; n--) {
            var a = e[n];
            a === "." ? e.splice(n, 1) : a === ".." ? (e.splice(n, 1), t++) : t && (e.splice(n, 1), t--);
          }
          if (r)
            for (; t; t--)
              e.unshift("..");
          return e;
        }, normalize: (e) => {
          var r = w.isAbs(e), t = e.slice(-1) === "/";
          return e = w.normalizeArray(e.split("/").filter((n) => !!n), !r).join("/"), !e && !r && (e = "."), e && t && (e += "/"), (r ? "/" : "") + e;
        }, dirname: (e) => {
          var r = w.splitPath(e), t = r[0], n = r[1];
          return !t && !n ? "." : (n && (n = n.slice(0, -1)), t + n);
        }, basename: (e) => e && e.match(/([^\/]+|\/)\/*$/)[1], join: (...e) => w.normalize(e.join("/")), join2: (e, r) => w.normalize(e + "/" + r) }, Rt = () => {
          if (ge) {
            var e = at;
            return (r) => e.randomFillSync(r);
          }
          return (r) => crypto.getRandomValues(r);
        }, Rr = (e) => {
          (Rr = Rt())(e);
        }, _e = { resolve: (...e) => {
          for (var r = "", t = !1, n = e.length - 1; n >= -1 && !t; n--) {
            var a = n >= 0 ? e[n] : o.cwd();
            if (typeof a != "string")
              throw new TypeError("Arguments to path.resolve must be strings");
            if (!a)
              return "";
            r = a + "/" + r, t = w.isAbs(a);
          }
          return r = w.normalizeArray(r.split("/").filter((i) => !!i), !t).join("/"), (t ? "/" : "") + r || ".";
        }, relative: (e, r) => {
          e = _e.resolve(e).slice(1), r = _e.resolve(r).slice(1);
          function t(c) {
            for (var f = 0; f < c.length && c[f] === ""; f++)
              ;
            for (var E = c.length - 1; E >= 0 && c[E] === ""; E--)
              ;
            return f > E ? [] : c.slice(f, E - f + 1);
          }
          for (var n = t(e.split("/")), a = t(r.split("/")), i = Math.min(n.length, a.length), s = i, _ = 0; _ < i; _++)
            if (n[_] !== a[_]) {
              s = _;
              break;
            }
          for (var l = [], _ = s; _ < n.length; _++)
            l.push("..");
          return l = l.concat(a.slice(s)), l.join("/");
        } }, Qe = [], Te = (e) => {
          for (var r = 0, t = 0; t < e.length; ++t) {
            var n = e.charCodeAt(t);
            n <= 127 ? r++ : n <= 2047 ? r += 2 : n >= 55296 && n <= 57343 ? (r += 4, ++t) : r += 3;
          }
          return r;
        }, er = (e, r, t, n) => {
          if (!(n > 0)) return 0;
          for (var a = t, i = t + n - 1, s = 0; s < e.length; ++s) {
            var _ = e.charCodeAt(s);
            if (_ >= 55296 && _ <= 57343) {
              var l = e.charCodeAt(++s);
              _ = 65536 + ((_ & 1023) << 10) | l & 1023;
            }
            if (_ <= 127) {
              if (t >= i) break;
              r[t++] = _;
            } else if (_ <= 2047) {
              if (t + 1 >= i) break;
              r[t++] = 192 | _ >> 6, r[t++] = 128 | _ & 63;
            } else if (_ <= 65535) {
              if (t + 2 >= i) break;
              r[t++] = 224 | _ >> 12, r[t++] = 128 | _ >> 6 & 63, r[t++] = 128 | _ & 63;
            } else {
              if (t + 3 >= i) break;
              r[t++] = 240 | _ >> 18, r[t++] = 128 | _ >> 12 & 63, r[t++] = 128 | _ >> 6 & 63, r[t++] = 128 | _ & 63;
            }
          }
          return r[t] = 0, t - a;
        }, Sr = (e, r, t) => {
          var n = Te(e) + 1, a = new Array(n), i = er(e, a, 0, a.length);
          return a.length = i, a;
        }, St = () => {
          if (!Qe.length) {
            var e = null;
            if (ge) {
              var r = 256, t = Buffer.alloc(r), n = 0, a = process.stdin.fd;
              try {
                n = Ve.readSync(a, t, 0, r);
              } catch (i) {
                if (i.toString().includes("EOF")) n = 0;
                else throw i;
              }
              n > 0 && (e = t.slice(0, n).toString("utf-8"));
            } else typeof window < "u" && typeof window.prompt == "function" && (e = window.prompt("Input: "), e !== null && (e += `
`));
            if (!e)
              return null;
            Qe = Sr(e);
          }
          return Qe.shift();
        }, te = { ttys: [], init() {
        }, shutdown() {
        }, register(e, r) {
          te.ttys[e] = { input: [], output: [], ops: r }, o.registerDevice(e, te.stream_ops);
        }, stream_ops: { open(e) {
          var r = te.ttys[e.node.rdev];
          if (!r)
            throw new o.ErrnoError(43);
          e.tty = r, e.seekable = !1;
        }, close(e) {
          e.tty.ops.fsync(e.tty);
        }, fsync(e) {
          e.tty.ops.fsync(e.tty);
        }, read(e, r, t, n, a) {
          if (!e.tty || !e.tty.ops.get_char)
            throw new o.ErrnoError(60);
          for (var i = 0, s = 0; s < n; s++) {
            var _;
            try {
              _ = e.tty.ops.get_char(e.tty);
            } catch {
              throw new o.ErrnoError(29);
            }
            if (_ === void 0 && i === 0)
              throw new o.ErrnoError(6);
            if (_ == null) break;
            i++, r[t + s] = _;
          }
          return i && (e.node.atime = Date.now()), i;
        }, write(e, r, t, n, a) {
          if (!e.tty || !e.tty.ops.put_char)
            throw new o.ErrnoError(60);
          try {
            for (var i = 0; i < n; i++)
              e.tty.ops.put_char(e.tty, r[t + i]);
          } catch {
            throw new o.ErrnoError(29);
          }
          return n && (e.node.mtime = e.node.ctime = Date.now()), i;
        } }, default_tty_ops: { get_char(e) {
          return St();
        }, put_char(e, r) {
          r === null || r === 10 ? (Ke(ie(e.output)), e.output = []) : r != 0 && e.output.push(r);
        }, fsync(e) {
          var r;
          ((r = e.output) == null ? void 0 : r.length) > 0 && (Ke(ie(e.output)), e.output = []);
        }, ioctl_tcgets(e) {
          return { c_iflag: 25856, c_oflag: 5, c_cflag: 191, c_lflag: 35387, c_cc: [3, 28, 127, 21, 4, 0, 1, 0, 17, 19, 26, 0, 18, 15, 23, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] };
        }, ioctl_tcsets(e, r, t) {
          return 0;
        }, ioctl_tiocgwinsz(e) {
          return [24, 80];
        } }, default_tty1_ops: { put_char(e, r) {
          r === null || r === 10 ? (Q(ie(e.output)), e.output = []) : r != 0 && e.output.push(r);
        }, fsync(e) {
          var r;
          ((r = e.output) == null ? void 0 : r.length) > 0 && (Q(ie(e.output)), e.output = []);
        } } }, Gt = (e, r) => {
          F.fill(0, e, e + r);
        }, Gr = (e, r) => Math.ceil(e / r) * r, Wr = (e) => {
          e = Gr(e, 65536);
          var r = Ho(65536, e);
          return r && Gt(r, e), r;
        }, P = { ops_table: null, mount(e) {
          return P.createNode(null, "/", 16895, 0);
        }, createNode(e, r, t, n) {
          if (o.isBlkdev(t) || o.isFIFO(t))
            throw new o.ErrnoError(63);
          P.ops_table || (P.ops_table = { dir: { node: { getattr: P.node_ops.getattr, setattr: P.node_ops.setattr, lookup: P.node_ops.lookup, mknod: P.node_ops.mknod, rename: P.node_ops.rename, unlink: P.node_ops.unlink, rmdir: P.node_ops.rmdir, readdir: P.node_ops.readdir, symlink: P.node_ops.symlink }, stream: { llseek: P.stream_ops.llseek } }, file: { node: { getattr: P.node_ops.getattr, setattr: P.node_ops.setattr }, stream: { llseek: P.stream_ops.llseek, read: P.stream_ops.read, write: P.stream_ops.write, allocate: P.stream_ops.allocate, mmap: P.stream_ops.mmap, msync: P.stream_ops.msync } }, link: { node: { getattr: P.node_ops.getattr, setattr: P.node_ops.setattr, readlink: P.node_ops.readlink }, stream: {} }, chrdev: { node: { getattr: P.node_ops.getattr, setattr: P.node_ops.setattr }, stream: o.chrdev_stream_ops } });
          var a = o.createNode(e, r, t, n);
          return o.isDir(a.mode) ? (a.node_ops = P.ops_table.dir.node, a.stream_ops = P.ops_table.dir.stream, a.contents = {}) : o.isFile(a.mode) ? (a.node_ops = P.ops_table.file.node, a.stream_ops = P.ops_table.file.stream, a.usedBytes = 0, a.contents = null) : o.isLink(a.mode) ? (a.node_ops = P.ops_table.link.node, a.stream_ops = P.ops_table.link.stream) : o.isChrdev(a.mode) && (a.node_ops = P.ops_table.chrdev.node, a.stream_ops = P.ops_table.chrdev.stream), a.atime = a.mtime = a.ctime = Date.now(), e && (e.contents[r] = a, e.atime = e.mtime = e.ctime = a.atime), a;
        }, getFileDataAsTypedArray(e) {
          return e.contents ? e.contents.subarray ? e.contents.subarray(0, e.usedBytes) : new Uint8Array(e.contents) : new Uint8Array(0);
        }, expandFileStorage(e, r) {
          var t = e.contents ? e.contents.length : 0;
          if (!(t >= r)) {
            var n = 1024 * 1024;
            r = Math.max(r, t * (t < n ? 2 : 1.125) >>> 0), t != 0 && (r = Math.max(r, 256));
            var a = e.contents;
            e.contents = new Uint8Array(r), e.usedBytes > 0 && e.contents.set(a.subarray(0, e.usedBytes), 0);
          }
        }, resizeFileStorage(e, r) {
          if (e.usedBytes != r)
            if (r == 0)
              e.contents = null, e.usedBytes = 0;
            else {
              var t = e.contents;
              e.contents = new Uint8Array(r), t && e.contents.set(t.subarray(0, Math.min(r, e.usedBytes))), e.usedBytes = r;
            }
        }, node_ops: { getattr(e) {
          var r = {};
          return r.dev = o.isChrdev(e.mode) ? e.id : 1, r.ino = e.id, r.mode = e.mode, r.nlink = 1, r.uid = 0, r.gid = 0, r.rdev = e.rdev, o.isDir(e.mode) ? r.size = 4096 : o.isFile(e.mode) ? r.size = e.usedBytes : o.isLink(e.mode) ? r.size = e.link.length : r.size = 0, r.atime = new Date(e.atime), r.mtime = new Date(e.mtime), r.ctime = new Date(e.ctime), r.blksize = 4096, r.blocks = Math.ceil(r.size / r.blksize), r;
        }, setattr(e, r) {
          for (const t of ["mode", "atime", "mtime", "ctime"])
            r[t] != null && (e[t] = r[t]);
          r.size !== void 0 && P.resizeFileStorage(e, r.size);
        }, lookup(e, r) {
          throw P.doesNotExistError;
        }, mknod(e, r, t, n) {
          return P.createNode(e, r, t, n);
        }, rename(e, r, t) {
          var n;
          try {
            n = o.lookupNode(r, t);
          } catch {
          }
          if (n) {
            if (o.isDir(e.mode))
              for (var a in n.contents)
                throw new o.ErrnoError(55);
            o.hashRemoveNode(n);
          }
          delete e.parent.contents[e.name], r.contents[t] = e, e.name = t, r.ctime = r.mtime = e.parent.ctime = e.parent.mtime = Date.now();
        }, unlink(e, r) {
          delete e.contents[r], e.ctime = e.mtime = Date.now();
        }, rmdir(e, r) {
          var t = o.lookupNode(e, r);
          for (var n in t.contents)
            throw new o.ErrnoError(55);
          delete e.contents[r], e.ctime = e.mtime = Date.now();
        }, readdir(e) {
          return [".", "..", ...Object.keys(e.contents)];
        }, symlink(e, r, t) {
          var n = P.createNode(e, r, 41471, 0);
          return n.link = t, n;
        }, readlink(e) {
          if (!o.isLink(e.mode))
            throw new o.ErrnoError(28);
          return e.link;
        } }, stream_ops: { read(e, r, t, n, a) {
          var i = e.node.contents;
          if (a >= e.node.usedBytes) return 0;
          var s = Math.min(e.node.usedBytes - a, n);
          if (s > 8 && i.subarray)
            r.set(i.subarray(a, a + s), t);
          else
            for (var _ = 0; _ < s; _++) r[t + _] = i[a + _];
          return s;
        }, write(e, r, t, n, a, i) {
          if (r.buffer === W.buffer && (i = !1), !n) return 0;
          var s = e.node;
          if (s.mtime = s.ctime = Date.now(), r.subarray && (!s.contents || s.contents.subarray)) {
            if (i)
              return s.contents = r.subarray(t, t + n), s.usedBytes = n, n;
            if (s.usedBytes === 0 && a === 0)
              return s.contents = r.slice(t, t + n), s.usedBytes = n, n;
            if (a + n <= s.usedBytes)
              return s.contents.set(r.subarray(t, t + n), a), n;
          }
          if (P.expandFileStorage(s, a + n), s.contents.subarray && r.subarray)
            s.contents.set(r.subarray(t, t + n), a);
          else
            for (var _ = 0; _ < n; _++)
              s.contents[a + _] = r[t + _];
          return s.usedBytes = Math.max(s.usedBytes, a + n), n;
        }, llseek(e, r, t) {
          var n = r;
          if (t === 1 ? n += e.position : t === 2 && o.isFile(e.node.mode) && (n += e.node.usedBytes), n < 0)
            throw new o.ErrnoError(28);
          return n;
        }, allocate(e, r, t) {
          P.expandFileStorage(e.node, r + t), e.node.usedBytes = Math.max(e.node.usedBytes, r + t);
        }, mmap(e, r, t, n, a) {
          if (!o.isFile(e.node.mode))
            throw new o.ErrnoError(43);
          var i, s, _ = e.node.contents;
          if (!(a & 2) && _ && _.buffer === W.buffer)
            s = !1, i = _.byteOffset;
          else {
            if (s = !0, i = Wr(r), !i)
              throw new o.ErrnoError(48);
            _ && ((t > 0 || t + r < _.length) && (_.subarray ? _ = _.subarray(t, t + r) : _ = Array.prototype.slice.call(_, t, t + r)), W.set(_, i));
          }
          return { ptr: i, allocated: s };
        }, msync(e, r, t, n, a) {
          return P.stream_ops.write(e, r, 0, n, t, !1), 0;
        } } }, Wt = async (e) => {
          var r = await Ae(e);
          return new Uint8Array(r);
        }, Nt = (e, r, t, n, a, i) => {
          o.createDataFile(e, r, t, n, a, i);
        }, It = u.preloadPlugins || [], Lt = (e, r, t, n) => {
          typeof Browser < "u" && Browser.init();
          var a = !1;
          return It.forEach((i) => {
            a || i.canHandle(r) && (i.handle(e, r, t, n), a = !0);
          }), a;
        }, bt = (e, r, t, n, a, i, s, _, l, c) => {
          var f = r ? _e.resolve(w.join2(e, r)) : e;
          function E(D) {
            function d(T) {
              c == null || c(), _ || Nt(e, r, T, n, a, l), i == null || i(), Je();
            }
            Lt(D, f, d, () => {
              s == null || s(), Je();
            }) || d(D);
          }
          Dr(), typeof t == "string" ? Wt(t).then(E, s) : E(t);
        }, kt = (e) => {
          var r = { r: 0, "r+": 2, w: 577, "w+": 578, a: 1089, "a+": 1090 }, t = r[e];
          if (typeof t > "u")
            throw new Error(`Unknown file open mode: ${e}`);
          return t;
        }, rr = (e, r) => {
          var t = 0;
          return e && (t |= 365), r && (t |= 146), t;
        }, o = { root: null, mounts: [], devices: {}, streams: [], nextInode: 1, nameTable: null, currentPath: "/", initialized: !1, ignorePermissions: !0, ErrnoError: class {
          constructor(e) {
            U(this, "name", "ErrnoError");
            this.errno = e;
          }
        }, filesystems: null, syncFSRequests: 0, readFiles: {}, FSStream: class {
          constructor() {
            U(this, "shared", {});
          }
          get object() {
            return this.node;
          }
          set object(e) {
            this.node = e;
          }
          get isRead() {
            return (this.flags & 2097155) !== 1;
          }
          get isWrite() {
            return (this.flags & 2097155) !== 0;
          }
          get isAppend() {
            return this.flags & 1024;
          }
          get flags() {
            return this.shared.flags;
          }
          set flags(e) {
            this.shared.flags = e;
          }
          get position() {
            return this.shared.position;
          }
          set position(e) {
            this.shared.position = e;
          }
        }, FSNode: class {
          constructor(e, r, t, n) {
            U(this, "node_ops", {});
            U(this, "stream_ops", {});
            U(this, "readMode", 365);
            U(this, "writeMode", 146);
            U(this, "mounted", null);
            e || (e = this), this.parent = e, this.mount = e.mount, this.id = o.nextInode++, this.name = r, this.mode = t, this.rdev = n, this.atime = this.mtime = this.ctime = Date.now();
          }
          get read() {
            return (this.mode & this.readMode) === this.readMode;
          }
          set read(e) {
            e ? this.mode |= this.readMode : this.mode &= ~this.readMode;
          }
          get write() {
            return (this.mode & this.writeMode) === this.writeMode;
          }
          set write(e) {
            e ? this.mode |= this.writeMode : this.mode &= ~this.writeMode;
          }
          get isFolder() {
            return o.isDir(this.mode);
          }
          get isDevice() {
            return o.isChrdev(this.mode);
          }
        }, lookupPath(e, r = {}) {
          if (!e)
            throw new o.ErrnoError(44);
          r.follow_mount ?? (r.follow_mount = !0), w.isAbs(e) || (e = o.cwd() + "/" + e);
          e: for (var t = 0; t < 40; t++) {
            for (var n = e.split("/").filter((c) => !!c), a = o.root, i = "/", s = 0; s < n.length; s++) {
              var _ = s === n.length - 1;
              if (_ && r.parent)
                break;
              if (n[s] !== ".") {
                if (n[s] === "..") {
                  i = w.dirname(i), a = a.parent;
                  continue;
                }
                i = w.join2(i, n[s]);
                try {
                  a = o.lookupNode(a, n[s]);
                } catch (c) {
                  if ((c == null ? void 0 : c.errno) === 44 && _ && r.noent_okay)
                    return { path: i };
                  throw c;
                }
                if (o.isMountpoint(a) && (!_ || r.follow_mount) && (a = a.mounted.root), o.isLink(a.mode) && (!_ || r.follow)) {
                  if (!a.node_ops.readlink)
                    throw new o.ErrnoError(52);
                  var l = a.node_ops.readlink(a);
                  w.isAbs(l) || (l = w.dirname(i) + "/" + l), e = l + "/" + n.slice(s + 1).join("/");
                  continue e;
                }
              }
            }
            return { path: i, node: a };
          }
          throw new o.ErrnoError(32);
        }, getPath(e) {
          for (var r; ; ) {
            if (o.isRoot(e)) {
              var t = e.mount.mountpoint;
              return r ? t[t.length - 1] !== "/" ? `${t}/${r}` : t + r : t;
            }
            r = r ? `${e.name}/${r}` : e.name, e = e.parent;
          }
        }, hashName(e, r) {
          for (var t = 0, n = 0; n < r.length; n++)
            t = (t << 5) - t + r.charCodeAt(n) | 0;
          return (e + t >>> 0) % o.nameTable.length;
        }, hashAddNode(e) {
          var r = o.hashName(e.parent.id, e.name);
          e.name_next = o.nameTable[r], o.nameTable[r] = e;
        }, hashRemoveNode(e) {
          var r = o.hashName(e.parent.id, e.name);
          if (o.nameTable[r] === e)
            o.nameTable[r] = e.name_next;
          else
            for (var t = o.nameTable[r]; t; ) {
              if (t.name_next === e) {
                t.name_next = e.name_next;
                break;
              }
              t = t.name_next;
            }
        }, lookupNode(e, r) {
          var t = o.mayLookup(e);
          if (t)
            throw new o.ErrnoError(t);
          for (var n = o.hashName(e.id, r), a = o.nameTable[n]; a; a = a.name_next) {
            var i = a.name;
            if (a.parent.id === e.id && i === r)
              return a;
          }
          return o.lookup(e, r);
        }, createNode(e, r, t, n) {
          var a = new o.FSNode(e, r, t, n);
          return o.hashAddNode(a), a;
        }, destroyNode(e) {
          o.hashRemoveNode(e);
        }, isRoot(e) {
          return e === e.parent;
        }, isMountpoint(e) {
          return !!e.mounted;
        }, isFile(e) {
          return (e & 61440) === 32768;
        }, isDir(e) {
          return (e & 61440) === 16384;
        }, isLink(e) {
          return (e & 61440) === 40960;
        }, isChrdev(e) {
          return (e & 61440) === 8192;
        }, isBlkdev(e) {
          return (e & 61440) === 24576;
        }, isFIFO(e) {
          return (e & 61440) === 4096;
        }, isSocket(e) {
          return (e & 49152) === 49152;
        }, flagsToPermissionString(e) {
          var r = ["r", "w", "rw"][e & 3];
          return e & 512 && (r += "w"), r;
        }, nodePermissions(e, r) {
          return o.ignorePermissions ? 0 : r.includes("r") && !(e.mode & 292) || r.includes("w") && !(e.mode & 146) || r.includes("x") && !(e.mode & 73) ? 2 : 0;
        }, mayLookup(e) {
          if (!o.isDir(e.mode)) return 54;
          var r = o.nodePermissions(e, "x");
          return r || (e.node_ops.lookup ? 0 : 2);
        }, mayCreate(e, r) {
          if (!o.isDir(e.mode))
            return 54;
          try {
            var t = o.lookupNode(e, r);
            return 20;
          } catch {
          }
          return o.nodePermissions(e, "wx");
        }, mayDelete(e, r, t) {
          var n;
          try {
            n = o.lookupNode(e, r);
          } catch (i) {
            return i.errno;
          }
          var a = o.nodePermissions(e, "wx");
          if (a)
            return a;
          if (t) {
            if (!o.isDir(n.mode))
              return 54;
            if (o.isRoot(n) || o.getPath(n) === o.cwd())
              return 10;
          } else if (o.isDir(n.mode))
            return 31;
          return 0;
        }, mayOpen(e, r) {
          return e ? o.isLink(e.mode) ? 32 : o.isDir(e.mode) && (o.flagsToPermissionString(r) !== "r" || r & 576) ? 31 : o.nodePermissions(e, o.flagsToPermissionString(r)) : 44;
        }, checkOpExists(e, r) {
          if (!e)
            throw new o.ErrnoError(r);
          return e;
        }, MAX_OPEN_FDS: 4096, nextfd() {
          for (var e = 0; e <= o.MAX_OPEN_FDS; e++)
            if (!o.streams[e])
              return e;
          throw new o.ErrnoError(33);
        }, getStreamChecked(e) {
          var r = o.getStream(e);
          if (!r)
            throw new o.ErrnoError(8);
          return r;
        }, getStream: (e) => o.streams[e], createStream(e, r = -1) {
          return e = Object.assign(new o.FSStream(), e), r == -1 && (r = o.nextfd()), e.fd = r, o.streams[r] = e, e;
        }, closeStream(e) {
          o.streams[e] = null;
        }, dupStream(e, r = -1) {
          var n, a;
          var t = o.createStream(e, r);
          return (a = (n = t.stream_ops) == null ? void 0 : n.dup) == null || a.call(n, t), t;
        }, doSetAttr(e, r, t) {
          var n = e == null ? void 0 : e.stream_ops.setattr, a = n ? e : r;
          n ?? (n = r.node_ops.setattr), o.checkOpExists(n, 63), n(a, t);
        }, chrdev_stream_ops: { open(e) {
          var t, n;
          var r = o.getDevice(e.node.rdev);
          e.stream_ops = r.stream_ops, (n = (t = e.stream_ops).open) == null || n.call(t, e);
        }, llseek() {
          throw new o.ErrnoError(70);
        } }, major: (e) => e >> 8, minor: (e) => e & 255, makedev: (e, r) => e << 8 | r, registerDevice(e, r) {
          o.devices[e] = { stream_ops: r };
        }, getDevice: (e) => o.devices[e], getMounts(e) {
          for (var r = [], t = [e]; t.length; ) {
            var n = t.pop();
            r.push(n), t.push(...n.mounts);
          }
          return r;
        }, syncfs(e, r) {
          typeof e == "function" && (r = e, e = !1), o.syncFSRequests++, o.syncFSRequests > 1 && Q(`warning: ${o.syncFSRequests} FS.syncfs operations in flight at once, probably just doing extra work`);
          var t = o.getMounts(o.root.mount), n = 0;
          function a(s) {
            return o.syncFSRequests--, r(s);
          }
          function i(s) {
            if (s)
              return i.errored ? void 0 : (i.errored = !0, a(s));
            ++n >= t.length && a(null);
          }
          t.forEach((s) => {
            if (!s.type.syncfs)
              return i(null);
            s.type.syncfs(s, e, i);
          });
        }, mount(e, r, t) {
          var n = t === "/", a = !t, i;
          if (n && o.root)
            throw new o.ErrnoError(10);
          if (!n && !a) {
            var s = o.lookupPath(t, { follow_mount: !1 });
            if (t = s.path, i = s.node, o.isMountpoint(i))
              throw new o.ErrnoError(10);
            if (!o.isDir(i.mode))
              throw new o.ErrnoError(54);
          }
          var _ = { type: e, opts: r, mountpoint: t, mounts: [] }, l = e.mount(_);
          return l.mount = _, _.root = l, n ? o.root = l : i && (i.mounted = _, i.mount && i.mount.mounts.push(_)), l;
        }, unmount(e) {
          var r = o.lookupPath(e, { follow_mount: !1 });
          if (!o.isMountpoint(r.node))
            throw new o.ErrnoError(28);
          var t = r.node, n = t.mounted, a = o.getMounts(n);
          Object.keys(o.nameTable).forEach((s) => {
            for (var _ = o.nameTable[s]; _; ) {
              var l = _.name_next;
              a.includes(_.mount) && o.destroyNode(_), _ = l;
            }
          }), t.mounted = null;
          var i = t.mount.mounts.indexOf(n);
          t.mount.mounts.splice(i, 1);
        }, lookup(e, r) {
          return e.node_ops.lookup(e, r);
        }, mknod(e, r, t) {
          var n = o.lookupPath(e, { parent: !0 }), a = n.node, i = w.basename(e);
          if (!i)
            throw new o.ErrnoError(28);
          if (i === "." || i === "..")
            throw new o.ErrnoError(20);
          var s = o.mayCreate(a, i);
          if (s)
            throw new o.ErrnoError(s);
          if (!a.node_ops.mknod)
            throw new o.ErrnoError(63);
          return a.node_ops.mknod(a, i, r, t);
        }, statfs(e) {
          return o.statfsNode(o.lookupPath(e, { follow: !0 }).node);
        }, statfsStream(e) {
          return o.statfsNode(e.node);
        }, statfsNode(e) {
          var r = { bsize: 4096, frsize: 4096, blocks: 1e6, bfree: 5e5, bavail: 5e5, files: o.nextInode, ffree: o.nextInode - 1, fsid: 42, flags: 2, namelen: 255 };
          return e.node_ops.statfs && Object.assign(r, e.node_ops.statfs(e.mount.opts.root)), r;
        }, create(e, r = 438) {
          return r &= 4095, r |= 32768, o.mknod(e, r, 0);
        }, mkdir(e, r = 511) {
          return r &= 1023, r |= 16384, o.mknod(e, r, 0);
        }, mkdirTree(e, r) {
          for (var t = e.split("/"), n = "", a = 0; a < t.length; ++a)
            if (t[a]) {
              n += "/" + t[a];
              try {
                o.mkdir(n, r);
              } catch (i) {
                if (i.errno != 20) throw i;
              }
            }
        }, mkdev(e, r, t) {
          return typeof t > "u" && (t = r, r = 438), r |= 8192, o.mknod(e, r, t);
        }, symlink(e, r) {
          if (!_e.resolve(e))
            throw new o.ErrnoError(44);
          var t = o.lookupPath(r, { parent: !0 }), n = t.node;
          if (!n)
            throw new o.ErrnoError(44);
          var a = w.basename(r), i = o.mayCreate(n, a);
          if (i)
            throw new o.ErrnoError(i);
          if (!n.node_ops.symlink)
            throw new o.ErrnoError(63);
          return n.node_ops.symlink(n, a, e);
        }, rename(e, r) {
          var t = w.dirname(e), n = w.dirname(r), a = w.basename(e), i = w.basename(r), s, _, l;
          if (s = o.lookupPath(e, { parent: !0 }), _ = s.node, s = o.lookupPath(r, { parent: !0 }), l = s.node, !_ || !l) throw new o.ErrnoError(44);
          if (_.mount !== l.mount)
            throw new o.ErrnoError(75);
          var c = o.lookupNode(_, a), f = _e.relative(e, n);
          if (f.charAt(0) !== ".")
            throw new o.ErrnoError(28);
          if (f = _e.relative(r, t), f.charAt(0) !== ".")
            throw new o.ErrnoError(55);
          var E;
          try {
            E = o.lookupNode(l, i);
          } catch {
          }
          if (c !== E) {
            var D = o.isDir(c.mode), d = o.mayDelete(_, a, D);
            if (d)
              throw new o.ErrnoError(d);
            if (d = E ? o.mayDelete(l, i, D) : o.mayCreate(l, i), d)
              throw new o.ErrnoError(d);
            if (!_.node_ops.rename)
              throw new o.ErrnoError(63);
            if (o.isMountpoint(c) || E && o.isMountpoint(E))
              throw new o.ErrnoError(10);
            if (l !== _ && (d = o.nodePermissions(_, "w"), d))
              throw new o.ErrnoError(d);
            o.hashRemoveNode(c);
            try {
              _.node_ops.rename(c, l, i), c.parent = l;
            } catch (T) {
              throw T;
            } finally {
              o.hashAddNode(c);
            }
          }
        }, rmdir(e) {
          var r = o.lookupPath(e, { parent: !0 }), t = r.node, n = w.basename(e), a = o.lookupNode(t, n), i = o.mayDelete(t, n, !0);
          if (i)
            throw new o.ErrnoError(i);
          if (!t.node_ops.rmdir)
            throw new o.ErrnoError(63);
          if (o.isMountpoint(a))
            throw new o.ErrnoError(10);
          t.node_ops.rmdir(t, n), o.destroyNode(a);
        }, readdir(e) {
          var r = o.lookupPath(e, { follow: !0 }), t = r.node, n = o.checkOpExists(t.node_ops.readdir, 54);
          return n(t);
        }, unlink(e) {
          var r = o.lookupPath(e, { parent: !0 }), t = r.node;
          if (!t)
            throw new o.ErrnoError(44);
          var n = w.basename(e), a = o.lookupNode(t, n), i = o.mayDelete(t, n, !1);
          if (i)
            throw new o.ErrnoError(i);
          if (!t.node_ops.unlink)
            throw new o.ErrnoError(63);
          if (o.isMountpoint(a))
            throw new o.ErrnoError(10);
          t.node_ops.unlink(t, n), o.destroyNode(a);
        }, readlink(e) {
          var r = o.lookupPath(e), t = r.node;
          if (!t)
            throw new o.ErrnoError(44);
          if (!t.node_ops.readlink)
            throw new o.ErrnoError(28);
          return t.node_ops.readlink(t);
        }, stat(e, r) {
          var t = o.lookupPath(e, { follow: !r }), n = t.node, a = o.checkOpExists(n.node_ops.getattr, 63);
          return a(n);
        }, fstat(e) {
          var r = o.getStreamChecked(e), t = r.node, n = r.stream_ops.getattr, a = n ? r : t;
          return n ?? (n = t.node_ops.getattr), o.checkOpExists(n, 63), n(a);
        }, lstat(e) {
          return o.stat(e, !0);
        }, doChmod(e, r, t, n) {
          o.doSetAttr(e, r, { mode: t & 4095 | r.mode & -4096, ctime: Date.now(), dontFollow: n });
        }, chmod(e, r, t) {
          var n;
          if (typeof e == "string") {
            var a = o.lookupPath(e, { follow: !t });
            n = a.node;
          } else
            n = e;
          o.doChmod(null, n, r, t);
        }, lchmod(e, r) {
          o.chmod(e, r, !0);
        }, fchmod(e, r) {
          var t = o.getStreamChecked(e);
          o.doChmod(t, t.node, r, !1);
        }, doChown(e, r, t) {
          o.doSetAttr(e, r, { timestamp: Date.now(), dontFollow: t });
        }, chown(e, r, t, n) {
          var a;
          if (typeof e == "string") {
            var i = o.lookupPath(e, { follow: !n });
            a = i.node;
          } else
            a = e;
          o.doChown(null, a, n);
        }, lchown(e, r, t) {
          o.chown(e, r, t, !0);
        }, fchown(e, r, t) {
          var n = o.getStreamChecked(e);
          o.doChown(n, n.node, !1);
        }, doTruncate(e, r, t) {
          if (o.isDir(r.mode))
            throw new o.ErrnoError(31);
          if (!o.isFile(r.mode))
            throw new o.ErrnoError(28);
          var n = o.nodePermissions(r, "w");
          if (n)
            throw new o.ErrnoError(n);
          o.doSetAttr(e, r, { size: t, timestamp: Date.now() });
        }, truncate(e, r) {
          if (r < 0)
            throw new o.ErrnoError(28);
          var t;
          if (typeof e == "string") {
            var n = o.lookupPath(e, { follow: !0 });
            t = n.node;
          } else
            t = e;
          o.doTruncate(null, t, r);
        }, ftruncate(e, r) {
          var t = o.getStreamChecked(e);
          if (r < 0 || !(t.flags & 2097155))
            throw new o.ErrnoError(28);
          o.doTruncate(t, t.node, r);
        }, utime(e, r, t) {
          var n = o.lookupPath(e, { follow: !0 }), a = n.node, i = o.checkOpExists(a.node_ops.setattr, 63);
          i(a, { atime: r, mtime: t });
        }, open(e, r, t = 438) {
          if (e === "")
            throw new o.ErrnoError(44);
          r = typeof r == "string" ? kt(r) : r, r & 64 ? t = t & 4095 | 32768 : t = 0;
          var n, a;
          if (typeof e == "object")
            n = e;
          else {
            a = e.endsWith("/");
            var i = o.lookupPath(e, { follow: !(r & 131072), noent_okay: !0 });
            n = i.node, e = i.path;
          }
          var s = !1;
          if (r & 64)
            if (n) {
              if (r & 128)
                throw new o.ErrnoError(20);
            } else {
              if (a)
                throw new o.ErrnoError(31);
              n = o.mknod(e, t | 511, 0), s = !0;
            }
          if (!n)
            throw new o.ErrnoError(44);
          if (o.isChrdev(n.mode) && (r &= -513), r & 65536 && !o.isDir(n.mode))
            throw new o.ErrnoError(54);
          if (!s) {
            var _ = o.mayOpen(n, r);
            if (_)
              throw new o.ErrnoError(_);
          }
          r & 512 && !s && o.truncate(n, 0), r &= -131713;
          var l = o.createStream({ node: n, path: o.getPath(n), flags: r, seekable: !0, position: 0, stream_ops: n.stream_ops, ungotten: [], error: !1 });
          return l.stream_ops.open && l.stream_ops.open(l), s && o.chmod(n, t & 511), u.logReadFiles && !(r & 1) && (e in o.readFiles || (o.readFiles[e] = 1)), l;
        }, close(e) {
          if (o.isClosed(e))
            throw new o.ErrnoError(8);
          e.getdents && (e.getdents = null);
          try {
            e.stream_ops.close && e.stream_ops.close(e);
          } catch (r) {
            throw r;
          } finally {
            o.closeStream(e.fd);
          }
          e.fd = null;
        }, isClosed(e) {
          return e.fd === null;
        }, llseek(e, r, t) {
          if (o.isClosed(e))
            throw new o.ErrnoError(8);
          if (!e.seekable || !e.stream_ops.llseek)
            throw new o.ErrnoError(70);
          if (t != 0 && t != 1 && t != 2)
            throw new o.ErrnoError(28);
          return e.position = e.stream_ops.llseek(e, r, t), e.ungotten = [], e.position;
        }, read(e, r, t, n, a) {
          if (n < 0 || a < 0)
            throw new o.ErrnoError(28);
          if (o.isClosed(e))
            throw new o.ErrnoError(8);
          if ((e.flags & 2097155) === 1)
            throw new o.ErrnoError(8);
          if (o.isDir(e.node.mode))
            throw new o.ErrnoError(31);
          if (!e.stream_ops.read)
            throw new o.ErrnoError(28);
          var i = typeof a < "u";
          if (!i)
            a = e.position;
          else if (!e.seekable)
            throw new o.ErrnoError(70);
          var s = e.stream_ops.read(e, r, t, n, a);
          return i || (e.position += s), s;
        }, write(e, r, t, n, a, i) {
          if (n < 0 || a < 0)
            throw new o.ErrnoError(28);
          if (o.isClosed(e))
            throw new o.ErrnoError(8);
          if (!(e.flags & 2097155))
            throw new o.ErrnoError(8);
          if (o.isDir(e.node.mode))
            throw new o.ErrnoError(31);
          if (!e.stream_ops.write)
            throw new o.ErrnoError(28);
          e.seekable && e.flags & 1024 && o.llseek(e, 0, 2);
          var s = typeof a < "u";
          if (!s)
            a = e.position;
          else if (!e.seekable)
            throw new o.ErrnoError(70);
          var _ = e.stream_ops.write(e, r, t, n, a, i);
          return s || (e.position += _), _;
        }, allocate(e, r, t) {
          if (o.isClosed(e))
            throw new o.ErrnoError(8);
          if (r < 0 || t <= 0)
            throw new o.ErrnoError(28);
          if (!(e.flags & 2097155))
            throw new o.ErrnoError(8);
          if (!o.isFile(e.node.mode) && !o.isDir(e.node.mode))
            throw new o.ErrnoError(43);
          if (!e.stream_ops.allocate)
            throw new o.ErrnoError(138);
          e.stream_ops.allocate(e, r, t);
        }, mmap(e, r, t, n, a) {
          if (n & 2 && !(a & 2) && (e.flags & 2097155) !== 2)
            throw new o.ErrnoError(2);
          if ((e.flags & 2097155) === 1)
            throw new o.ErrnoError(2);
          if (!e.stream_ops.mmap)
            throw new o.ErrnoError(43);
          if (!r)
            throw new o.ErrnoError(28);
          return e.stream_ops.mmap(e, r, t, n, a);
        }, msync(e, r, t, n, a) {
          return e.stream_ops.msync ? e.stream_ops.msync(e, r, t, n, a) : 0;
        }, ioctl(e, r, t) {
          if (!e.stream_ops.ioctl)
            throw new o.ErrnoError(59);
          return e.stream_ops.ioctl(e, r, t);
        }, readFile(e, r = {}) {
          if (r.flags = r.flags || 0, r.encoding = r.encoding || "binary", r.encoding !== "utf8" && r.encoding !== "binary")
            throw new Error(`Invalid encoding type "${r.encoding}"`);
          var t, n = o.open(e, r.flags), a = o.stat(e), i = a.size, s = new Uint8Array(i);
          return o.read(n, s, 0, i, 0), r.encoding === "utf8" ? t = ie(s) : r.encoding === "binary" && (t = s), o.close(n), t;
        }, writeFile(e, r, t = {}) {
          t.flags = t.flags || 577;
          var n = o.open(e, t.flags, t.mode);
          if (typeof r == "string") {
            var a = new Uint8Array(Te(r) + 1), i = er(r, a, 0, a.length);
            o.write(n, a, 0, i, void 0, t.canOwn);
          } else if (ArrayBuffer.isView(r))
            o.write(n, r, 0, r.byteLength, void 0, t.canOwn);
          else
            throw new Error("Unsupported data type");
          o.close(n);
        }, cwd: () => o.currentPath, chdir(e) {
          var r = o.lookupPath(e, { follow: !0 });
          if (r.node === null)
            throw new o.ErrnoError(44);
          if (!o.isDir(r.node.mode))
            throw new o.ErrnoError(54);
          var t = o.nodePermissions(r.node, "x");
          if (t)
            throw new o.ErrnoError(t);
          o.currentPath = r.path;
        }, createDefaultDirectories() {
          o.mkdir("/tmp"), o.mkdir("/home"), o.mkdir("/home/web_user");
        }, createDefaultDevices() {
          o.mkdir("/dev"), o.registerDevice(o.makedev(1, 3), { read: () => 0, write: (n, a, i, s, _) => s, llseek: () => 0 }), o.mkdev("/dev/null", o.makedev(1, 3)), te.register(o.makedev(5, 0), te.default_tty_ops), te.register(o.makedev(6, 0), te.default_tty1_ops), o.mkdev("/dev/tty", o.makedev(5, 0)), o.mkdev("/dev/tty1", o.makedev(6, 0));
          var e = new Uint8Array(1024), r = 0, t = () => (r === 0 && (Rr(e), r = e.byteLength), e[--r]);
          o.createDevice("/dev", "random", t), o.createDevice("/dev", "urandom", t), o.mkdir("/dev/shm"), o.mkdir("/dev/shm/tmp");
        }, createSpecialDirectories() {
          o.mkdir("/proc");
          var e = o.mkdir("/proc/self");
          o.mkdir("/proc/self/fd"), o.mount({ mount() {
            var r = o.createNode(e, "fd", 16895, 73);
            return r.stream_ops = { llseek: P.stream_ops.llseek }, r.node_ops = { lookup(t, n) {
              var a = +n, i = o.getStreamChecked(a), s = { parent: null, mount: { mountpoint: "fake" }, node_ops: { readlink: () => i.path }, id: a + 1 };
              return s.parent = s, s;
            }, readdir() {
              return Array.from(o.streams.entries()).filter(([t, n]) => n).map(([t, n]) => t.toString());
            } }, r;
          } }, {}, "/proc/self/fd");
        }, createStandardStreams(e, r, t) {
          e ? o.createDevice("/dev", "stdin", e) : o.symlink("/dev/tty", "/dev/stdin"), r ? o.createDevice("/dev", "stdout", null, r) : o.symlink("/dev/tty", "/dev/stdout"), t ? o.createDevice("/dev", "stderr", null, t) : o.symlink("/dev/tty1", "/dev/stderr"), o.open("/dev/stdin", 0), o.open("/dev/stdout", 1), o.open("/dev/stderr", 1);
        }, staticInit() {
          o.nameTable = new Array(4096), o.mount(P, {}, "/"), o.createDefaultDirectories(), o.createDefaultDevices(), o.createSpecialDirectories(), o.filesystems = { MEMFS: P };
        }, init(e, r, t) {
          o.initialized = !0, e ?? (e = u.stdin), r ?? (r = u.stdout), t ?? (t = u.stderr), o.createStandardStreams(e, r, t);
        }, quit() {
          o.initialized = !1;
          for (var e = 0; e < o.streams.length; e++) {
            var r = o.streams[e];
            r && o.close(r);
          }
        }, findObject(e, r) {
          var t = o.analyzePath(e, r);
          return t.exists ? t.object : null;
        }, analyzePath(e, r) {
          try {
            var t = o.lookupPath(e, { follow: !r });
            e = t.path;
          } catch {
          }
          var n = { isRoot: !1, exists: !1, error: 0, name: null, path: null, object: null, parentExists: !1, parentPath: null, parentObject: null };
          try {
            var t = o.lookupPath(e, { parent: !0 });
            n.parentExists = !0, n.parentPath = t.path, n.parentObject = t.node, n.name = w.basename(e), t = o.lookupPath(e, { follow: !r }), n.exists = !0, n.path = t.path, n.object = t.node, n.name = t.node.name, n.isRoot = t.path === "/";
          } catch (a) {
            n.error = a.errno;
          }
          return n;
        }, createPath(e, r, t, n) {
          e = typeof e == "string" ? e : o.getPath(e);
          for (var a = r.split("/").reverse(); a.length; ) {
            var i = a.pop();
            if (i) {
              var s = w.join2(e, i);
              try {
                o.mkdir(s);
              } catch {
              }
              e = s;
            }
          }
          return s;
        }, createFile(e, r, t, n, a) {
          var i = w.join2(typeof e == "string" ? e : o.getPath(e), r), s = rr(n, a);
          return o.create(i, s);
        }, createDataFile(e, r, t, n, a, i) {
          var s = r;
          e && (e = typeof e == "string" ? e : o.getPath(e), s = r ? w.join2(e, r) : e);
          var _ = rr(n, a), l = o.create(s, _);
          if (t) {
            if (typeof t == "string") {
              for (var c = new Array(t.length), f = 0, E = t.length; f < E; ++f) c[f] = t.charCodeAt(f);
              t = c;
            }
            o.chmod(l, _ | 146);
            var D = o.open(l, 577);
            o.write(D, t, 0, t.length, 0, i), o.close(D), o.chmod(l, _);
          }
        }, createDevice(e, r, t, n) {
          var _;
          var a = w.join2(typeof e == "string" ? e : o.getPath(e), r), i = rr(!!t, !!n);
          (_ = o.createDevice).major ?? (_.major = 64);
          var s = o.makedev(o.createDevice.major++, 0);
          return o.registerDevice(s, { open(l) {
            l.seekable = !1;
          }, close(l) {
            var c;
            (c = n == null ? void 0 : n.buffer) != null && c.length && n(10);
          }, read(l, c, f, E, D) {
            for (var d = 0, T = 0; T < E; T++) {
              var y;
              try {
                y = t();
              } catch {
                throw new o.ErrnoError(29);
              }
              if (y === void 0 && d === 0)
                throw new o.ErrnoError(6);
              if (y == null) break;
              d++, c[f + T] = y;
            }
            return d && (l.node.atime = Date.now()), d;
          }, write(l, c, f, E, D) {
            for (var d = 0; d < E; d++)
              try {
                n(c[f + d]);
              } catch {
                throw new o.ErrnoError(29);
              }
            return E && (l.node.mtime = l.node.ctime = Date.now()), d;
          } }), o.mkdev(a, i, s);
        }, forceLoadFile(e) {
          if (e.isDevice || e.isFolder || e.link || e.contents) return !0;
          if (typeof XMLHttpRequest < "u")
            throw new Error("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
          try {
            e.contents = fe(e.url), e.usedBytes = e.contents.length;
          } catch {
            throw new o.ErrnoError(29);
          }
        }, createLazyFile(e, r, t, n, a) {
          class i {
            constructor() {
              U(this, "lengthKnown", !1);
              U(this, "chunks", []);
            }
            get(d) {
              if (!(d > this.length - 1 || d < 0)) {
                var T = d % this.chunkSize, y = d / this.chunkSize | 0;
                return this.getter(y)[T];
              }
            }
            setDataGetter(d) {
              this.getter = d;
            }
            cacheLength() {
              var d = new XMLHttpRequest();
              if (d.open("HEAD", t, !1), d.send(null), !(d.status >= 200 && d.status < 300 || d.status === 304)) throw new Error("Couldn't load " + t + ". Status: " + d.status);
              var T = Number(d.getResponseHeader("Content-length")), y, I = (y = d.getResponseHeader("Accept-Ranges")) && y === "bytes", G = (y = d.getResponseHeader("Content-Encoding")) && y === "gzip", L = 1024 * 1024;
              I || (L = T);
              var C = ($, z) => {
                if ($ > z) throw new Error("invalid range (" + $ + ", " + z + ") or no bytes requested!");
                if (z > T - 1) throw new Error("only " + T + " bytes available! programmer error!");
                var N = new XMLHttpRequest();
                if (N.open("GET", t, !1), T !== L && N.setRequestHeader("Range", "bytes=" + $ + "-" + z), N.responseType = "arraybuffer", N.overrideMimeType && N.overrideMimeType("text/plain; charset=x-user-defined"), N.send(null), !(N.status >= 200 && N.status < 300 || N.status === 304)) throw new Error("Couldn't load " + t + ". Status: " + N.status);
                return N.response !== void 0 ? new Uint8Array(N.response || []) : Sr(N.responseText || "");
              }, ae = this;
              ae.setDataGetter(($) => {
                var z = $ * L, N = ($ + 1) * L - 1;
                if (N = Math.min(N, T - 1), typeof ae.chunks[$] > "u" && (ae.chunks[$] = C(z, N)), typeof ae.chunks[$] > "u") throw new Error("doXHR failed!");
                return ae.chunks[$];
              }), (G || !T) && (L = T = 1, T = this.getter(0).length, L = T, Ke("LazyFiles on gzip forces download of the whole file when length is accessed")), this._length = T, this._chunkSize = L, this.lengthKnown = !0;
            }
            get length() {
              return this.lengthKnown || this.cacheLength(), this._length;
            }
            get chunkSize() {
              return this.lengthKnown || this.cacheLength(), this._chunkSize;
            }
          }
          if (typeof XMLHttpRequest < "u") {
            if (!me) throw "Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc";
            var s = new i(), _ = { isDevice: !1, contents: s };
          } else
            var _ = { isDevice: !1, url: t };
          var l = o.createFile(e, r, _, n, a);
          _.contents ? l.contents = _.contents : _.url && (l.contents = null, l.url = _.url), Object.defineProperties(l, { usedBytes: { get: function() {
            return this.contents.length;
          } } });
          var c = {}, f = Object.keys(l.stream_ops);
          f.forEach((D) => {
            var d = l.stream_ops[D];
            c[D] = (...T) => (o.forceLoadFile(l), d(...T));
          });
          function E(D, d, T, y, I) {
            var G = D.node.contents;
            if (I >= G.length) return 0;
            var L = Math.min(G.length - I, y);
            if (G.slice)
              for (var C = 0; C < L; C++)
                d[T + C] = G[I + C];
            else
              for (var C = 0; C < L; C++)
                d[T + C] = G.get(I + C);
            return L;
          }
          return c.read = (D, d, T, y, I) => (o.forceLoadFile(l), E(D, d, T, y, I)), c.mmap = (D, d, T, y, I) => {
            o.forceLoadFile(l);
            var G = Wr(d);
            if (!G)
              throw new o.ErrnoError(48);
            return E(D, W, G, d, T), { ptr: G, allocated: !0 };
          }, l.stream_ops = c, l;
        } }, S = { DEFAULT_POLLMASK: 5, calculateAt(e, r, t) {
          if (w.isAbs(r))
            return r;
          var n;
          if (e === -100)
            n = o.cwd();
          else {
            var a = S.getStreamFromFD(e);
            n = a.path;
          }
          if (r.length == 0) {
            if (!t)
              throw new o.ErrnoError(44);
            return n;
          }
          return n + "/" + r;
        }, writeStat(e, r) {
          v[e >> 2] = r.dev, v[e + 4 >> 2] = r.mode, g[e + 8 >> 2] = r.nlink, v[e + 12 >> 2] = r.uid, v[e + 16 >> 2] = r.gid, v[e + 20 >> 2] = r.rdev, K[e + 24 >> 3] = BigInt(r.size), v[e + 32 >> 2] = 4096, v[e + 36 >> 2] = r.blocks;
          var t = r.atime.getTime(), n = r.mtime.getTime(), a = r.ctime.getTime();
          return K[e + 40 >> 3] = BigInt(Math.floor(t / 1e3)), g[e + 48 >> 2] = t % 1e3 * 1e3 * 1e3, K[e + 56 >> 3] = BigInt(Math.floor(n / 1e3)), g[e + 64 >> 2] = n % 1e3 * 1e3 * 1e3, K[e + 72 >> 3] = BigInt(Math.floor(a / 1e3)), g[e + 80 >> 2] = a % 1e3 * 1e3 * 1e3, K[e + 88 >> 3] = BigInt(r.ino), 0;
        }, writeStatFs(e, r) {
          v[e + 4 >> 2] = r.bsize, v[e + 40 >> 2] = r.bsize, v[e + 8 >> 2] = r.blocks, v[e + 12 >> 2] = r.bfree, v[e + 16 >> 2] = r.bavail, v[e + 20 >> 2] = r.files, v[e + 24 >> 2] = r.ffree, v[e + 28 >> 2] = r.fsid, v[e + 44 >> 2] = r.flags, v[e + 36 >> 2] = r.namelen;
        }, doMsync(e, r, t, n, a) {
          if (!o.isFile(r.node.mode))
            throw new o.ErrnoError(43);
          if (n & 2)
            return 0;
          var i = F.slice(e, e + t);
          o.msync(r, i, a, t, n);
        }, getStreamFromFD(e) {
          var r = o.getStreamChecked(e);
          return r;
        }, varargs: void 0, getStr(e) {
          var r = re(e);
          return r;
        } };
        function Ft(e, r, t) {
          S.varargs = t;
          try {
            var n = S.getStreamFromFD(e);
            switch (r) {
              case 0: {
                var a = Ge();
                if (a < 0)
                  return -28;
                for (; o.streams[a]; )
                  a++;
                var i;
                return i = o.dupStream(n, a), i.fd;
              }
              case 1:
              case 2:
                return 0;
              case 3:
                return n.flags;
              case 4: {
                var a = Ge();
                return n.flags |= a, 0;
              }
              case 12: {
                var a = se(), s = 0;
                return j[a + s >> 1] = 2, 0;
              }
              case 13:
              case 14:
                return 0;
            }
            return -28;
          } catch (_) {
            if (typeof o > "u" || _.name !== "ErrnoError") throw _;
            return -_.errno;
          }
        }
        function Mt(e, r) {
          try {
            return S.writeStat(r, o.fstat(e));
          } catch (t) {
            if (typeof o > "u" || t.name !== "ErrnoError") throw t;
            return -t.errno;
          }
        }
        function $t(e, r, t) {
          S.varargs = t;
          try {
            var n = S.getStreamFromFD(e);
            switch (r) {
              case 21509:
                return n.tty ? 0 : -59;
              case 21505: {
                if (!n.tty) return -59;
                if (n.tty.ops.ioctl_tcgets) {
                  var a = n.tty.ops.ioctl_tcgets(n), i = se();
                  v[i >> 2] = a.c_iflag || 0, v[i + 4 >> 2] = a.c_oflag || 0, v[i + 8 >> 2] = a.c_cflag || 0, v[i + 12 >> 2] = a.c_lflag || 0;
                  for (var s = 0; s < 32; s++)
                    W[i + s + 17] = a.c_cc[s] || 0;
                  return 0;
                }
                return 0;
              }
              case 21510:
              case 21511:
              case 21512:
                return n.tty ? 0 : -59;
              case 21506:
              case 21507:
              case 21508: {
                if (!n.tty) return -59;
                if (n.tty.ops.ioctl_tcsets) {
                  for (var i = se(), _ = v[i >> 2], l = v[i + 4 >> 2], c = v[i + 8 >> 2], f = v[i + 12 >> 2], E = [], s = 0; s < 32; s++)
                    E.push(W[i + s + 17]);
                  return n.tty.ops.ioctl_tcsets(n.tty, r, { c_iflag: _, c_oflag: l, c_cflag: c, c_lflag: f, c_cc: E });
                }
                return 0;
              }
              case 21519: {
                if (!n.tty) return -59;
                var i = se();
                return v[i >> 2] = 0, 0;
              }
              case 21520:
                return n.tty ? -28 : -59;
              case 21531: {
                var i = se();
                return o.ioctl(n, r, i);
              }
              case 21523: {
                if (!n.tty) return -59;
                if (n.tty.ops.ioctl_tiocgwinsz) {
                  var D = n.tty.ops.ioctl_tiocgwinsz(n.tty), i = se();
                  j[i >> 1] = D[0], j[i + 2 >> 1] = D[1];
                }
                return 0;
              }
              case 21524:
                return n.tty ? 0 : -59;
              case 21515:
                return n.tty ? 0 : -59;
              default:
                return -28;
            }
          } catch (d) {
            if (typeof o > "u" || d.name !== "ErrnoError") throw d;
            return -d.errno;
          }
        }
        function Bt(e, r) {
          try {
            return e = S.getStr(e), S.writeStat(r, o.lstat(e));
          } catch (t) {
            if (typeof o > "u" || t.name !== "ErrnoError") throw t;
            return -t.errno;
          }
        }
        function xt(e, r, t, n) {
          try {
            r = S.getStr(r);
            var a = n & 256, i = n & 4096;
            return n = n & -6401, r = S.calculateAt(e, r, i), S.writeStat(t, a ? o.lstat(r) : o.stat(r));
          } catch (s) {
            if (typeof o > "u" || s.name !== "ErrnoError") throw s;
            return -s.errno;
          }
        }
        function Ut(e, r, t, n) {
          S.varargs = n;
          try {
            r = S.getStr(r), r = S.calculateAt(e, r);
            var a = n ? Ge() : 0;
            return o.open(r, t, a).fd;
          } catch (i) {
            if (typeof o > "u" || i.name !== "ErrnoError") throw i;
            return -i.errno;
          }
        }
        function jt(e, r) {
          try {
            return e = S.getStr(e), S.writeStat(r, o.stat(e));
          } catch (t) {
            if (typeof o > "u" || t.name !== "ErrnoError") throw t;
            return -t.errno;
          }
        }
        var Ht = () => ve(""), We = (e) => {
          if (e === null)
            return "null";
          var r = typeof e;
          return r === "object" || r === "array" || r === "function" ? e.toString() : "" + e;
        }, Vt = () => {
          for (var e = new Array(256), r = 0; r < 256; ++r)
            e[r] = String.fromCharCode(r);
          Nr = e;
        }, Nr, b = (e) => {
          for (var r = "", t = e; F[t]; )
            r += Nr[F[t++]];
          return r;
        }, le = {}, ne = {}, Ne = {}, he, m = (e) => {
          throw new he(e);
        }, Ir, Ie = (e) => {
          throw new Ir(e);
        }, Le = (e, r, t) => {
          e.forEach((_) => Ne[_] = r);
          function n(_) {
            var l = t(_);
            l.length !== e.length && Ie("Mismatched type converter count");
            for (var c = 0; c < e.length; ++c)
              x(e[c], l[c]);
          }
          var a = new Array(r.length), i = [], s = 0;
          r.forEach((_, l) => {
            ne.hasOwnProperty(_) ? a[l] = ne[_] : (i.push(_), le.hasOwnProperty(_) || (le[_] = []), le[_].push(() => {
              a[l] = ne[_], ++s, s === i.length && n(a);
            }));
          }), i.length === 0 && n(a);
        };
        function Kt(e, r, t = {}) {
          var n = r.name;
          if (e || m(`type "${n}" must have a positive integer typeid pointer`), ne.hasOwnProperty(e)) {
            if (t.ignoreDuplicateRegistrations)
              return;
            m(`Cannot register type '${n}' twice`);
          }
          if (ne[e] = r, delete Ne[e], le.hasOwnProperty(e)) {
            var a = le[e];
            delete le[e], a.forEach((i) => i());
          }
        }
        function x(e, r, t = {}) {
          return Kt(e, r, t);
        }
        var Lr = (e, r, t) => {
          switch (r) {
            case 1:
              return t ? (n) => W[n] : (n) => F[n];
            case 2:
              return t ? (n) => j[n >> 1] : (n) => Ee[n >> 1];
            case 4:
              return t ? (n) => v[n >> 2] : (n) => g[n >> 2];
            case 8:
              return t ? (n) => K[n >> 3] : (n) => hr[n >> 3];
            default:
              throw new TypeError(`invalid integer width (${r}): ${e}`);
          }
        }, Xt = (e, r, t, n, a) => {
          r = b(r);
          var i = r.indexOf("u") != -1;
          x(e, { name: r, fromWireType: (s) => s, toWireType: function(s, _) {
            if (typeof _ != "bigint" && typeof _ != "number")
              throw new TypeError(`Cannot convert "${We(_)}" to ${this.name}`);
            return typeof _ == "number" && (_ = BigInt(_)), _;
          }, argPackAdvance: H, readValueFromPointer: Lr(r, t, !i), destructorFunction: null });
        }, H = 8, zt = (e, r, t, n) => {
          r = b(r), x(e, { name: r, fromWireType: function(a) {
            return !!a;
          }, toWireType: function(a, i) {
            return i ? t : n;
          }, argPackAdvance: H, readValueFromPointer: function(a) {
            return this.fromWireType(F[a]);
          }, destructorFunction: null });
        }, Jt = (e) => ({ count: e.count, deleteScheduled: e.deleteScheduled, preservePointerOnDelete: e.preservePointerOnDelete, ptr: e.ptr, ptrType: e.ptrType, smartPtr: e.smartPtr, smartPtrType: e.smartPtrType }), tr = (e) => {
          function r(t) {
            return t.$$.ptrType.registeredClass.name;
          }
          m(r(e) + " instance already deleted");
        }, nr = !1, br = (e) => {
        }, qt = (e) => {
          e.smartPtr ? e.smartPtrType.rawDestructor(e.smartPtr) : e.ptrType.registeredClass.rawDestructor(e.ptr);
        }, kr = (e) => {
          e.count.value -= 1;
          var r = e.count.value === 0;
          r && qt(e);
        }, Fr = (e, r, t) => {
          if (r === t)
            return e;
          if (t.baseClass === void 0)
            return null;
          var n = Fr(e, r, t.baseClass);
          return n === null ? null : t.downcast(n);
        }, Mr = {}, Zt = {}, Qt = (e, r) => {
          for (r === void 0 && m("ptr should not be undefined"); e.baseClass; )
            r = e.upcast(r), e = e.baseClass;
          return r;
        }, en = (e, r) => (r = Qt(e, r), Zt[r]), be = (e, r) => {
          (!r.ptrType || !r.ptr) && Ie("makeClassHandle requires ptr and ptrType");
          var t = !!r.smartPtrType, n = !!r.smartPtr;
          return t !== n && Ie("Both smartPtrType and smartPtr must be specified"), r.count = { value: 1 }, pe(Object.create(e, { $$: { value: r, writable: !0 } }));
        };
        function rn(e) {
          var r = this.getPointee(e);
          if (!r)
            return this.destructor(e), null;
          var t = en(this.registeredClass, r);
          if (t !== void 0) {
            if (t.$$.count.value === 0)
              return t.$$.ptr = r, t.$$.smartPtr = e, t.clone();
            var n = t.clone();
            return this.destructor(e), n;
          }
          function a() {
            return this.isSmartPointer ? be(this.registeredClass.instancePrototype, { ptrType: this.pointeeType, ptr: r, smartPtrType: this, smartPtr: e }) : be(this.registeredClass.instancePrototype, { ptrType: this, ptr: e });
          }
          var i = this.registeredClass.getActualType(r), s = Mr[i];
          if (!s)
            return a.call(this);
          var _;
          this.isConst ? _ = s.constPointerType : _ = s.pointerType;
          var l = Fr(r, this.registeredClass, _.registeredClass);
          return l === null ? a.call(this) : this.isSmartPointer ? be(_.registeredClass.instancePrototype, { ptrType: _, ptr: l, smartPtrType: this, smartPtr: e }) : be(_.registeredClass.instancePrototype, { ptrType: _, ptr: l });
        }
        var pe = (e) => typeof FinalizationRegistry > "u" ? (pe = (r) => r, e) : (nr = new FinalizationRegistry((r) => {
          kr(r.$$);
        }), pe = (r) => {
          var t = r.$$, n = !!t.smartPtr;
          if (n) {
            var a = { $$: t };
            nr.register(r, a, r);
          }
          return r;
        }, br = (r) => nr.unregister(r), pe(e)), tn = () => {
          Object.assign(ke.prototype, { isAliasOf(e) {
            if (!(this instanceof ke) || !(e instanceof ke))
              return !1;
            var r = this.$$.ptrType.registeredClass, t = this.$$.ptr;
            e.$$ = e.$$;
            for (var n = e.$$.ptrType.registeredClass, a = e.$$.ptr; r.baseClass; )
              t = r.upcast(t), r = r.baseClass;
            for (; n.baseClass; )
              a = n.upcast(a), n = n.baseClass;
            return r === n && t === a;
          }, clone() {
            if (this.$$.ptr || tr(this), this.$$.preservePointerOnDelete)
              return this.$$.count.value += 1, this;
            var e = pe(Object.create(Object.getPrototypeOf(this), { $$: { value: Jt(this.$$) } }));
            return e.$$.count.value += 1, e.$$.deleteScheduled = !1, e;
          }, delete() {
            this.$$.ptr || tr(this), this.$$.deleteScheduled && !this.$$.preservePointerOnDelete && m("Object already scheduled for deletion"), br(this), kr(this.$$), this.$$.preservePointerOnDelete || (this.$$.smartPtr = void 0, this.$$.ptr = void 0);
          }, isDeleted() {
            return !this.$$.ptr;
          }, deleteLater() {
            return this.$$.ptr || tr(this), this.$$.deleteScheduled && !this.$$.preservePointerOnDelete && m("Object already scheduled for deletion"), this.$$.deleteScheduled = !0, this;
          } });
        };
        function ke() {
        }
        var ue = (e, r) => Object.defineProperty(r, "name", { value: e }), nn = (e, r, t) => {
          if (e[r].overloadTable === void 0) {
            var n = e[r];
            e[r] = function(...a) {
              return e[r].overloadTable.hasOwnProperty(a.length) || m(`Function '${t}' called with an invalid number of arguments (${a.length}) - expects one of (${e[r].overloadTable})!`), e[r].overloadTable[a.length].apply(this, a);
            }, e[r].overloadTable = [], e[r].overloadTable[n.argCount] = n;
          }
        }, or = (e, r, t) => {
          u.hasOwnProperty(e) ? ((t === void 0 || u[e].overloadTable !== void 0 && u[e].overloadTable[t] !== void 0) && m(`Cannot register public name '${e}' twice`), nn(u, e, e), u[e].overloadTable.hasOwnProperty(t) && m(`Cannot register multiple overloads of a function with the same number of arguments (${t})!`), u[e].overloadTable[t] = r) : (u[e] = r, u[e].argCount = t);
        }, on = 48, an = 57, sn = (e) => {
          e = e.replace(/[^a-zA-Z0-9_]/g, "$");
          var r = e.charCodeAt(0);
          return r >= on && r <= an ? `_${e}` : e;
        };
        function _n(e, r, t, n, a, i, s, _) {
          this.name = e, this.constructor = r, this.instancePrototype = t, this.rawDestructor = n, this.baseClass = a, this.getActualType = i, this.upcast = s, this.downcast = _, this.pureVirtualFunctions = [];
        }
        var Fe = (e, r, t) => {
          for (; r !== t; )
            r.upcast || m(`Expected null or instance of ${t.name}, got an instance of ${r.name}`), e = r.upcast(e), r = r.baseClass;
          return e;
        };
        function ln(e, r) {
          if (r === null)
            return this.isReference && m(`null is not a valid ${this.name}`), 0;
          r.$$ || m(`Cannot pass "${We(r)}" as a ${this.name}`), r.$$.ptr || m(`Cannot pass deleted object as a pointer of type ${this.name}`);
          var t = r.$$.ptrType.registeredClass, n = Fe(r.$$.ptr, t, this.registeredClass);
          return n;
        }
        function un(e, r) {
          var t;
          if (r === null)
            return this.isReference && m(`null is not a valid ${this.name}`), this.isSmartPointer ? (t = this.rawConstructor(), e !== null && e.push(this.rawDestructor, t), t) : 0;
          (!r || !r.$$) && m(`Cannot pass "${We(r)}" as a ${this.name}`), r.$$.ptr || m(`Cannot pass deleted object as a pointer of type ${this.name}`), !this.isConst && r.$$.ptrType.isConst && m(`Cannot convert argument of type ${r.$$.smartPtrType ? r.$$.smartPtrType.name : r.$$.ptrType.name} to parameter type ${this.name}`);
          var n = r.$$.ptrType.registeredClass;
          if (t = Fe(r.$$.ptr, n, this.registeredClass), this.isSmartPointer)
            switch (r.$$.smartPtr === void 0 && m("Passing raw pointer to smart pointer is illegal"), this.sharingPolicy) {
              case 0:
                r.$$.smartPtrType === this ? t = r.$$.smartPtr : m(`Cannot convert argument of type ${r.$$.smartPtrType ? r.$$.smartPtrType.name : r.$$.ptrType.name} to parameter type ${this.name}`);
                break;
              case 1:
                t = r.$$.smartPtr;
                break;
              case 2:
                if (r.$$.smartPtrType === this)
                  t = r.$$.smartPtr;
                else {
                  var a = r.clone();
                  t = this.rawShare(t, M.toHandle(() => a.delete())), e !== null && e.push(this.rawDestructor, t);
                }
                break;
              default:
                m("Unsupporting sharing policy");
            }
          return t;
        }
        function cn(e, r) {
          if (r === null)
            return this.isReference && m(`null is not a valid ${this.name}`), 0;
          r.$$ || m(`Cannot pass "${We(r)}" as a ${this.name}`), r.$$.ptr || m(`Cannot pass deleted object as a pointer of type ${this.name}`), r.$$.ptrType.isConst && m(`Cannot convert argument of type ${r.$$.ptrType.name} to parameter type ${this.name}`);
          var t = r.$$.ptrType.registeredClass, n = Fe(r.$$.ptr, t, this.registeredClass);
          return n;
        }
        function Me(e) {
          return this.fromWireType(g[e >> 2]);
        }
        var fn = () => {
          Object.assign($e.prototype, { getPointee(e) {
            return this.rawGetPointee && (e = this.rawGetPointee(e)), e;
          }, destructor(e) {
            var r;
            (r = this.rawDestructor) == null || r.call(this, e);
          }, argPackAdvance: H, readValueFromPointer: Me, fromWireType: rn });
        };
        function $e(e, r, t, n, a, i, s, _, l, c, f) {
          this.name = e, this.registeredClass = r, this.isReference = t, this.isConst = n, this.isSmartPointer = a, this.pointeeType = i, this.sharingPolicy = s, this.rawGetPointee = _, this.rawConstructor = l, this.rawShare = c, this.rawDestructor = f, !a && r.baseClass === void 0 ? n ? (this.toWireType = ln, this.destructorFunction = null) : (this.toWireType = cn, this.destructorFunction = null) : this.toWireType = un;
        }
        var $r = (e, r, t) => {
          u.hasOwnProperty(e) || Ie("Replacing nonexistent public symbol"), u[e].overloadTable !== void 0 && t !== void 0 ? u[e].overloadTable[t] = r : (u[e] = r, u[e].argCount = t);
        }, oe = (e, r) => {
          e = b(e);
          function t() {
            return Cr(r);
          }
          var n = t();
          return typeof n != "function" && m(`unknown function pointer with signature ${e}: ${r}`), n;
        }, En = (e, r) => {
          var t = ue(r, function(n) {
            this.name = r, this.message = n;
            var a = new Error(n).stack;
            a !== void 0 && (this.stack = this.toString() + `
` + a.replace(/^Error(:[^\n]*)?\n/, ""));
          });
          return t.prototype = Object.create(e.prototype), t.prototype.constructor = t, t.prototype.toString = function() {
            return this.message === void 0 ? this.name : `${this.name}: ${this.message}`;
          }, t;
        }, Br, xr = (e) => {
          var r = Uo(e), t = b(r);
          return Z(r), t;
        }, Be = (e, r) => {
          var t = [], n = {};
          function a(i) {
            if (!n[i] && !ne[i]) {
              if (Ne[i]) {
                Ne[i].forEach(a);
                return;
              }
              t.push(i), n[i] = !0;
            }
          }
          throw r.forEach(a), new Br(`${e}: ` + t.map(xr).join([", "]));
        }, dn = (e, r, t, n, a, i, s, _, l, c, f, E, D) => {
          f = b(f), i = oe(a, i), _ && (_ = oe(s, _)), c && (c = oe(l, c)), D = oe(E, D);
          var d = sn(f);
          or(d, function() {
            Be(`Cannot construct ${f} due to unbound types`, [n]);
          }), Le([e, r, t], n ? [n] : [], (T) => {
            var N;
            T = T[0];
            var y, I;
            n ? (y = T.registeredClass, I = y.instancePrototype) : I = ke.prototype;
            var G = ue(f, function(...Er) {
              if (Object.getPrototypeOf(this) !== L)
                throw new he("Use 'new' to construct " + f);
              if (C.constructor_body === void 0)
                throw new he(f + " has no accessible constructor");
              var ot = C.constructor_body[Er.length];
              if (ot === void 0)
                throw new he(`Tried to invoke ctor of ${f} with invalid number of parameters (${Er.length}) - expected (${Object.keys(C.constructor_body).toString()}) parameters instead!`);
              return ot.apply(this, Er);
            }), L = Object.create(I, { constructor: { value: G } });
            G.prototype = L;
            var C = new _n(f, G, L, D, y, i, _, c);
            C.baseClass && ((N = C.baseClass).__derivedClasses ?? (N.__derivedClasses = []), C.baseClass.__derivedClasses.push(C));
            var ae = new $e(f, C, !0, !1, !1), $ = new $e(f + "*", C, !1, !1, !1), z = new $e(f + " const*", C, !1, !0, !1);
            return Mr[e] = { pointerType: $, constPointerType: z }, $r(d, G), [ae, $, z];
          });
        }, ar = (e) => {
          for (; e.length; ) {
            var r = e.pop(), t = e.pop();
            t(r);
          }
        }, Ur = (e, r, t) => (e instanceof Object || m(`${t} with invalid "this": ${e}`), e instanceof r.registeredClass.constructor || m(`${t} incompatible with "this" of type ${e.constructor.name}`), e.$$.ptr || m(`cannot call emscripten binding method ${t} on deleted object`), Fe(e.$$.ptr, e.$$.ptrType.registeredClass, r.registeredClass)), vn = (e, r, t, n, a, i, s, _, l, c) => {
          r = b(r), a = oe(n, a), Le([], [e], (f) => {
            f = f[0];
            var E = `${f.name}.${r}`, D = { get() {
              Be(`Cannot access ${E} due to unbound types`, [t, s]);
            }, enumerable: !0, configurable: !0 };
            return l ? D.set = () => Be(`Cannot access ${E} due to unbound types`, [t, s]) : D.set = (d) => m(E + " is a read-only property"), Object.defineProperty(f.registeredClass.instancePrototype, r, D), Le([], l ? [t, s] : [t], (d) => {
              var T = d[0], y = { get() {
                var G = Ur(this, f, E + " getter");
                return T.fromWireType(a(i, G));
              }, enumerable: !0 };
              if (l) {
                l = oe(_, l);
                var I = d[1];
                y.set = function(G) {
                  var L = Ur(this, f, E + " setter"), C = [];
                  l(c, L, I.toWireType(C, G)), ar(C);
                };
              }
              return Object.defineProperty(f.registeredClass.instancePrototype, r, y), [];
            }), [];
          });
        }, ir = [], X = [], sr = (e) => {
          e > 9 && --X[e + 1] === 0 && (X[e] = void 0, ir.push(e));
        }, Tn = () => X.length / 2 - 5 - ir.length, hn = () => {
          X.push(0, 1, void 0, 1, null, 1, !0, 1, !1, 1), u.count_emval_handles = Tn;
        }, M = { toValue: (e) => (e || m("Cannot use deleted val. handle = " + e), X[e]), toHandle: (e) => {
          switch (e) {
            case void 0:
              return 2;
            case null:
              return 4;
            case !0:
              return 6;
            case !1:
              return 8;
            default: {
              const r = ir.pop() || X.length;
              return X[r] = e, X[r + 1] = 1, r;
            }
          }
        } }, pn = { name: "emscripten::val", fromWireType: (e) => {
          var r = M.toValue(e);
          return sr(e), r;
        }, toWireType: (e, r) => M.toHandle(r), argPackAdvance: H, readValueFromPointer: Me, destructorFunction: null }, Dn = (e) => x(e, pn), Pn = (e, r, t) => {
          switch (r) {
            case 1:
              return t ? function(n) {
                return this.fromWireType(W[n]);
              } : function(n) {
                return this.fromWireType(F[n]);
              };
            case 2:
              return t ? function(n) {
                return this.fromWireType(j[n >> 1]);
              } : function(n) {
                return this.fromWireType(Ee[n >> 1]);
              };
            case 4:
              return t ? function(n) {
                return this.fromWireType(v[n >> 2]);
              } : function(n) {
                return this.fromWireType(g[n >> 2]);
              };
            default:
              throw new TypeError(`invalid integer width (${r}): ${e}`);
          }
        }, mn = (e, r, t, n) => {
          r = b(r);
          function a() {
          }
          a.values = {}, x(e, { name: r, constructor: a, fromWireType: function(i) {
            return this.constructor.values[i];
          }, toWireType: (i, s) => s.value, argPackAdvance: H, readValueFromPointer: Pn(r, t, n), destructorFunction: null }), or(r, a);
        }, _r = (e, r) => {
          var t = ne[e];
          return t === void 0 && m(`${r} has unknown type ${xr(e)}`), t;
        }, gn = (e, r, t) => {
          var n = _r(e, "enum");
          r = b(r);
          var a = n.constructor, i = Object.create(n.constructor.prototype, { value: { value: t }, constructor: { value: ue(`${n.name}_${r}`, function() {
          }) } });
          a.values[t] = i, a[r] = i;
        }, An = (e, r) => {
          switch (r) {
            case 4:
              return function(t) {
                return this.fromWireType(Xe[t >> 2]);
              };
            case 8:
              return function(t) {
                return this.fromWireType(ze[t >> 3]);
              };
            default:
              throw new TypeError(`invalid float width (${r}): ${e}`);
          }
        }, yn = (e, r, t) => {
          r = b(r), x(e, { name: r, fromWireType: (n) => n, toWireType: (n, a) => a, argPackAdvance: H, readValueFromPointer: An(r, t), destructorFunction: null });
        };
        function jr(e) {
          for (var r = 1; r < e.length; ++r)
            if (e[r] !== null && e[r].destructorFunction === void 0)
              return !0;
          return !1;
        }
        function Hr(e, r) {
          if (!(e instanceof Function))
            throw new TypeError(`new_ called with constructor type ${typeof e} which is not a function`);
          var t = ue(e.name || "unknownFunctionName", function() {
          });
          t.prototype = e.prototype;
          var n = new t(), a = e.apply(n, r);
          return a instanceof Object ? a : n;
        }
        function wn(e, r, t, n) {
          for (var a = jr(e), i = e.length - 2, s = [], _ = ["fn"], l = 0; l < i; ++l)
            s.push(`arg${l}`), _.push(`arg${l}Wired`);
          s = s.join(","), _ = _.join(",");
          var c = `return function (${s}) {
`;
          a && (c += `var destructors = [];
`);
          for (var f = a ? "destructors" : "null", E = ["humanName", "throwBindingError", "invoker", "fn", "runDestructors", "retType", "classParam"], l = 0; l < i; ++l)
            c += `var arg${l}Wired = argType${l}['toWireType'](${f}, arg${l});
`, E.push(`argType${l}`);
          if (c += (t || n ? "var rv = " : "") + `invoker(${_});
`, a)
            c += `runDestructors(destructors);
`;
          else
            for (var l = 2; l < e.length; ++l) {
              var D = l === 1 ? "thisWired" : "arg" + (l - 2) + "Wired";
              e[l].destructorFunction !== null && (c += `${D}_dtor(${D});
`, E.push(`${D}_dtor`));
            }
          return t && (c += `var ret = retType['fromWireType'](rv);
return ret;
`), c += `}
`, [E, c];
        }
        function On(e, r, t, n, a, i) {
          var s = r.length;
          s < 2 && m("argTypes array size mismatch! Must at least get return value and 'this' types!");
          for (var _ = r[1] !== null && t !== null, l = jr(r), c = r[0].name !== "void", f = [e, m, n, a, ar, r[0], r[1]], E = 0; E < s - 2; ++E)
            f.push(r[E + 2]);
          if (!l)
            for (var E = 2; E < r.length; ++E)
              r[E].destructorFunction !== null && f.push(r[E].destructorFunction);
          let [D, d] = wn(r, _, c, i);
          D.push(d);
          var T = Hr(Function, D)(...f);
          return ue(e, T);
        }
        var Cn = (e, r) => {
          for (var t = [], n = 0; n < e; n++)
            t.push(g[r + n * 4 >> 2]);
          return t;
        }, Yn = (e) => {
          e = e.trim();
          const r = e.indexOf("(");
          return r === -1 ? e : e.slice(0, r);
        }, Rn = (e, r, t, n, a, i, s, _) => {
          var l = Cn(r, t);
          e = b(e), e = Yn(e), a = oe(n, a), or(e, function() {
            Be(`Cannot call ${e} due to unbound types`, l);
          }, r - 1), Le([], l, (c) => {
            var f = [c[0], null].concat(c.slice(1));
            return $r(e, On(e, f, null, a, i, s), r - 1), [];
          });
        }, Sn = (e, r, t, n, a) => {
          r = b(r);
          var i = (f) => f;
          if (n === 0) {
            var s = 32 - 8 * t;
            i = (f) => f << s >>> s;
          }
          var _ = r.includes("unsigned"), l = (f, E) => {
          }, c;
          _ ? c = function(f, E) {
            return l(E, this.name), E >>> 0;
          } : c = function(f, E) {
            return l(E, this.name), E;
          }, x(e, { name: r, fromWireType: i, toWireType: c, argPackAdvance: H, readValueFromPointer: Lr(r, t, n !== 0), destructorFunction: null });
        }, Gn = (e, r, t) => {
          var n = [Int8Array, Uint8Array, Int16Array, Uint16Array, Int32Array, Uint32Array, Float32Array, Float64Array, BigInt64Array, BigUint64Array], a = n[r];
          function i(s) {
            var _ = g[s >> 2], l = g[s + 4 >> 2];
            return new a(W.buffer, l, _);
          }
          t = b(t), x(e, { name: t, fromWireType: i, argPackAdvance: H, readValueFromPointer: i }, { ignoreDuplicateRegistrations: !0 });
        }, q = (e, r, t) => er(e, F, r, t), Wn = (e, r) => {
          r = b(r), x(e, { name: r, fromWireType(t) {
            for (var n = g[t >> 2], a = t + 4, i, s, _ = a, s = 0; s <= n; ++s) {
              var l = a + s;
              if (s == n || F[l] == 0) {
                var c = l - _, f = re(_, c);
                i === void 0 ? i = f : (i += "\0", i += f), _ = l + 1;
              }
            }
            return Z(t), i;
          }, toWireType(t, n) {
            n instanceof ArrayBuffer && (n = new Uint8Array(n));
            var a, i = typeof n == "string";
            i || n instanceof Uint8Array || n instanceof Uint8ClampedArray || n instanceof Int8Array || m("Cannot pass non-string to std::string"), i ? a = Te(n) : a = n.length;
            var s = cr(4 + a + 1), _ = s + 4;
            if (g[s >> 2] = a, i)
              q(n, _, a + 1);
            else if (i)
              for (var l = 0; l < a; ++l) {
                var c = n.charCodeAt(l);
                c > 255 && (Z(s), m("String has UTF-16 code units that do not fit in 8 bits")), F[_ + l] = c;
              }
            else
              for (var l = 0; l < a; ++l)
                F[_ + l] = n[l];
            return t !== null && t.push(Z, s), s;
          }, argPackAdvance: H, readValueFromPointer: Me, destructorFunction(t) {
            Z(t);
          } });
        }, Vr = typeof TextDecoder < "u" ? new TextDecoder("utf-16le") : void 0, Nn = (e, r) => {
          for (var t = e, n = t >> 1, a = n + r / 2; !(n >= a) && Ee[n]; ) ++n;
          if (t = n << 1, t - e > 32 && Vr) return Vr.decode(F.subarray(e, t));
          for (var i = "", s = 0; !(s >= r / 2); ++s) {
            var _ = j[e + s * 2 >> 1];
            if (_ == 0) break;
            i += String.fromCharCode(_);
          }
          return i;
        }, In = (e, r, t) => {
          if (t ?? (t = 2147483647), t < 2) return 0;
          t -= 2;
          for (var n = r, a = t < e.length * 2 ? t / 2 : e.length, i = 0; i < a; ++i) {
            var s = e.charCodeAt(i);
            j[r >> 1] = s, r += 2;
          }
          return j[r >> 1] = 0, r - n;
        }, Ln = (e) => e.length * 2, bn = (e, r) => {
          for (var t = 0, n = ""; !(t >= r / 4); ) {
            var a = v[e + t * 4 >> 2];
            if (a == 0) break;
            if (++t, a >= 65536) {
              var i = a - 65536;
              n += String.fromCharCode(55296 | i >> 10, 56320 | i & 1023);
            } else
              n += String.fromCharCode(a);
          }
          return n;
        }, kn = (e, r, t) => {
          if (t ?? (t = 2147483647), t < 4) return 0;
          for (var n = r, a = n + t - 4, i = 0; i < e.length; ++i) {
            var s = e.charCodeAt(i);
            if (s >= 55296 && s <= 57343) {
              var _ = e.charCodeAt(++i);
              s = 65536 + ((s & 1023) << 10) | _ & 1023;
            }
            if (v[r >> 2] = s, r += 4, r + 4 > a) break;
          }
          return v[r >> 2] = 0, r - n;
        }, Fn = (e) => {
          for (var r = 0, t = 0; t < e.length; ++t) {
            var n = e.charCodeAt(t);
            n >= 55296 && n <= 57343 && ++t, r += 4;
          }
          return r;
        }, Mn = (e, r, t) => {
          t = b(t);
          var n, a, i, s;
          r === 2 ? (n = Nn, a = In, s = Ln, i = (_) => Ee[_ >> 1]) : r === 4 && (n = bn, a = kn, s = Fn, i = (_) => g[_ >> 2]), x(e, { name: t, fromWireType: (_) => {
            for (var l = g[_ >> 2], c, f = _ + 4, E = 0; E <= l; ++E) {
              var D = _ + 4 + E * r;
              if (E == l || i(D) == 0) {
                var d = D - f, T = n(f, d);
                c === void 0 ? c = T : (c += "\0", c += T), f = D + r;
              }
            }
            return Z(_), c;
          }, toWireType: (_, l) => {
            typeof l != "string" && m(`Cannot pass non-string to C++ string type ${t}`);
            var c = s(l), f = cr(4 + c + r);
            return g[f >> 2] = c / r, a(l, f + 4, c + r), _ !== null && _.push(Z, f), f;
          }, argPackAdvance: H, readValueFromPointer: Me, destructorFunction(_) {
            Z(_);
          } });
        }, $n = (e, r) => {
          r = b(r), x(e, { isVoid: !0, name: r, argPackAdvance: 0, fromWireType: () => {
          }, toWireType: (t, n) => {
          } });
        }, Kr = () => je || "./this.program", Bn = (e, r) => q(Kr(), e, r), Xr = 0, xn = () => {
          yr = !1, Xr = 0;
        }, Un = {}, zr = (e) => {
          var r = Un[e];
          return r === void 0 ? b(e) : r;
        }, lr = [], jn = (e, r, t, n, a) => (e = lr[e], r = M.toValue(r), t = zr(t), e(r, r[t], n, a)), Hn = (e) => {
          var r = lr.length;
          return lr.push(e), r;
        }, Vn = (e, r) => {
          for (var t = new Array(e), n = 0; n < e; ++n)
            t[n] = _r(g[r + n * 4 >> 2], "parameter " + n);
          return t;
        }, Kn = (e, r, t) => {
          var n = [], a = e.toWireType(n, t);
          return n.length && (g[r >> 2] = M.toHandle(n)), a;
        }, Xn = (e, r, t) => {
          var n = Vn(e, r), a = n.shift();
          e--;
          var i = `return function (obj, func, destructorsRef, args) {
`, s = 0, _ = [];
          t === 0 && _.push("obj");
          for (var l = ["retType"], c = [a], f = 0; f < e; ++f)
            _.push("arg" + f), l.push("argType" + f), c.push(n[f]), i += `  var arg${f} = argType${f}.readValueFromPointer(args${s ? "+" + s : ""});
`, s += n[f].argPackAdvance;
          var E = t === 1 ? "new func" : "func.call";
          i += `  var rv = ${E}(${_.join(", ")});
`, a.isVoid || (l.push("emval_returnValue"), c.push(Kn), i += `  return emval_returnValue(retType, destructorsRef, rv);
`), i += `};
`, l.push(i);
          var D = Hr(Function, l)(...c), d = `methodCaller<(${n.map((T) => T.name).join(", ")}) => ${a.name}>`;
          return Hn(ue(d, D));
        }, zn = (e) => {
          e > 9 && (X[e + 1] += 1);
        }, Jn = () => M.toHandle([]), qn = (e) => M.toHandle(zr(e)), Zn = () => M.toHandle({}), Qn = (e) => {
          var r = M.toValue(e);
          ar(r), sr(e);
        }, eo = (e, r, t) => {
          e = M.toValue(e), r = M.toValue(r), t = M.toValue(t), e[r] = t;
        }, ro = (e, r) => {
          e = _r(e, "_emval_take_value");
          var t = e.readValueFromPointer(r);
          return M.toHandle(t);
        }, to = 9007199254740992, no = -9007199254740992, ur = (e) => e < no || e > to ? NaN : Number(e);
        function oo(e, r) {
          e = ur(e);
          var t = new Date(e * 1e3);
          v[r >> 2] = t.getUTCSeconds(), v[r + 4 >> 2] = t.getUTCMinutes(), v[r + 8 >> 2] = t.getUTCHours(), v[r + 12 >> 2] = t.getUTCDate(), v[r + 16 >> 2] = t.getUTCMonth(), v[r + 20 >> 2] = t.getUTCFullYear() - 1900, v[r + 24 >> 2] = t.getUTCDay();
          var n = Date.UTC(t.getUTCFullYear(), 0, 1, 0, 0, 0, 0), a = (t.getTime() - n) / (1e3 * 60 * 60 * 24) | 0;
          v[r + 28 >> 2] = a;
        }
        var ao = (e) => e % 4 === 0 && (e % 100 !== 0 || e % 400 === 0), io = [0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335], so = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334], Jr = (e) => {
          var r = ao(e.getFullYear()), t = r ? io : so, n = t[e.getMonth()] + e.getDate() - 1;
          return n;
        };
        function _o(e, r) {
          e = ur(e);
          var t = new Date(e * 1e3);
          v[r >> 2] = t.getSeconds(), v[r + 4 >> 2] = t.getMinutes(), v[r + 8 >> 2] = t.getHours(), v[r + 12 >> 2] = t.getDate(), v[r + 16 >> 2] = t.getMonth(), v[r + 20 >> 2] = t.getFullYear() - 1900, v[r + 24 >> 2] = t.getDay();
          var n = Jr(t) | 0;
          v[r + 28 >> 2] = n, v[r + 36 >> 2] = -(t.getTimezoneOffset() * 60);
          var a = new Date(t.getFullYear(), 0, 1), i = new Date(t.getFullYear(), 6, 1).getTimezoneOffset(), s = a.getTimezoneOffset(), _ = (i != s && t.getTimezoneOffset() == Math.min(s, i)) | 0;
          v[r + 32 >> 2] = _;
        }
        var lo = function(e) {
          var r = (() => {
            var t = new Date(v[e + 20 >> 2] + 1900, v[e + 16 >> 2], v[e + 12 >> 2], v[e + 8 >> 2], v[e + 4 >> 2], v[e >> 2], 0), n = v[e + 32 >> 2], a = t.getTimezoneOffset(), i = new Date(t.getFullYear(), 0, 1), s = new Date(t.getFullYear(), 6, 1).getTimezoneOffset(), _ = i.getTimezoneOffset(), l = Math.min(_, s);
            if (n < 0)
              v[e + 32 >> 2] = +(s != _ && l == a);
            else if (n > 0 != (l == a)) {
              var c = Math.max(_, s), f = n > 0 ? l : c;
              t.setTime(t.getTime() + (f - a) * 6e4);
            }
            v[e + 24 >> 2] = t.getDay();
            var E = Jr(t) | 0;
            v[e + 28 >> 2] = E, v[e >> 2] = t.getSeconds(), v[e + 4 >> 2] = t.getMinutes(), v[e + 8 >> 2] = t.getHours(), v[e + 12 >> 2] = t.getDate(), v[e + 16 >> 2] = t.getMonth(), v[e + 20 >> 2] = t.getYear();
            var D = t.getTime();
            return isNaN(D) ? -1 : D / 1e3;
          })();
          return BigInt(r);
        }, De = {}, qr = (e) => {
          if (e instanceof Pr || e == "unwind")
            return Ce;
          He(1, e);
        }, Zr = () => yr || Xr > 0, Qr = (e) => {
          var r;
          Ce = e, Zr() || ((r = u.onExit) == null || r.call(u, e), Oe = !0), He(e, new Pr(e));
        }, uo = (e, r) => {
          Ce = e, Qr(e);
        }, co = uo, fo = () => {
          if (!Zr())
            try {
              co(Ce);
            } catch (e) {
              qr(e);
            }
        }, Eo = (e) => {
          if (!Oe)
            try {
              e(), fo();
            } catch (r) {
              qr(r);
            }
        }, vo = () => performance.now(), To = (e, r) => {
          if (De[e] && (clearTimeout(De[e].id), delete De[e]), !r) return 0;
          var t = setTimeout(() => {
            delete De[e], Eo(() => jo(e, vo()));
          }, r);
          return De[e] = { id: t, timeout_ms: r }, 0;
        }, ho = (e, r, t, n) => {
          var a = (/* @__PURE__ */ new Date()).getFullYear(), i = new Date(a, 0, 1), s = new Date(a, 6, 1), _ = i.getTimezoneOffset(), l = s.getTimezoneOffset(), c = Math.max(_, l);
          g[e >> 2] = c * 60, v[r >> 2] = +(_ != l);
          var f = (d) => {
            var T = d >= 0 ? "-" : "+", y = Math.abs(d), I = String(Math.floor(y / 60)).padStart(2, "0"), G = String(y % 60).padStart(2, "0");
            return `UTC${T}${I}${G}`;
          }, E = f(_), D = f(l);
          l < _ ? (q(E, t, 17), q(D, n, 17)) : (q(E, n, 17), q(D, t, 17));
        }, po = () => Date.now(), Do = () => 2147483648, Po = (e) => {
          var r = we.buffer, t = (e - r.byteLength + 65535) / 65536 | 0;
          try {
            return we.grow(t), pr(), 1;
          } catch {
          }
        }, mo = (e) => {
          var r = F.length;
          e >>>= 0;
          var t = Do();
          if (e > t)
            return !1;
          for (var n = 1; n <= 4; n *= 2) {
            var a = r * (1 + 0.2 / n);
            a = Math.min(a, e + 100663296);
            var i = Math.min(t, Gr(Math.max(e, a), 65536)), s = Po(i);
            if (s)
              return !0;
          }
          return !1;
        }, xe = {}, Pe = () => {
          if (!Pe.strings) {
            var e = (typeof navigator == "object" && navigator.languages && navigator.languages[0] || "C").replace("-", "_") + ".UTF-8", r = { USER: "web_user", LOGNAME: "web_user", PATH: "/", PWD: "/", HOME: "/home/web_user", LANG: e, _: Kr() };
            for (var t in xe)
              xe[t] === void 0 ? delete r[t] : r[t] = xe[t];
            var n = [];
            for (var t in r)
              n.push(`${t}=${r[t]}`);
            Pe.strings = n;
          }
          return Pe.strings;
        }, go = (e, r) => {
          for (var t = 0; t < e.length; ++t)
            W[r++] = e.charCodeAt(t);
          W[r] = 0;
        }, Ao = (e, r) => {
          var t = 0;
          return Pe().forEach((n, a) => {
            var i = r + t;
            g[e + a * 4 >> 2] = i, go(n, i), t += n.length + 1;
          }), 0;
        }, yo = (e, r) => {
          var t = Pe();
          g[e >> 2] = t.length;
          var n = 0;
          return t.forEach((a) => n += a.length + 1), g[r >> 2] = n, 0;
        };
        function wo(e) {
          try {
            var r = S.getStreamFromFD(e);
            return o.close(r), 0;
          } catch (t) {
            if (typeof o > "u" || t.name !== "ErrnoError") throw t;
            return t.errno;
          }
        }
        var Oo = (e, r, t, n) => {
          for (var a = 0, i = 0; i < t; i++) {
            var s = g[r >> 2], _ = g[r + 4 >> 2];
            r += 8;
            var l = o.read(e, W, s, _, n);
            if (l < 0) return -1;
            if (a += l, l < _) break;
          }
          return a;
        };
        function Co(e, r, t, n) {
          try {
            var a = S.getStreamFromFD(e), i = Oo(a, r, t);
            return g[n >> 2] = i, 0;
          } catch (s) {
            if (typeof o > "u" || s.name !== "ErrnoError") throw s;
            return s.errno;
          }
        }
        function Yo(e, r, t, n) {
          r = ur(r);
          try {
            if (isNaN(r)) return 61;
            var a = S.getStreamFromFD(e);
            return o.llseek(a, r, t), K[n >> 3] = BigInt(a.position), a.getdents && r === 0 && t === 0 && (a.getdents = null), 0;
          } catch (i) {
            if (typeof o > "u" || i.name !== "ErrnoError") throw i;
            return i.errno;
          }
        }
        var Ro = (e, r, t, n) => {
          for (var a = 0, i = 0; i < t; i++) {
            var s = g[r >> 2], _ = g[r + 4 >> 2];
            r += 8;
            var l = o.write(e, W, s, _, n);
            if (l < 0) return -1;
            if (a += l, l < _)
              break;
          }
          return a;
        };
        function So(e, r, t, n) {
          try {
            var a = S.getStreamFromFD(e), i = Ro(a, r, t);
            return g[n >> 2] = i, 0;
          } catch (s) {
            if (typeof o > "u" || s.name !== "ErrnoError") throw s;
            return s.errno;
          }
        }
        var et = (e) => {
          var r = u["_" + e];
          return r;
        }, Go = (e, r) => {
          W.set(e, r);
        }, rt = (e) => zo(e), Wo = (e) => {
          var r = Te(e) + 1, t = rt(r);
          return q(e, t, r), t;
        }, tt = (e, r, t, n, a) => {
          var i = { string: (T) => {
            var y = 0;
            return T != null && T !== 0 && (y = Wo(T)), y;
          }, array: (T) => {
            var y = rt(T.length);
            return Go(T, y), y;
          } };
          function s(T) {
            return r === "string" ? re(T) : r === "boolean" ? !!T : T;
          }
          var _ = et(e), l = [], c = 0;
          if (n)
            for (var f = 0; f < n.length; f++) {
              var E = i[t[f]];
              E ? (c === 0 && (c = wt()), l[f] = E(n[f])) : l[f] = n[f];
            }
          var D = _(...l);
          function d(T) {
            return c !== 0 && yt(c), s(T);
          }
          return D = d(D), D;
        }, No = (e, r, t, n) => {
          var a = !t || t.every((s) => s === "number" || s === "boolean"), i = r !== "string";
          return i && a && !n ? et(e) : (...s) => tt(e, r, t, s);
        }, Io = (e) => {
          var r = Te(e) + 1, t = cr(r);
          return t && q(e, t, r), t;
        }, Lo = (e) => Vo(e), bo = Lo;
        u._setTempRet0 = bo;
        var ko = (e) => Ko(), Fo = ko;
        u._getTempRet0 = Fo;
        var Mo = () => Ze;
        u.___cxa_uncaught_exceptions = Mo;
        var ce = [], $o = () => {
          if (!ce.length)
            return 0;
          var e = ce[ce.length - 1];
          return qo(e.excPtr), e.excPtr;
        };
        u.___cxa_current_primary_exception = $o;
        var Bo = () => {
          var e = ce.pop();
          e || ve("no exception to throw");
          var r = e.excPtr;
          throw e.get_rethrown() || (ce.push(e), e.set_rethrown(!0), e.set_caught(!1), Ze++), Se = r, Se;
        }, xo = (e) => {
          if (e) {
            var r = new Yr(e);
            ce.push(r), r.set_rethrown(!0), Bo();
          }
        };
        u.___cxa_rethrow_primary_exception = xo, o.createPreloadedFile = bt, o.staticInit(), P.doesNotExistError = new o.ErrnoError(44), P.doesNotExistError.stack = "<generic error, no stack>", Vt(), he = u.BindingError = class extends Error {
          constructor(r) {
            super(r), this.name = "BindingError";
          }
        }, Ir = u.InternalError = class extends Error {
          constructor(r) {
            super(r), this.name = "InternalError";
          }
        }, tn(), fn(), Br = u.UnboundTypeError = En(Error, "UnboundTypeError"), hn();
        var nt = { __assert_fail: Ot, __call_sighandler: Ct, __cxa_throw: Yt, __syscall_fcntl64: Ft, __syscall_fstat64: Mt, __syscall_ioctl: $t, __syscall_lstat64: Bt, __syscall_newfstatat: xt, __syscall_openat: Ut, __syscall_stat64: jt, _abort_js: Ht, _embind_register_bigint: Xt, _embind_register_bool: zt, _embind_register_class: dn, _embind_register_class_property: vn, _embind_register_emval: Dn, _embind_register_enum: mn, _embind_register_enum_value: gn, _embind_register_float: yn, _embind_register_function: Rn, _embind_register_integer: Sn, _embind_register_memory_view: Gn, _embind_register_std_string: Wn, _embind_register_std_wstring: Mn, _embind_register_void: $n, _emscripten_get_progname: Bn, _emscripten_runtime_keepalive_clear: xn, _emval_call_method: jn, _emval_decref: sr, _emval_get_method_caller: Xn, _emval_incref: zn, _emval_new_array: Jn, _emval_new_cstring: qn, _emval_new_object: Zn, _emval_run_destructors: Qn, _emval_set_property: eo, _emval_take_value: ro, _gmtime_js: oo, _localtime_js: _o, _mktime_js: lo, _setitimer_js: To, _tzset_js: ho, emscripten_date_now: po, emscripten_resize_heap: mo, environ_get: Ao, environ_sizes_get: yo, fd_close: wo, fd_read: Co, fd_seek: Yo, fd_write: So, proc_exit: Qr }, A = await Pt();
        A.__wasm_call_ctors;
        var cr = A.malloc, Z = A.free;
        A.memcmp, A.calloc, A.realloc, A.__errno_location, A.fileno;
        var Uo = A.__getTypeName;
        A._emval_coro_resume, A.memcpy, A.htonl, A.htons, A.ntohs;
        var jo = A._emscripten_timeout;
        A.strerror;
        var Ho = A.emscripten_builtin_memalign;
        A.setThrew;
        var Vo = A._emscripten_tempret_set, Ko = A._emscripten_tempret_get, Xo = A._emscripten_stack_restore, zo = A._emscripten_stack_alloc, Jo = A.emscripten_stack_get_current;
        A.__cxa_decrement_exception_refcount;
        var qo = A.__cxa_increment_exception_refcount;
        A.__cxa_demangle, A.__cxa_can_catch, A.__cxa_get_exception_ptr, u.ENV = xe, u.ccall = tt, u.cwrap = No, u.setValue = At, u.UTF8ToString = re, u.stringToNewUTF8 = Io, u.FS = o;
        function fr() {
          if (ee > 0) {
            de = fr;
            return;
          }
          if (ct(), ee > 0) {
            de = fr;
            return;
          }
          function e() {
            var r;
            u.calledRun = !0, !Oe && (ft(), vr(u), (r = u.onRuntimeInitialized) == null || r.call(u), Et());
          }
          u.setStatus ? (u.setStatus("Running..."), setTimeout(() => {
            setTimeout(() => u.setStatus(""), 1), e();
          }, 1)) : e();
        }
        if (u.preInit)
          for (typeof u.preInit == "function" && (u.preInit = [u.preInit]); u.preInit.length > 0; )
            u.preInit.pop()();
        return fr(), J = _t, J;
      };
    })();
    h.exports = Y, h.exports.default = Y;
  }(dr)), dr.exports;
}
var aa = oa();
const la = /* @__PURE__ */ ea(aa), ua = Object.freeze({
  DWG_SUPERTYPE_ENTITY: 0,
  DWG_SUPERTYPE_OBJECT: 1
}), k = Object.freeze({
  DWG_TYPE_UNUSED: 0,
  DWG_TYPE_TEXT: 1,
  DWG_TYPE_ATTRIB: 2,
  DWG_TYPE_ATTDEF: 3,
  DWG_TYPE_BLOCK: 4,
  DWG_TYPE_ENDBLK: 5,
  DWG_TYPE_SEQEND: 6,
  DWG_TYPE_INSERT: 7,
  DWG_TYPE_MINSERT: 8,
  // DWG_TYPE_TRACE_old: 0x09, /* old TRACE r10-r11 only */
  DWG_TYPE_VERTEX_2D: 10,
  DWG_TYPE_VERTEX_3D: 11,
  DWG_TYPE_VERTEX_MESH: 12,
  DWG_TYPE_VERTEX_PFACE: 13,
  DWG_TYPE_VERTEX_PFACE_FACE: 14,
  DWG_TYPE_POLYLINE_2D: 15,
  DWG_TYPE_POLYLINE_3D: 16,
  DWG_TYPE_ARC: 17,
  DWG_TYPE_CIRCLE: 18,
  DWG_TYPE_LINE: 19,
  DWG_TYPE_DIMENSION_ORDINATE: 20,
  DWG_TYPE_DIMENSION_LINEAR: 21,
  DWG_TYPE_DIMENSION_ALIGNED: 22,
  DWG_TYPE_DIMENSION_ANG3PT: 23,
  DWG_TYPE_DIMENSION_ANG2LN: 24,
  DWG_TYPE_DIMENSION_RADIUS: 25,
  DWG_TYPE_DIMENSION_DIAMETER: 26,
  DWG_TYPE_POINT: 27,
  DWG_TYPE__3DFACE: 28,
  DWG_TYPE_POLYLINE_PFACE: 29,
  DWG_TYPE_POLYLINE_MESH: 30,
  DWG_TYPE_SOLID: 31,
  DWG_TYPE_TRACE: 32,
  DWG_TYPE_SHAPE: 33,
  DWG_TYPE_VIEWPORT: 34,
  DWG_TYPE_ELLIPSE: 35,
  DWG_TYPE_SPLINE: 36,
  DWG_TYPE_REGION: 37,
  DWG_TYPE__3DSOLID: 38,
  DWG_TYPE_BODY: 39,
  DWG_TYPE_RAY: 40,
  DWG_TYPE_XLINE: 41,
  DWG_TYPE_DICTIONARY: 42,
  DWG_TYPE_OLEFRAME: 43,
  DWG_TYPE_MTEXT: 44,
  DWG_TYPE_LEADER: 45,
  DWG_TYPE_TOLERANCE: 46,
  DWG_TYPE_MLINE: 47,
  DWG_TYPE_BLOCK_CONTROL: 48,
  DWG_TYPE_BLOCK_HEADER: 49,
  DWG_TYPE_LAYER_CONTROL: 50,
  DWG_TYPE_LAYER: 51,
  DWG_TYPE_STYLE_CONTROL: 52,
  /* 52 SHAPEFILE_CONTROL */
  DWG_TYPE_STYLE: 53,
  /* DWG_TYPE_<UNKNOWN>: 0x36, */
  /* DWG_TYPE_<UNKNOWN>: 0x37, */
  DWG_TYPE_LTYPE_CONTROL: 56,
  DWG_TYPE_LTYPE: 57,
  /* DWG_TYPE_<UNKNOWN>: 0x3a, */
  /* DWG_TYPE_<UNKNOWN>: 0x3b, */
  DWG_TYPE_VIEW_CONTROL: 60,
  DWG_TYPE_VIEW: 61,
  DWG_TYPE_UCS_CONTROL: 62,
  DWG_TYPE_UCS: 63,
  DWG_TYPE_VPORT_CONTROL: 64,
  DWG_TYPE_VPORT: 65,
  DWG_TYPE_APPID_CONTROL: 66,
  DWG_TYPE_APPID: 67,
  DWG_TYPE_DIMSTYLE_CONTROL: 68,
  DWG_TYPE_DIMSTYLE: 69,
  DWG_TYPE_VX_CONTROL: 70,
  DWG_TYPE_VX_TABLE_RECORD: 71,
  DWG_TYPE_GROUP: 72,
  DWG_TYPE_MLINESTYLE: 73,
  DWG_TYPE_OLE2FRAME: 74,
  DWG_TYPE_DUMMY: 75,
  DWG_TYPE_LONG_TRANSACTION: 76,
  DWG_TYPE_LWPOLYLINE: 77,
  /* ?? */
  DWG_TYPE_HATCH: 78,
  DWG_TYPE_XRECORD: 79,
  DWG_TYPE_PLACEHOLDER: 80,
  DWG_TYPE_VBA_PROJECT: 81,
  DWG_TYPE_LAYOUT: 82,
  DWG_TYPE_PROXY_ENTITY: 498,
  /* 498 */
  DWG_TYPE_PROXY_OBJECT: 499,
  /* 499 */
  /* non-fixed types > 500. not stored as type, but as fixedtype */
  DWG_TYPE_ACDSRECORD: 500,
  DWG_TYPE_ACDSSCHEMA: 501,
  DWG_TYPE_ACMECOMMANDHISTORY: 502,
  DWG_TYPE_ACMESCOPE: 503,
  DWG_TYPE_ACMESTATEMGR: 504,
  DWG_TYPE_ACSH_BOOLEAN_CLASS: 505,
  DWG_TYPE_ACSH_BOX_CLASS: 506,
  DWG_TYPE_ACSH_BREP_CLASS: 507,
  DWG_TYPE_ACSH_CHAMFER_CLASS: 508,
  DWG_TYPE_ACSH_CONE_CLASS: 509,
  DWG_TYPE_ACSH_CYLINDER_CLASS: 510,
  DWG_TYPE_ACSH_EXTRUSION_CLASS: 511,
  DWG_TYPE_ACSH_FILLET_CLASS: 512,
  DWG_TYPE_ACSH_HISTORY_CLASS: 513,
  DWG_TYPE_ACSH_LOFT_CLASS: 514,
  DWG_TYPE_ACSH_PYRAMID_CLASS: 515,
  DWG_TYPE_ACSH_REVOLVE_CLASS: 516,
  DWG_TYPE_ACSH_SPHERE_CLASS: 517,
  DWG_TYPE_ACSH_SWEEP_CLASS: 518,
  DWG_TYPE_ACSH_TORUS_CLASS: 519,
  DWG_TYPE_ACSH_WEDGE_CLASS: 520,
  DWG_TYPE_ALDIMOBJECTCONTEXTDATA: 521,
  DWG_TYPE_ALIGNMENTPARAMETERENTITY: 522,
  DWG_TYPE_ANGDIMOBJECTCONTEXTDATA: 523,
  DWG_TYPE_ANNOTSCALEOBJECTCONTEXTDATA: 524,
  DWG_TYPE_ARCALIGNEDTEXT: 525,
  DWG_TYPE_ARC_DIMENSION: 526,
  DWG_TYPE_ASSOC2DCONSTRAINTGROUP: 527,
  DWG_TYPE_ASSOC3POINTANGULARDIMACTIONBODY: 528,
  DWG_TYPE_ASSOCACTION: 529,
  DWG_TYPE_ASSOCACTIONPARAM: 530,
  DWG_TYPE_ASSOCALIGNEDDIMACTIONBODY: 531,
  DWG_TYPE_ASSOCARRAYACTIONBODY: 532,
  DWG_TYPE_ASSOCARRAYMODIFYACTIONBODY: 533,
  DWG_TYPE_ASSOCARRAYMODIFYPARAMETERS: 534,
  DWG_TYPE_ASSOCARRAYPATHPARAMETERS: 535,
  DWG_TYPE_ASSOCARRAYPOLARPARAMETERS: 536,
  DWG_TYPE_ASSOCARRAYRECTANGULARPARAMETERS: 537,
  DWG_TYPE_ASSOCASMBODYACTIONPARAM: 538,
  DWG_TYPE_ASSOCBLENDSURFACEACTIONBODY: 539,
  DWG_TYPE_ASSOCCOMPOUNDACTIONPARAM: 540,
  DWG_TYPE_ASSOCDEPENDENCY: 541,
  DWG_TYPE_ASSOCDIMDEPENDENCYBODY: 542,
  DWG_TYPE_ASSOCEDGEACTIONPARAM: 543,
  DWG_TYPE_ASSOCEDGECHAMFERACTIONBODY: 544,
  DWG_TYPE_ASSOCEDGEFILLETACTIONBODY: 545,
  DWG_TYPE_ASSOCEXTENDSURFACEACTIONBODY: 546,
  DWG_TYPE_ASSOCEXTRUDEDSURFACEACTIONBODY: 547,
  DWG_TYPE_ASSOCFACEACTIONPARAM: 548,
  DWG_TYPE_ASSOCFILLETSURFACEACTIONBODY: 549,
  DWG_TYPE_ASSOCGEOMDEPENDENCY: 550,
  DWG_TYPE_ASSOCLOFTEDSURFACEACTIONBODY: 551,
  DWG_TYPE_ASSOCMLEADERACTIONBODY: 552,
  DWG_TYPE_ASSOCNETWORK: 553,
  DWG_TYPE_ASSOCNETWORKSURFACEACTIONBODY: 554,
  DWG_TYPE_ASSOCOBJECTACTIONPARAM: 555,
  DWG_TYPE_ASSOCOFFSETSURFACEACTIONBODY: 556,
  DWG_TYPE_ASSOCORDINATEDIMACTIONBODY: 557,
  DWG_TYPE_ASSOCOSNAPPOINTREFACTIONPARAM: 558,
  DWG_TYPE_ASSOCPATCHSURFACEACTIONBODY: 559,
  DWG_TYPE_ASSOCPATHACTIONPARAM: 560,
  DWG_TYPE_ASSOCPERSSUBENTMANAGER: 561,
  DWG_TYPE_ASSOCPLANESURFACEACTIONBODY: 562,
  DWG_TYPE_ASSOCPOINTREFACTIONPARAM: 563,
  DWG_TYPE_ASSOCRESTOREENTITYSTATEACTIONBODY: 564,
  DWG_TYPE_ASSOCREVOLVEDSURFACEACTIONBODY: 565,
  DWG_TYPE_ASSOCROTATEDDIMACTIONBODY: 566,
  DWG_TYPE_ASSOCSWEPTSURFACEACTIONBODY: 567,
  DWG_TYPE_ASSOCTRIMSURFACEACTIONBODY: 568,
  DWG_TYPE_ASSOCVALUEDEPENDENCY: 569,
  DWG_TYPE_ASSOCVARIABLE: 570,
  DWG_TYPE_ASSOCVERTEXACTIONPARAM: 571,
  DWG_TYPE_BASEPOINTPARAMETERENTITY: 572,
  DWG_TYPE_BLKREFOBJECTCONTEXTDATA: 573,
  DWG_TYPE_BLOCKALIGNEDCONSTRAINTPARAMETER: 574,
  DWG_TYPE_BLOCKALIGNMENTGRIP: 575,
  DWG_TYPE_BLOCKALIGNMENTPARAMETER: 576,
  DWG_TYPE_BLOCKANGULARCONSTRAINTPARAMETER: 577,
  DWG_TYPE_BLOCKARRAYACTION: 578,
  DWG_TYPE_BLOCKBASEPOINTPARAMETER: 579,
  DWG_TYPE_BLOCKDIAMETRICCONSTRAINTPARAMETER: 580,
  DWG_TYPE_BLOCKFLIPACTION: 581,
  DWG_TYPE_BLOCKFLIPGRIP: 582,
  DWG_TYPE_BLOCKFLIPPARAMETER: 583,
  DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT: 584,
  DWG_TYPE_BLOCKHORIZONTALCONSTRAINTPARAMETER: 585,
  DWG_TYPE_BLOCKLINEARCONSTRAINTPARAMETER: 586,
  DWG_TYPE_BLOCKLINEARGRIP: 587,
  DWG_TYPE_BLOCKLINEARPARAMETER: 588,
  DWG_TYPE_BLOCKLOOKUPACTION: 589,
  DWG_TYPE_BLOCKLOOKUPGRIP: 590,
  DWG_TYPE_BLOCKLOOKUPPARAMETER: 591,
  DWG_TYPE_BLOCKMOVEACTION: 592,
  DWG_TYPE_BLOCKPARAMDEPENDENCYBODY: 593,
  DWG_TYPE_BLOCKPOINTPARAMETER: 594,
  DWG_TYPE_BLOCKPOLARGRIP: 595,
  DWG_TYPE_BLOCKPOLARPARAMETER: 596,
  DWG_TYPE_BLOCKPOLARSTRETCHACTION: 597,
  DWG_TYPE_BLOCKPROPERTIESTABLE: 598,
  DWG_TYPE_BLOCKPROPERTIESTABLEGRIP: 599,
  DWG_TYPE_BLOCKRADIALCONSTRAINTPARAMETER: 600,
  DWG_TYPE_BLOCKREPRESENTATION: 601,
  DWG_TYPE_BLOCKROTATEACTION: 602,
  DWG_TYPE_BLOCKROTATIONGRIP: 603,
  DWG_TYPE_BLOCKROTATIONPARAMETER: 604,
  DWG_TYPE_BLOCKSCALEACTION: 605,
  DWG_TYPE_BLOCKSTRETCHACTION: 606,
  DWG_TYPE_BLOCKUSERPARAMETER: 607,
  DWG_TYPE_BLOCKVERTICALCONSTRAINTPARAMETER: 608,
  DWG_TYPE_BLOCKVISIBILITYGRIP: 609,
  DWG_TYPE_BLOCKVISIBILITYPARAMETER: 610,
  DWG_TYPE_BLOCKXYGRIP: 611,
  DWG_TYPE_BLOCKXYPARAMETER: 612,
  DWG_TYPE_CAMERA: 613,
  DWG_TYPE_CELLSTYLEMAP: 614,
  DWG_TYPE_CONTEXTDATAMANAGER: 615,
  DWG_TYPE_CSACDOCUMENTOPTIONS: 616,
  DWG_TYPE_CURVEPATH: 617,
  DWG_TYPE_DATALINK: 618,
  DWG_TYPE_DATATABLE: 619,
  DWG_TYPE_DBCOLOR: 620,
  DWG_TYPE_DETAILVIEWSTYLE: 621,
  DWG_TYPE_DGNDEFINITION: 622,
  DWG_TYPE_DGNUNDERLAY: 623,
  DWG_TYPE_DICTIONARYVAR: 624,
  DWG_TYPE_DICTIONARYWDFLT: 625,
  DWG_TYPE_DIMASSOC: 626,
  DWG_TYPE_DMDIMOBJECTCONTEXTDATA: 627,
  DWG_TYPE_DWFDEFINITION: 628,
  DWG_TYPE_DWFUNDERLAY: 629,
  DWG_TYPE_DYNAMICBLOCKPROXYNODE: 630,
  DWG_TYPE_DYNAMICBLOCKPURGEPREVENTER: 631,
  DWG_TYPE_EVALUATION_GRAPH: 632,
  DWG_TYPE_EXTRUDEDSURFACE: 633,
  DWG_TYPE_FCFOBJECTCONTEXTDATA: 634,
  DWG_TYPE_FIELD: 635,
  DWG_TYPE_FIELDLIST: 636,
  DWG_TYPE_FLIPPARAMETERENTITY: 637,
  DWG_TYPE_GEODATA: 638,
  DWG_TYPE_GEOMAPIMAGE: 639,
  DWG_TYPE_GEOPOSITIONMARKER: 640,
  DWG_TYPE_GRADIENT_BACKGROUND: 641,
  DWG_TYPE_GROUND_PLANE_BACKGROUND: 642,
  DWG_TYPE_HELIX: 643,
  DWG_TYPE_IBL_BACKGROUND: 644,
  DWG_TYPE_IDBUFFER: 645,
  DWG_TYPE_IMAGE: 646,
  DWG_TYPE_IMAGEDEF: 647,
  DWG_TYPE_IMAGEDEF_REACTOR: 648,
  DWG_TYPE_IMAGE_BACKGROUND: 649,
  DWG_TYPE_INDEX: 650,
  DWG_TYPE_LARGE_RADIAL_DIMENSION: 651,
  DWG_TYPE_LAYERFILTER: 652,
  DWG_TYPE_LAYER_INDEX: 653,
  DWG_TYPE_LAYOUTPRINTCONFIG: 654,
  DWG_TYPE_LEADEROBJECTCONTEXTDATA: 655,
  DWG_TYPE_LIGHT: 656,
  DWG_TYPE_LIGHTLIST: 657,
  DWG_TYPE_LINEARPARAMETERENTITY: 658,
  DWG_TYPE_LOFTEDSURFACE: 659,
  DWG_TYPE_MATERIAL: 660,
  DWG_TYPE_MENTALRAYRENDERSETTINGS: 661,
  DWG_TYPE_MESH: 662,
  DWG_TYPE_MLEADEROBJECTCONTEXTDATA: 663,
  DWG_TYPE_MLEADERSTYLE: 664,
  DWG_TYPE_MOTIONPATH: 665,
  DWG_TYPE_MPOLYGON: 666,
  DWG_TYPE_MTEXTATTRIBUTEOBJECTCONTEXTDATA: 667,
  DWG_TYPE_MTEXTOBJECTCONTEXTDATA: 668,
  DWG_TYPE_MULTILEADER: 669,
  DWG_TYPE_NAVISWORKSMODEL: 670,
  DWG_TYPE_NAVISWORKSMODELDEF: 671,
  DWG_TYPE_NPOCOLLECTION: 672,
  DWG_TYPE_NURBSURFACE: 673,
  DWG_TYPE_OBJECT_PTR: 674,
  DWG_TYPE_ORDDIMOBJECTCONTEXTDATA: 675,
  DWG_TYPE_PARTIAL_VIEWING_INDEX: 676,
  //DWG_TYPE_PARTIAL_VIEWING_FILTER,
  DWG_TYPE_PDFDEFINITION: 677,
  DWG_TYPE_PDFUNDERLAY: 678,
  DWG_TYPE_PERSUBENTMGR: 679,
  DWG_TYPE_PLANESURFACE: 680,
  DWG_TYPE_PLOTSETTINGS: 681,
  DWG_TYPE_POINTCLOUD: 682,
  DWG_TYPE_POINTCLOUDCOLORMAP: 683,
  DWG_TYPE_POINTCLOUDDEF: 684,
  DWG_TYPE_POINTCLOUDDEFEX: 685,
  DWG_TYPE_POINTCLOUDDEF_REACTOR: 686,
  DWG_TYPE_POINTCLOUDDEF_REACTOR_EX: 687,
  DWG_TYPE_POINTCLOUDEX: 688,
  DWG_TYPE_POINTPARAMETERENTITY: 689,
  DWG_TYPE_POINTPATH: 690,
  DWG_TYPE_POLARGRIPENTITY: 691,
  DWG_TYPE_RADIMLGOBJECTCONTEXTDATA: 692,
  DWG_TYPE_RADIMOBJECTCONTEXTDATA: 693,
  DWG_TYPE_RAPIDRTRENDERSETTINGS: 694,
  DWG_TYPE_RASTERVARIABLES: 695,
  DWG_TYPE_RENDERENTRY: 696,
  DWG_TYPE_RENDERENVIRONMENT: 697,
  DWG_TYPE_RENDERGLOBAL: 698,
  DWG_TYPE_RENDERSETTINGS: 699,
  DWG_TYPE_REVOLVEDSURFACE: 700,
  DWG_TYPE_ROTATIONPARAMETERENTITY: 701,
  DWG_TYPE_RTEXT: 702,
  DWG_TYPE_SCALE: 703,
  DWG_TYPE_SECTIONOBJECT: 704,
  DWG_TYPE_SECTIONVIEWSTYLE: 705,
  DWG_TYPE_SECTION_MANAGER: 706,
  DWG_TYPE_SECTION_SETTINGS: 707,
  DWG_TYPE_SKYLIGHT_BACKGROUND: 708,
  DWG_TYPE_SOLID_BACKGROUND: 709,
  DWG_TYPE_SORTENTSTABLE: 710,
  DWG_TYPE_SPATIAL_FILTER: 711,
  DWG_TYPE_SPATIAL_INDEX: 712,
  DWG_TYPE_SUN: 713,
  DWG_TYPE_SUNSTUDY: 714,
  DWG_TYPE_SWEPTSURFACE: 715,
  DWG_TYPE_TABLE: 716,
  DWG_TYPE_TABLECONTENT: 717,
  DWG_TYPE_TABLEGEOMETRY: 718,
  DWG_TYPE_TABLESTYLE: 719,
  DWG_TYPE_TEXTOBJECTCONTEXTDATA: 720,
  DWG_TYPE_TVDEVICEPROPERTIES: 721,
  DWG_TYPE_VISIBILITYGRIPENTITY: 722,
  DWG_TYPE_VISIBILITYPARAMETERENTITY: 723,
  DWG_TYPE_VISUALSTYLE: 724,
  DWG_TYPE_WIPEOUT: 725,
  DWG_TYPE_WIPEOUTVARIABLES: 726,
  DWG_TYPE_XREFPANELOBJECT: 727,
  DWG_TYPE_XYPARAMETERENTITY: 728,
  DWG_TYPE_BREAKDATA: 729,
  DWG_TYPE_BREAKPOINTREF: 730,
  DWG_TYPE_FLIPGRIPENTITY: 731,
  DWG_TYPE_LINEARGRIPENTITY: 732,
  DWG_TYPE_ROTATIONGRIPENTITY: 733,
  DWG_TYPE_XYGRIPENTITY: 734,
  /* preR13 entities */
  DWG_TYPE__3DLINE: 735,
  DWG_TYPE_REPEAT: 736,
  DWG_TYPE_ENDREP: 737,
  DWG_TYPE_JUMP: 738,
  /* pre2.0 entities */
  DWG_TYPE_LOAD: 739,
  // after 1.0 add new types here for binary compat
  DWG_TYPE_FREED: 65533,
  DWG_TYPE_UNKNOWN_ENT: 65534,
  DWG_TYPE_UNKNOWN_OBJ: 65535
}), ca = Object.fromEntries(
  Object.entries(k).map(([h, p]) => [p, h])
), st = Object.freeze({
  DWG: 0,
  DXF: 1
});
function fa(h) {
  h.dwg_read_data = function(p, Y) {
    if (Y == st.DWG) {
      const O = "tmp.dwg";
      this.FS.writeFile(O, new Uint8Array(p));
      const R = this.dwg_read_file(O);
      return R.error != 0 && console.log("Failed to open dwg file with error code: ", R.error), this.FS.unlink(O), R.data;
    } else if (Y == st.DXF) {
      const O = "tmp.dxf";
      this.FS.writeFile(O, new Uint8Array(p));
      const R = this.dxf_read_file(O);
      return R.error != 0 && console.log("Failed to open dxf file with error code: ", R.error), this.FS.unlink(O), R.data;
    }
  }, h.dwg_getall_entitie_in_model_space = function(p) {
    const Y = this.dwg_model_space_object(p), O = [];
    let R = this.get_first_owned_entity(Y);
    for (; R; )
      O.push(R), R = this.get_next_owned_entity(Y);
    return O;
  }, h.dwg_getall_object_by_type = function(p, Y) {
    const O = this.dwg_get_num_objects(p), R = [];
    for (let V = 0; V < O; V++) {
      const J = this.dwg_get_object(p, V), u = this.dwg_object_to_object_tio(J);
      u && this.dwg_object_get_fixedtype(J) == Y && R.push(u);
    }
    return R;
  }, h.dwg_getall_entity_by_type = function(p, Y) {
    const O = this.dwg_get_num_objects(p), R = [];
    for (let V = 0; V < O; V++) {
      const J = this.dwg_get_object(p, V), u = this.dwg_object_to_entity_tio(J);
      u && this.dwg_object_get_fixedtype(J) == Y && R.push(u);
    }
    return R;
  }, h.dwg_getall_LAYER = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_LAYER);
  }, h.dwg_getall_LTYPE = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_LTYPE);
  }, h.dwg_getall_STYLE = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_STYLE);
  }, h.dwg_getall_DIMSTYLE = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_DIMSTYLE);
  }, h.dwg_getall_VPORT = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_VPORT);
  }, h.dwg_getall_LAYOUT = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_LAYOUT);
  }, h.dwg_getall_BLOCK = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_BLOCK);
  }, h.dwg_getall_BLOCK_HEADER = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_BLOCK_HEADER);
  }, h.dwg_getall_IMAGEDEF = function(p) {
    return h.dwg_getall_object_by_type(p, k.DWG_TYPE_IMAGEDEF);
  }, h.dwg_getall_VERTEX_2D = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_VERTEX_2D);
  }, h.dwg_getall_VERTEX_3D = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_VERTEX_3D);
  }, h.dwg_getall_POLYLINE_2D = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_POLYLINE_2D);
  }, h.dwg_getall_POLYLINE_3D = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_POLYLINE_3D);
  }, h.dwg_getall_IMAGE = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_IMAGE);
  }, h.dwg_getall_LWPOLYLINE = function(p) {
    return h.dwg_getall_entity_by_type(p, k.DWG_TYPE_LWPOLYLINE);
  };
}
export {
  st as Dwg_File_Type,
  ua as Dwg_Object_Supertype,
  k as Dwg_Object_Type,
  ca as Dwg_Object_Type_Inverted,
  la as createModule,
  fa as extend_lib
};
