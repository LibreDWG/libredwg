import globals from 'globals'
import jsLint from '@eslint/js'
import tsLint from 'typescript-eslint'
import eslintConfigPrettier from 'eslint-config-prettier'
import pluginSimpleImportSort from 'eslint-plugin-simple-import-sort'

export default [
  {
    files: ["**/*.{js,mjs,cjs,ts,mts,jsx,tsx}"],
    languageOptions: {
      // common parser options, enable TypeScript and JSX
      parser: "@typescript-eslint/parser",
      parserOptions: {
        sourceType: "module"
      }
    }
  },
  { 
    languageOptions: { 
      globals: { 
        ...globals.browser,
      } 
    }
  },
  {
    plugins: {
      // key "simple-import-sort" is the plugin namespace
      "simple-import-sort": pluginSimpleImportSort
    },
    rules: {
      "simple-import-sort/imports": [
        "error"
      ]
    }
  },
  jsLint.configs.recommended,
  ...tsLint.configs.recommended,
  {
    ignores: ['node_modules', 'dist', 'public', 'src/libredwg-web.js']
  },
  eslintConfigPrettier,
  {
    rules: {
      "@typescript-eslint/no-empty-object-type": ["off"],
      "@typescript-eslint/no-unused-expressions": ["off"],
      '@typescript-eslint/no-unused-vars': [
        'error',
        {
          args: 'none',
          argsIgnorePattern: '^_',
          varsIgnorePattern: '^_',
          caughtErrorsIgnorePattern: '^_'
        }
      ],
      'quotes': ['error', 'single']
    }
  }
]