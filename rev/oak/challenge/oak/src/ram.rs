use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use rand::prelude::*;
use ring::aead;
use std::{
    collections::VecDeque,
    io::{self, Read, Write},
    net::TcpStream,
};

pub const NULL: u32 = 0;
pub const BLOCK_SIZE: usize = 8;

const LEVEL: usize = 17;
const BUCKET_SIZE: usize = 2;
const N: u32 = 1 << (LEVEL + 5);
const X: usize = 1 << (LEVEL - 1);

#[cfg(feature = "client")]
pub mod client {
    use super::*;

    #[derive(Clone, Copy)]
    pub struct Block {
        addr: u32,
        data: [u32; BLOCK_SIZE],
    }

    impl Block {
        fn dummy() -> Self {
            Block {
                addr: NULL,
                data: [0u32; BLOCK_SIZE],
            }
        }
    }

    #[derive(Clone)]
    pub struct Bucket {
        blocks: Vec<Block>,
    }

    impl Bucket {
        fn build(blocks: Vec<Block>) -> Self {
            assert_eq!(blocks.len(), BUCKET_SIZE);
            Self { blocks }
        }

        #[inline(never)]
        fn enc(&self, sealing_key: &aead::LessSafeKey, nonce: u64, idx: u32) -> Vec<u8> {
            let mut seq = vec![];
            for block in &self.blocks {
                seq.write_u32::<LittleEndian>(block.addr).unwrap();
                for unit in &block.data {
                    seq.write_u32::<LittleEndian>(*unit).unwrap();
                }
            }

            let mut nonce_bytes = Vec::new();
            nonce_bytes.write_u64::<LittleEndian>(nonce);
            nonce_bytes.write_u32::<LittleEndian>(idx as u32);
            sealing_key
                .seal_in_place_append_tag(
                    aead::Nonce::try_assume_unique_for_key(&nonce_bytes).unwrap(),
                    aead::Aad::empty(),
                    &mut seq,
                )
                .unwrap();
            seq
        }

        #[inline(never)]
        fn dec(ctxt: &[u8], opening_key: &aead::LessSafeKey, nonce: u64, idx: u32) -> Self {
            let mut blocks = Vec::new();
            if ctxt.is_empty() {
                for _ in 0..BUCKET_SIZE {
                    blocks.push(Block::dummy());
                }
            } else {
                let mut ptxt = ctxt.to_owned();
                let mut nonce_bytes = Vec::new();
                nonce_bytes.write_u64::<LittleEndian>(nonce);
                nonce_bytes.write_u32::<LittleEndian>(idx as u32);
                opening_key
                    .open_in_place(
                        aead::Nonce::try_assume_unique_for_key(&nonce_bytes).unwrap(),
                        aead::Aad::empty(),
                        &mut ptxt,
                    )
                    .unwrap();

                let mut cur = io::Cursor::new(ptxt);
                for _ in 0..BUCKET_SIZE {
                    let addr = cur.read_u32::<LittleEndian>().unwrap();
                    let mut data = [0u32; BLOCK_SIZE];
                    for idx in 0..BLOCK_SIZE {
                        data[idx] = cur.read_u32::<LittleEndian>().unwrap();
                    }
                    blocks.push(Block { addr, data: data });
                }
            }
            Self::build(blocks)
        }
    }

    pub struct RamClient {
        stash: VecDeque<Box<Block>>,
        pos_map: Vec<usize>,
        enc_key: aead::LessSafeKey,
        stream: TcpStream,
        dirmap: Vec<u8>,
        nonces: Vec<u64>,
        counter: u64,
    }

    impl RamClient {
        pub fn new(key: &[u8], stream: TcpStream) -> Self {
            let mut pos_map = Vec::new();
            let mut rng = rand::thread_rng();
            for _ in 0..N {
                pos_map.push(rng.gen::<usize>() % X);
            }
            let enc_key = aead::LessSafeKey::new(
                aead::UnboundKey::new(&aead::CHACHA20_POLY1305, key).unwrap(),
            );
            Self {
                stash: VecDeque::new(),
                pos_map,
                enc_key,
                stream,
                dirmap: vec![0u8; X],
                nonces: vec![0u64; X],
                counter: 0,
            }
        }

        pub fn read(&mut self, addr: u32) -> [u32; BLOCK_SIZE] {
            self.process(addr, None).data
        }

