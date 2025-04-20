# libredwg-web

This is a DWG/DXF JavaScript parser based on libredwg. It can be used in browser and Node.js environments. 

- [Live demo](https://mlight-lee.github.io/libredwg-web/)
- [API docs](https://mlight-lee.github.io/libredwg-web/docs/)

## Build WebAssembly

Download and install emscripten according to [this doc](https://emscripten.org/docs/getting_started/downloads.html). Please make sure the following command is executed to activate `PATH` and other environment variables in the current terminal before building web assembly.

```bash
# Activate PATH and other environment variables for emscripten in the current terminal
source ./emsdk_env.sh

./autogen.sh

# Install npm dependencies to build JavaScript bindings for libredwg
pnpm install

# Check for dependencies, available tools, and system configurations and prepare the software package for building libredwg on a specific system
pnpm build:prepare

# Compile and build libredwg
pnpm build:obj

# Use emscripten to build web assembly for libredwg
pnpm build:wasm

# Copy web assembly (wasm file and JavaScript glue code file) from build directory to distribution directory of this package
nnpm copy

# Build web assembly wrapper so that it is easier to use it
pnpm build
```

In order to reduce the size of wasm file, the following functionalities are not included by default when building web assembly.

- write dwg file
- read/write dxf file
- import/export json file

If you want those functionalities, just modify command `build:prepare` defined in [package.json](./package.json) and remove the following options.

- disable-write
- disable-json
- disable-dxf

## Usage

There are two approaches to use this package. No matter which approach to use, please do remember copying wasm file (libredwg.wasm) to the same folder as your JavaScript bundle file when deploying your application. 

```bash
npm install @mlightcad/libredwg-web
```

### Use Raw Web Assembly

The raw web assembly module (wasm file and JavaScript glue code file) is stored in folder [wasm](./wasm/). 

```javascript
import { createModule } from "@mlightcad/libredwg-web/wasm/libredwg-web.js";

// Create libredwg module
const libredwg = await createModule();

// Store file content to one temporary file and read it
const fileName = 'tmp.dwg';
libredwg.FS.writeFile(
  fileName,
  new Uint8Array(fileContent)
);
const result = libredwg.dwg_read_file(fileName);
if (result.error != 0) {
  console.log('Failed to open dwg file with error code: ', result.error);
}
libredwg.FS.unlink(fileName);

// Get pointer to Dwg_Data
const data = result.data;
```

### Use Web Assembly Wrapper

Web assembly wrapper is stored in folder [dist](./dist/). It provides one class `LibreDwg` to wrap the web assembly. This class provides

- Method to convert dwg data to [DwgDatabase](https://mlight-lee.github.io/libredwg-web/docs/interfaces/database_database.DwgDatabase.html) instance with the strong type definition so that it is easy to use.
- More methods that the raw web assembly API doesn't provide. 

```typescript
import { Dwg_File_Type, LibreDwg } from '@mlightcad/libredwg-web';
const libredwg = await LibreDwg.create();
const dwg = libredwg.dwg_read_data(fileContent, Dwg_File_Type.DWG);
const db = this.libredwg.convert(dwg);
```

## Interfaces

There are two kinds of interfaces defined to access dwg/dxf drawing data. 

### Interfaces with prifix 'Dwg'

Those interfaces are much more easier to use with better data structure. It is quite similar to interfaces defined in project [@mlightcad/dxf-json](https://github.com/mlight-lee/dxf-json). Those interfaces describe most of commonly used objects in the dwg/dxf drawing.

### Interfaces with prefix 'Dwg_'

Those interfaces are JavaScript version of `structs` defined in libredwg C++ code. Only a few `structs` have the correponding JavaScript interface. Most of them are defined to make it easier to convert libredwg data structure to [DwgDatabase](https://mlight-lee.github.io/libredwg-web/docs/interfaces/database_database.DwgDatabase.html).

So it is recommend to use interfaces with prefix 'Dwg'.

## Demo App

One demo app is provided in folder [test](./test/). You can run the following command to launch it.

```javascript
pnpm demo
```