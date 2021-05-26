const wasm = import('./message_board_wasm')

wasm
  .then(m => {
    m.main()
  })
  .catch(console.error)
