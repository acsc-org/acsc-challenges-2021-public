#!/bin/sh

cargo build --features=client --release --bin client
cargo build --features=server --release --bin server

strip --strip-debug target/release/client
strip --strip-debug target/release/server
