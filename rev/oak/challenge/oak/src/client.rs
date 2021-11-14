mod encoder;
mod ram;
use byteorder::{LittleEndian, WriteBytesExt};
use encoder::Encoder;
use ram::client::RamClient;
use ring::digest;
use std::{env, fs, io::prelude::*, net::TcpStream};

const ENC_KEY: &[u8] = &[
    250, 121, 105, 188, 82, 106, 64, 155, 83, 132, 141, 176, 48, 134, 101, 222, 128, 168, 16, 114,
    139, 166, 177, 187, 27, 91, 21, 244, 179, 137, 22, 153,
];

fn main() {
    if env::args().len() < 3 {
        println!("{} [server] [input_file]", env::args().nth(0).unwrap());
        return;
    }

    let server = env::args().nth(1).unwrap();
    let filename = env::args().nth(2).unwrap();

    let stream = TcpStream::connect(server).unwrap();

    let mut ram_client = RamClient::new(ENC_KEY, stream);
    let mut encoder = Encoder::new(&mut ram_client);

    let mut ctx = digest::Context::new(&digest::SHA512);
    let mut size = 0u64;
    let mut input = fs::File::open(filename).unwrap();
    let mut output = fs::OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open("/dev/null")
        .unwrap();
    let mut buf = [0u8; 1];
    while input.read_exact(&mut buf).is_ok() {
        ctx.update(&buf);
        if let Some(code) = encoder.encode(buf[0]) {
            output.write_u32::<LittleEndian>(code).unwrap();
        }
        eprintln!("Offset: {}", size);
        size += 1;
    }

    let mut tail = vec![];
    tail.write_u64::<LittleEndian>(size).unwrap();
    tail.extend_from_slice(ctx.finish().as_ref());
    for data in tail {
        if let Some(code) = encoder.encode(data) {
            output.write_u32::<LittleEndian>(code).unwrap();
        }
    }

    if let Some(code) = encoder.finish() {
        output.write_u32::<LittleEndian>(code).unwrap();
    }
}
