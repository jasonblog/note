const s = require('shelljs')

s.cd(__dirname)
s.exec('cargo build --release')
s.exec(
  'wasm-bindgen target/wasm32-unknown-unknown/release/wasm_demo.wasm --out-dir .'
)
