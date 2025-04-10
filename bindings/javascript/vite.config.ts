import { defineConfig } from 'vite'

export default defineConfig({
  build: {
    assetsInlineLimit: 0,
    outDir: 'dist',
    lib: {
      entry: 'src/index.ts',
      name: 'libredwg-web',
      fileName: 'libredwg-web'
    }
  }
})
