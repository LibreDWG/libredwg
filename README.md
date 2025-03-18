# libredwg-web

This is a webassembly version of libredwg. It can be used in browser and Node.js environments. 

You can play with it through this [live demo](https://mlight-lee.github.io/libredwg-web/).

## Build WebAssembly

Download and install emscripten according to [this doc](https://emscripten.org/docs/getting_started/downloads.html). Please make sure the following command is executed to activate `PATH` and other environment variables in the current terminal before building web assembly.

```
source ./emsdk_env.sh
```

### Use auotmake

```
./autogen.sh
mkdir build-wasm
cd build-wasm
emconfigure ../configure CFLAGS="-sUSE_ZLIB=1" CC=emcc --disable-bindings --disable-shared
emmake make
emcc ../src/bindings.cpp -O2 -s LINKABLE=1 -lembind -std=c++11 -Isrc -I../include src/*.o -o libredwg-web.js -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME="createModule" --emit-tsd libredwg-web.d.ts
```

emcc ../src/bindings.cpp -O2 -s LINKABLE=1 -lembind -std=c++11 -Isrc -I../include src/*.o -o libredwg-web.js -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME="createModule" -sEXPORTED_RUNTIME_METHODS=FS,ENV,ccall,cwrap,UTF8ToString,stringToNewUTF8,setValue --emit-tsd libredwg-web.d.ts