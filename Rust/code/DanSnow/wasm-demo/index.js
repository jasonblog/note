const wasm = import('./wasm_demo')

wasm
  .then(m => {
    m.main()
  })
  .catch(console.error)
