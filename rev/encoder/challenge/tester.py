from pwn import *
import re
from subprocess import check_output

e = ELF("./encoder")
main = e.symbols["main"]

marker = b'\x90\x90\x90'
main_bytes = bytearray(e.read(main, 0x1000))

target = [m.start() for m in re.finditer(marker, main_bytes)]

for i in range(0, len(target)):
    curr = target[i]
    try:
        next = target[i + 1]
    except:
        break
    bs = main_bytes[curr + len(marker):next].hex()
    encoded = check_output("./tester {}".format(bs), shell=True).decode("utf-8")
    encoded = bytes.fromhex(encoded)
    code_size = next - curr - len(marker)
    assert code_size < 256

    main_bytes[curr:curr + 2] = b'\x0f\x0b'
    main_bytes[curr + 2] = code_size

    for i in range(code_size):
        main_bytes[curr + len(marker) + i] = encoded[i]

with open('./encoder', 'rb') as f:
    data = f.read()
data = data[:main] + bytes(main_bytes) + data[main + 0x1000:]
with open('./encoder_enc', 'wb') as f:
    f.write(data)