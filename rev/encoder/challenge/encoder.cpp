#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

#define SEED 516765332
#define NOP() asm("nop");
#define PLACE_HOLDER() NOP() NOP() NOP()

#define FUCK_IDA() asm(".byte 0x73, 0x05"); asm(".byte 0xFF, 0xFE, 0xF0, 0xAB, 0x2A"); 

#define FORCEINLINE inline __attribute__((always_inline))

int key = 0;
char file_name[256]{0x00, };
int urand_fd = 0;
struct DecInfo
{
    uint8_t *rip;
    uint8_t code_size;
};

std::vector<DecInfo> DecVec;
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

void sigill_handler(int signo, siginfo_t *si, void *data)
{
    FUCK_IDA()
    ucontext_t *uc = (ucontext_t *)data;
    // parse structure 
    uint8_t *rip = (uint8_t *)uc->uc_mcontext.gregs[REG_RIP];
    auto backup = uc->uc_mcontext;
    int code_size = *(rip +2);
    int skip_len = 3;

    FUCK_IDA()
    
    // scramble decrypted code
    if(!DecVec.empty())
    {
        DecInfo di = DecVec.back();
        read(urand_fd, di.rip+skip_len, di.code_size);
        DecVec.pop_back();
    }
    // save decrypted code info
    DecInfo di;
    di.rip = rip;
    di.code_size = code_size;
    DecVec.emplace_back(di);

    // decrypt and skip bytes
    uint8_t *code = (uint8_t *)malloc(code_size);
    memset(code, 0x00, code_size);
    memcpy(code, rip+skip_len, code_size);
    serial_encryption<SEED>::decrypt_buffer(code, code_size);
    memset(rip, 0x90, skip_len);
    memcpy(rip+skip_len, code, code_size);
    uc->uc_mcontext = backup;
    uc->uc_mcontext.gregs[REG_RIP] += skip_len; // len("ud2" + size)
}

void __attribute__ ((destructor)) postmain()
{
    close(urand_fd);
}

void __attribute__ ((constructor)) premain()
{
    FUCK_IDA()
    urand_fd = open("/dev/urandom", O_RDONLY);
    srand(time(NULL));
    key = rand() % 255;
    struct sigaction sa, osa;
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sigill_handler;
    sigaction(SIGILL, &sa, &osa);
}

template<typename T, int size>
FORCEINLINE constexpr T __ror(T n, T bit)
{
    return (n >> bit) | (n << (size - bit));
}

FORCEINLINE void encrypt(uint8_t *input, uint16_t *output, int len, uint8_t key)
{
    PLACE_HOLDER()
    for(int i=0 ; i<len; i++)
    {
        uint8_t cc = input[i];
        cc ^= key;

        uint8_t p1 = cc >> 13;
        uint8_t p2 = (cc >> 8) & 0x1f;
        uint8_t p3 = ~(cc >> 3) & 0x1f;
        uint8_t p4 = ~cc & 7;

        uint16_t ec = (p3 << 11) | (p1 << 8) | (p4 << 5) | p2;
        uint16_t ec2 = __ror<uint16_t, 16>(ec, ((i + 1) * 3) & 0xf);
        output[i] = ec2;
    }
}

int main(int argc, char **argv)
{
    int ret = mprotect((void *)((unsigned long)&main & 0xfffffffffffff000), 0x4000, PROT_READ | PROT_WRITE | PROT_EXEC);

PLACE_HOLDER() // placeholder, should be replace as tester.py result
    if(argc < 2)
    {
PLACE_HOLDER()
        printf("[*] Usage : %s [file]\n", argv[0]);
        return 0;
    }
PLACE_HOLDER()
    FILE *input_fp = fopen(argv[1], "rb");

PLACE_HOLDER()
    if(!input_fp)
    {
PLACE_HOLDER()
        printf("open error\n");
        return 0;
    }
PLACE_HOLDER()
    fseek(input_fp, 0, SEEK_END);

PLACE_HOLDER()
    int sz = ftell(input_fp);

PLACE_HOLDER()
    fseek(input_fp, 0, SEEK_SET);

PLACE_HOLDER()
    uint8_t *file_contents = (uint8_t *)malloc(sz);

PLACE_HOLDER()
    memset(file_contents, 0x00, sz);

PLACE_HOLDER()
    fread(file_contents, sz, 1, input_fp);

PLACE_HOLDER()
    fclose(input_fp);
PLACE_HOLDER()
    uint16_t *enc = (uint16_t *)malloc(sz * sizeof(uint16_t));
PLACE_HOLDER()
    encrypt(file_contents, enc, sz, key);

PLACE_HOLDER()
    strcat(file_name, argv[1]);

PLACE_HOLDER()
    strcat(file_name, ".enc");
    
PLACE_HOLDER()
    FILE *output_fp = fopen(file_name, "wb");

PLACE_HOLDER()
    fwrite(enc, sz * sizeof(uint16_t), 1, output_fp);

PLACE_HOLDER()
    fclose(output_fp);

PLACE_HOLDER()
    free(enc);
    return 0;
}
