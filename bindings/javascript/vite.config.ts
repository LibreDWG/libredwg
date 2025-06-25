import { defineConfig } from 'vite'

export default defineConfig({
  build: {
    assetsInlineLimit: 0,
    outDir: 'dist',
    minify: false,
    lib: {
      entry: 'src/index.ts',
      name: 'libredwg-web',
      fileName: 'libredwg-web'
    },
    rollupOptions: {
      external: ['module']
    }
  }
})
