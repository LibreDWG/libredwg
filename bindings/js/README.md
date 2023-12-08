These are the (initial) Javascript bindings for GNU LibreDWG.
You can convert between DWG, DXF, DXFB (binary) and JSON.
It is based on WASM (emscripten).

Please wait for the official release before using this in production.

GNU LibreDWG is a free C library to handle DWG files. It aims to be a free replacement for the OpenDWG libraries. DWG is the native file format of AutoCAD.

LibreDWG is based on LibDWG, originally written by Felipe Castro.

LibreDWG is in beta development stage. Not all planned features are yet completed, but the API should stay mostly stable. At the moment our decoder (i.e. reader) is done, just some very advanced R2010+ and pre-R13 entities fail to read and are skipped over. The writer is good enough for R2000. Among the example applications we wrote using LibreDWG is a reader (from dwg, dxf, json), a writer (convert from dwg, dxf, json or add from scratch), a rewriter (i.e. saveas), an initial SVG and Postscript conversion, converters from and to DXF and JSON, dwggrep to search for text, and dwglayer to print the list of layers.

More information: https://www.gnu.org/software/libredwg/

Usage:

```js
import { convert } from 'libredwg'

let input = /* ... obtain a DWG ArrayBuffer using upload, fetch or IndexedDB */

/*
 * Available inputs/outputs (case sensitive): dwg, dxf, dxfb, json, jsonString
 *   jsonString is json as a string, without the cost of internal (de)serialization
 * Available DWG/DXF/DXFB versions: [TODO], true = detect / don't care, falsy = no export
 * Available log levels: falsy: none, 1/error, 2/warn, 3/info, 4/trace, 5/insane, 9/all
 * [TODO buffer sizes including log]
 *
 * default:
 * {
 *   input: <required>
 *   from: { dwg: true }, // detect
 *   json: true,
 *   level: 'warn'
 * }
 */
const res = convert({input, from: { dwg: 'R_2000' }, json: true, dxfb: 'R_2004', level: 'warn'})

/* Result:
 * {
 *   error: 'INVALIDDWG',
 *   log: '...',
 * }
 * or:
 * {
 *   log: '...',
 *   json: {...}
 *   dxfb: <ArrayBuffer>
 * }
 * [TODO list of codes]
 */
console.log('result', res)
```

Later versions might add the actual API, native node.js, streams, promises and Typescript.
