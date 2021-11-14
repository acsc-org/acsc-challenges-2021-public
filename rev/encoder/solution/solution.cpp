#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef DEBUG
#include <stdarg.h>
void debug_print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#else
void debug_print(const char *fmt, ...){}
#endif

template<typename T, int size>
inline __attribute__((always_inline)) constexpr T __rol(T n, T bit)
{
    auto c = bit % size;
    return (n << c) | (n >> (size - c));
}

uint8_t *decrypt(uint16_t *input, int len, int key)
{
    uint8_t *raw = (uint8_t *)malloc(len);

    for (int i = 0; i < len; ++i)
    {
        uint16_t cc = __rol<uint16_t, 16>(static_cast<uint16_t>(input[i]), ((i + 1) * 3) & 0xf);

        uint8_t p3 = ((~cc) >> 11) & 0x1f;
        uint8_t p1 = (cc >> 8) & 7;
        uint8_t p4 = ~(cc >> 5) & 7;
        uint8_t p2 = cc & 0x1f;

        raw[i] = static_cast<uint8_t>((p1 << 13) | (p2 << 8) | (p3 << 3) | p4);

        raw[i] ^= key;

    }
    
    return raw;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("[*] Usage : %s [file]\n", argv[0]);
        return -1;
    }

    FILE *input_fp = fopen(argv[1], "rb");

    // get file size
    fseek(input_fp, 0, SEEK_END);
    int sz = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    uint8_t *file_contents = (uint8_t *)malloc(sz);
    memset(file_contents, 0x00, sz);
    fread(file_contents, sz, 1, input_fp);
    fclose(input_fp);

    uint8_t *dec = nullptr;
    for(int i=0; i<=0xff; i++)
    {
        dec = decrypt((uint16_t *)file_contents, sz, i);

        // check jpeg header
        if(dec[0] == 0xff && dec[1] == 0xd8 && dec[2] == 0xff && dec[3] == 0xe0)
        {
            printf("key found ! %d", i);
            break;
        }
    }

    FILE *output_fp = fopen("flag.dec.jpg", "wb");
    fwrite(dec, sz / 2, 1, output_fp);
    fclose(output_fp);
    free(dec);
}