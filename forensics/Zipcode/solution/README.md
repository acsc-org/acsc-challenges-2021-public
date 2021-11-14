# Solution

Recommend to use Simple DEFLATE decompressor to read Huffman code info:
https://github.com/nayuki/Simple-DEFLATE-decompressor

```py
import deflatedecompress
import struct
import io

with open('challenge.zip', 'rb') as f:
    for i in range(76):
        f.read(0x0C + 2 + 4)
        tmp = f.read(4)
        ln = struct.unpack("<I", tmp)[0]
        f.read(16)
        data = f.read(ln)
        deflatedecompress.Decompressor.decompress_to_bytes(
            deflatedecompress.BitInputStream(io.BytesIO(data))
        )
        print(deflatedecompress.wow)
```

Modify `_decode_huffman_codes` in `deflatedecompress.py`:
```py
        ...
        # Create literal-length code tree
		litlencode = CanonicalCode(codelens[ : numlitlencodes])

		global wow
        wow = list(litlencode._code_bits_to_symbol.values())
		
		# Create distance code tree with some extra processing
		distcodelen: List[int] = codelens[numlitlencodes : ]
        ...
```