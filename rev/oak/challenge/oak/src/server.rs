mod ram;
use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use ram::server::RamServer;
use std::{
    fs,
    io::{Read, Write},
    net::TcpListener,
};

fn main() {
    let listener = TcpListener::bind("127.0.0.1:7878").unwrap();
    let stream = listener.incoming().next().unwrap();
    let mut stream = stream.unwrap();
    println!("Connection established!");

    let mut ram_server = RamServer::new();
    let mut log = fs::File::create("access.log").unwrap();

    loop {
        let op = match stream.read_u8() {
            Ok(op) => op,
            Err(_) => break,
        };

        log.write_u8(op).unwrap();
        if op == 0 {
            let idx = stream.read_u32::<LittleEndian>().unwrap() as usize;
            let buf = ram_server.read_bucket(idx);

            log.write_u32::<LittleEndian>(idx as u32).unwrap();

            stream.write_u16::<LittleEndian>(buf.len() as u16).unwrap();
            stream.write_all(&buf).unwrap();
            stream.flush().unwrap();
        } else {
            let idx = stream.read_u32::<LittleEndian>().unwrap() as usize;
            let len = stream.read_u16::<LittleEndian>().unwrap() as usize;
            let mut buf = vec![0u8; len];
            stream.read_exact(&mut buf).unwrap();
            ram_server.write_bucket(idx, &buf);

            log.write_u32::<LittleEndian>(idx as u32).unwrap();
            log.write_u16::<LittleEndian>(buf.len() as u16).unwrap();
            log.write_all(&buf).unwrap();
        }
        log.flush().unwrap();
    }
}
