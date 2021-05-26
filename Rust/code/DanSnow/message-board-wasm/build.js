const s = require('shelljs')

s.cd(__dirname)
s.exec('cargo build --release')
s.exec(
  'wasm-bindgen target/wasm32-unknown-unknown/release/message_board_wasm.wasm --out-dir .'
)
