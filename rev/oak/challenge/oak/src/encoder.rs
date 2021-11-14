use super::ram::{client::RamClient, BLOCK_SIZE, NULL};

const CODE_OFF: u32 = 0;
const DICT_OFF: u32 = 1;

#[derive(Clone, Copy)]
struct Node {
    addr: u32,
    code: u32,
    dict: [u32; 256],
}

pub struct Encoder<'a> {
    ram: &'a mut RamClient,
    next_alloc: u32,
    next_code: u32,
    prev_node_addr: u32,
    root_node: Option<Node>,
}

impl<'a> Encoder<'a> {
    pub fn new(ram: &'a mut RamClient) -> Self {
        let mut encoder = Self {
            ram,
            next_alloc: 1,
            next_code: 0,
            prev_node_addr: NULL,
            root_node: None,
        };
        let mut root_node = encoder.alloc_node(0xFFFFFFFF);
        for symbol in 0..256 {
            root_node.dict[symbol] = encoder.create_node().addr;
        }
        encoder.write_node(&root_node);
        encoder.root_node = Some(root_node);
        encoder
    }

    pub fn encode(&mut self, data: u8) -> Option<u32> {
        let mut curr_node = if self.prev_node_addr == NULL {
            self.root_node.unwrap()
        } else {
            self.load_node(self.prev_node_addr)
        };
        let data = data as usize;
        let next_node_addr = curr_node.dict[data];
        if next_node_addr == NULL {
            curr_node.dict[data] = self.create_node().addr;
            let idx = data / BLOCK_SIZE;
            self.ram.write(
                curr_node.addr + DICT_OFF + idx as u32,
                &curr_node.dict[idx * BLOCK_SIZE..(idx + 1) * BLOCK_SIZE],
            );
            self.prev_node_addr = self.root_node.unwrap().dict[data];
            Some(curr_node.code)
        } else {
            self.prev_node_addr = next_node_addr;
            None
        }
    }

    pub fn finish(&mut self) -> Option<u32> {
        if self.prev_node_addr == NULL {
            None
        } else {
            Some(self.load_node(self.prev_node_addr).code)
        }
    }

    fn create_node(&mut self) -> Node {
        let node = self.alloc_node(self.next_code);
        self.next_code += 1;
        node
    }

    fn alloc_node(&mut self, code: u32) -> Node {
        let new_node = Node {
            addr: self.next_alloc,
            code,
            dict: [NULL; 256],
        };
        self.ram.write(new_node.addr + CODE_OFF, &[new_node.code]);
        self.next_alloc += DICT_OFF + (256 / BLOCK_SIZE as u32);
        new_node
    }

    fn load_node(&mut self, addr: u32) -> Node {
        let code = self.ram.read(addr + CODE_OFF)[0];
        let mut dict = [NULL; 256];
        for idx in 0..(256 / BLOCK_SIZE) {
            let block = self.ram.read(addr + DICT_OFF + idx as u32);
            dict[idx * BLOCK_SIZE..(idx + 1) * BLOCK_SIZE].copy_from_slice(&block);
        }
        Node { addr, code, dict }
    }

    fn write_node(&mut self, node: &Node) {
        self.ram.write(node.addr + CODE_OFF, &[node.code]);
        for idx in 0..(256 / BLOCK_SIZE) {
            self.ram.write(
                node.addr + DICT_OFF + idx as u32,
                &node.dict[idx * BLOCK_SIZE..(idx + 1) * BLOCK_SIZE],
            );
        }
    }
}
