from Crypto.Cipher import ChaCha20_Poly1305
import struct
import sys
import collections

key = bytes([250, 121, 105, 188, 82, 106, 64, 155, 83, 132, 141, 176, 48, 134, 101,
            222, 128, 168, 16, 114, 139, 166, 177, 187, 27, 91, 21, 244, 179, 137, 22, 153])

BLOCK_SIZE = 8
X = 65536

record = open(sys.argv[1], 'rb').read()
off = 0
dump = collections.defaultdict(lambda: [0] * BLOCK_SIZE)
counter = 0
while off < len(record):
    if record[off] == 0:
        off += 1 + 4
        continue

    off += 1
    idx = struct.unpack('I', record[off:off + 4])[0]
    off += 4
    size = struct.unpack('H', record[off:off + 2])[0]
    off += 2
    blob = record[off:off + size]
    off += size
    cipher = ChaCha20_Poly1305.new(
        key=key, nonce=struct.pack('QI', counter, idx))
    ptxt = cipher.decrypt_and_verify(blob[:-16], blob[-16:])
    chunk_size = (1 + BLOCK_SIZE) * 4
    for pos in range(0, 2):
        part = struct.unpack('I' + 'I' * BLOCK_SIZE,
                             ptxt[pos * chunk_size:(pos + 1) * chunk_size])
        addr = part[0]
        dump[addr] = part[1:]

    if idx == 1:
        counter += 1

codes = []


def load_node(prefix, sym, addr):
    code = dump[addr][0]
    if code > 255 and sym is not None:
        codes.append((code, sym, prefix))

    dic = []
    for i in range(256):
        dic.append(dump[addr + 1 + (i // BLOCK_SIZE)][i % BLOCK_SIZE])

    for (a, b) in enumerate(dic):
        if b != 0:
            load_node(prefix + [sym], a, b)

    return (code, dic)


load_node([], None, 1)
codes.sort(key=lambda x: x[0])
out = []
for code in codes:
    out.extend(code[2][1:])

open('out.jpg', 'wb').write(bytes(out))
