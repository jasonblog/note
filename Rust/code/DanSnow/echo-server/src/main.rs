extern crate futures;
extern crate tokio;

use futures::{Future, Stream};
use std::net::SocketAddr;
use tokio::{
    io::{self, AsyncRead},
    net::TcpListener,
    runtime::current_thread::Runtime,
};

fn main() {
    let mut runtime = Runtime::new().unwrap();
    let handle = runtime.handle();
    let listener = TcpListener::bind(&SocketAddr::new("127.0.0.1".parse().unwrap(), 1234)).unwrap();
    let fut = listener
        .incoming()
        .for_each(|stream| {
            let (read, write) = stream.split();
            handle
                .spawn(
                    io::copy(read, write)
                        .map(|_| ())
                        .map_err(|e| println!("{:?}", e)),
                )
                .unwrap();
            Ok(())
        })
        .map_err(|e| println!("{:?}", e));
    runtime.block_on(fut).unwrap();
}