        pub fn write(&mut self, addr: u32, data: &[u32]) {
            assert!(data.len() <= BLOCK_SIZE);
            let mut block = [0u32; BLOCK_SIZE];
            block[..data.len()].copy_from_slice(data);
            self.process(addr, Some(block));
        }

        #[inline(never)]
        fn process(&mut self, addr: u32, updated_data: Option<[u32; BLOCK_SIZE]>) -> Block {
            assert!(addr < N);
            let mut rng = rand::thread_rng();
            let old_pos = self.pos_map[addr as usize];
            self.pos_map[addr as usize] = rng.gen::<usize>() % X;

            for level in 0..LEVEL {
                let bucket = self.read_bucket(get_idx(old_pos, level));
                for block in &bucket.blocks {
                    if block.addr != NULL {
                        self.stash.push_back(Box::new(*block));
                    }
                }
            }

            let mut ret_block = None;
            for block in &mut self.stash {
                if block.addr == addr {
                    if let Some(updated_data) = updated_data {
                        block.data = updated_data;
                    }
                    ret_block = Some(*block.as_ref());
                    break;
                }
            }
            let ret_block = if let Some(block) = ret_block {
                block
            } else {
                let new_block = Block {
                    addr,
                    data: updated_data.unwrap_or([0u32; BLOCK_SIZE]),
                };
                self.stash.push_back(Box::new(new_block));
                new_block
            };

            for level in (0..LEVEL).rev() {
                let old_stash = std::mem::replace(&mut self.stash, VecDeque::new());
                let old_idx = get_idx(old_pos, level);
                let mut bucket_blocks = Vec::new();
                for block in old_stash {
                    let idx = get_idx(self.pos_map[block.addr as usize], level);
                    if idx == old_idx && bucket_blocks.len() < BUCKET_SIZE {
                        bucket_blocks.push(*block);
                    } else {
                        self.stash.push_back(block);
                    }
                }
                while bucket_blocks.len() < BUCKET_SIZE {
                    bucket_blocks.push(Block::dummy());
                }
                self.write_bucket(old_idx, &Bucket::build(bucket_blocks), self.counter);

                if level < (LEVEL - 1) {
                    self.dirmap[old_idx as usize] = (get_idx(old_pos, level + 1) as u8) & 0x1;
                }
            }
            self.nonces[old_pos] = self.counter;
            self.counter += 1;

            ret_block
        }

        #[inline(never)]
        fn read_bucket(&mut self, idx: u32) -> Bucket {
            self.stream.write_u8(0).unwrap();
            self.stream.write_u32::<LittleEndian>(idx).unwrap();
            self.stream.flush().unwrap();

            let mut cur = idx as usize;
            while cur < X {
                cur = cur * 2 + self.dirmap[cur] as usize;
            }
            let nonce = self.nonces[cur - X];

            let len = self.stream.read_u16::<LittleEndian>().unwrap() as usize;
            let mut buf = vec![0u8; len];
            self.stream.read_exact(&mut buf).unwrap();
            Bucket::dec(&buf, &self.enc_key, nonce, idx)
        }

        #[inline(never)]
        fn write_bucket(&mut self, idx: u32, bucket: &Bucket, nonce: u64) {
            self.stream.write_u8(1).unwrap();
            self.stream.write_u32::<LittleEndian>(idx).unwrap();
            let payload = bucket.enc(&self.enc_key, nonce, idx);
            self.stream
                .write_u16::<LittleEndian>(payload.len() as u16)
                .unwrap();
            self.stream.write_all(&payload).unwrap();
            self.stream.flush().unwrap();
        }
    }

    #[inline(never)]
    fn get_idx(pos: usize, level: usize) -> u32 {
        (1 << level) + (pos >> (LEVEL - 1 - level)) as u32
    }
}

#[cfg(feature = "server")]
pub mod server {
    use super::*;

    pub struct RamServer {
        buckets: Vec<Vec<u8>>,
    }

    impl RamServer {
        pub fn new() -> Self {
            Self {
                buckets: vec![Vec::new(); 1 << LEVEL],
            }
        }

        #[inline(never)]
        pub fn read_bucket(&self, idx: usize) -> Vec<u8> {
            self.buckets[idx].clone()
        }

        #[inline(never)]
        pub fn write_bucket(&mut self, idx: usize, bucket: &[u8]) {
            self.buckets[idx] = bucket.to_owned();
        }
    }
}
