#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SEED 516765332
#define FORCEINLINE inline __attribute__((always_inline))

template<unsigned long seed>
class serial_encryption
{
    static const unsigned char initial_state1 = (unsigned char)(((seed + 1) * 176989) & 0xFF);
    static const unsigned char initial_state2 = (unsigned char)(((seed + 1) * 178903) & 0xFF);
    static const unsigned char initial_state3 = (unsigned char)(((seed + 1) * 180391) & 0xFF);
    static const unsigned char initial_state4 = (unsigned char)(((seed + 1) * 183823) & 0xFF);
    static const unsigned char shift1 = (char)(((seed + 1) * 164447) & 0xFF);
    static const unsigned char shift2 = (char)(((seed + 1) * 163637) & 0xFF);
    static const unsigned char xor1 = (char)(((seed + 1) * 166723) & 0xFF);
    static const unsigned char shift3 = (char)(((seed + 1) * 170179) & 0xFF);
    static const unsigned char shift4 = (char)(((seed + 1) * 173059) & 0xFF);

    template<unsigned char wordbits>
    FORCEINLINE static constexpr unsigned char rotl_bits(unsigned char v, unsigned char shift)
    {
        //int shift_moded = shift % sizeof(v) * 8;
        return
            ((v << (shift % wordbits)) | (v >> (wordbits - (shift % wordbits))))
                &((1 << wordbits) - 1);
    }
    template<unsigned char wordbits>
    FORCEINLINE static constexpr unsigned char rotr_bits(unsigned char v, unsigned char shift)
    {
        //int shift_moded = shift % sizeof(v) * 8;
        return
            ((v >> (shift % wordbits)) | (v << (wordbits - (shift % wordbits))))
                &((1 << wordbits) - 1);
    }
    template<typename integerT>
    FORCEINLINE static constexpr integerT rotl_pure(integerT v, int shift)
    {
        //int shift_moded = shift % sizeof(v) * 8;
        return
            (v << (shift % sizeof(v) * 8))
            | (v >> (sizeof(v) * 8 - (shift % sizeof(v) * 8)));
    }
    FORCEINLINE static constexpr unsigned char rotl8_pure(unsigned char v, unsigned char shift) { return rotl_pure<decltype(v)>(v, shift); }
    FORCEINLINE static constexpr unsigned int rotl32_pure(unsigned int v, unsigned char shift) { return rotl_pure<decltype(v)>(v, shift); }

public:
    static const unsigned long initial_state = initial_state1 | (initial_state2 << 8) | (initial_state3 << 16) | (initial_state4 << 24);
    FORCEINLINE static constexpr unsigned long next(unsigned long state)
    {
        return
            rotl_pure<unsigned long>(
            (((rotl_bits<5>(rotl32_pure(state, shift1), 2) << 3) ^ (rotl_bits<5>(rotl32_pure(state, shift2), 3) >> 3)) ^ xor1)
                + state,
                9
                );
    }

    FORCEINLINE static constexpr unsigned char encrypt(unsigned char e, unsigned char state)
    {
        e += state;
        return (rotl_bits<4>(e & 0xF, shift3) | (rotl_bits<4>((e >> 4) & 0xF, shift4) << 4));
    }
    FORCEINLINE static constexpr unsigned char decrypt(unsigned char e, unsigned char state)
    {
        return (rotr_bits<4>(e & 0xF, shift3) | (rotr_bits<4>((e >> 4) & 0xF, shift4) << 4)) - state;
    }

    FORCEINLINE static void encrypt_buffer(unsigned char* buffer, size_t size)
    {
        unsigned long s = initial_state;
        for (size_t i = 0; i < size; i++)
        {
            buffer[i] = encrypt(buffer[i], (unsigned char)s);
            s = next(s);
        }
    }
    FORCEINLINE static void decrypt_buffer(unsigned char* buffer, size_t size)
    {
        unsigned long s = initial_state;
        for (size_t i = 0; i < size; i++)
        {
            buffer[i] = decrypt(buffer[i], (unsigned char)s);
            s = next(s);
        }
    }
};

int main(int argc, char **argv)
{
    char *hexstring = argv[1];
    int i;
    int len = strlen(hexstring);
    unsigned char *bytearray = (unsigned char *)malloc(len / 2);

    for (i = 0; i < (len / 2); i++)
    {
        sscanf(hexstring + 2*i, "%2hhx", &bytearray[i]);
    }

    serial_encryption<SEED>::encrypt_buffer(bytearray, len/2);
    for (i = 0; i < (len / 2); i++)
    {
        printf("%02x", bytearray[i]);
    }
    // puts("");
    // serial_encryption<SEED>::decrypt_buffer(bytearray, len/2);
    // for (i = 0; i < (len / 2); i++)
    // {
    //     printf("%02x", bytearray[i]);
    // }
    return 0;
}