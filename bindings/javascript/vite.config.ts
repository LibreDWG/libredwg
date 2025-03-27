import commonjs from '@rollup/plugin-commonjs'
import { defineConfig } from 'vite'
import path from 'path'
import fs from 'fs'

let copied = false  // Global flag to ensure copying happens only once

export default defineConfig({
  build: {
    outDir: 'dist',
    lib: {
      entry: 'src/index.ts',
      name: 'libredwg-web',
      fileName: 'libredwg-web'
    }
  },
  server: {
    open: '/test/index.html', // Automatically open the test page
    watch: {
      // Watch for changes in the wasm folder (optional, if you want live updates during development)
      usePolling: true,
    },
  },
  plugins: [
    {
      name: 'copy-wasm-to-dist',
      writeBundle() {
        if (copied) return; // Prevent multiple executions
        copied = true;

        const src = path.resolve(__dirname, 'wasm/libredwg-web.wasm');
        const dest = path.resolve(__dirname, 'dist/libredwg-web.wasm');

        if (fs.existsSync(src)) {
          fs.copyFileSync(src, dest);
          console.log('✅ Copied wasm file to dist');
        } else {
          console.error('❌ WASM file not found:', src);
        }
      },
    },
    {
      name: 'copy-wasm-files',
      configureServer(server) {
        server.middlewares.use((req, res, next) => {
          // Define paths for the wasm file and the mjs file
          const wasmSource = path.resolve(__dirname, 'wasm/libredwg-web.wasm');
          const wasmDest = path.resolve(__dirname, 'test/libredwg-web.wasm');
          const mjsSource = path.resolve(__dirname, 'dist/libredwg-web.mjs');
          const mjsDest = path.resolve(__dirname, 'test/libredwg-web.mjs');

          // Check if the wasm file exists in the source folder, and copy to test folder if not present
          if (fs.existsSync(wasmSource) && !fs.existsSync(wasmDest)) {
            fs.copyFileSync(wasmSource, wasmDest);
            console.log('✅ Copied wasm file to test folder');
          }

          // Check if the mjs file exists in the dist folder, and copy it to wasm folder
          if (fs.existsSync(mjsSource) && !fs.existsSync(mjsDest)) {
            fs.copyFileSync(mjsSource, mjsDest);
            console.log('✅ Copied mjs file to wasm folder');
          }

          next();
        });
      },
    },
    commonjs(),
  ],
})
