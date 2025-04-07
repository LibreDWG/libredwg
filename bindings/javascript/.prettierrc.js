export default {
  semi: false,
  tabWidth: 2,
  singleQuote: true,
  printWidth: 80,
  endOfLine: 'auto',
  trailingComma: 'none',
  arrowParens: 'avoid',
  overrides: [
    {
      files: '*.ts',
      options: {
        parser: 'typescript'
      }
    },
    {
      files: '*.js',
      options: {
        parser: 'babel'
      }
    },
    {
      files: '*.md',
      options: {
        parser: 'markdown'
      }
    },
    {
      files: '*.json',
      options: {
        parser: 'json-stringify'
      }
    }
  ]
}