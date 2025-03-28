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
      name: 'copy-wasm-to-test',
      configureServer(server) {
        server.middlewares.use((req, res, next) => {
          // Define the array of files to copy: [source, destination]
          const filesToCopy = [
            {
              source: path.resolve(__dirname, 'wasm/libredwg-web.wasm'),
              dest: path.resolve(__dirname, 'test/libredwg-web.wasm'),
            },
            {
              source: path.resolve(__dirname, 'dist/libredwg-web.mjs'),
              dest: path.resolve(__dirname, 'test/libredwg-web.mjs'),
            },
          ];

          // Iterate through each file and copy it if it doesn't exist at the destination
          filesToCopy.forEach(({ source, dest }) => {
            if (fs.existsSync(source) && !fs.existsSync(dest)) {
              fs.copyFileSync(source, dest);
              console.log(`✅ Copied ${path.basename(source)} to ${path.basename(dest)}`);
            }
          });
        });
      },
    },
    commonjs(),
  ],
})
