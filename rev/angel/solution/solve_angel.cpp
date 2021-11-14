#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>

void CopyArray(uint8_t* dst, uint8_t* src, int size, int dstOffset=0, int srcOffset=0)
{
    for (int i = 0; i < size; i++) {
        dst[dstOffset+i] = src[srcOffset+i];
    }
}

uint8_t S_BOX_ZERO[] = {2, 15, 12, 1, 5, 6, 10, 13, 14, 8, 3, 4, 0, 11, 9, 7};
uint8_t S_BOX_ONE[]  = {15, 4, 5, 8, 9, 7, 2, 1, 10, 3, 0, 14, 6, 12, 13, 11};

uint8_t permuteByte(uint8_t n) {
    uint8_t res = 0;
    res |= (n & 0x1) << 1;
    res |= (n & 0x2) >> 1;
    res |= (n & 0x4) << 4;
    res |= (n & 0x8) << 2;
    res |= (n & 0x10) >> 1;
    res |= (n & 0x20) << 2;
    res |= (n & 0x40) >> 4;
    res |= (n & 0x80) >> 3;
    return res;
}

uint8_t reverseByte(uint8_t n) {
    n = (n & 0xF0) >> 4 | (n & 0x0F) << 4;
    n = (n & 0xCC) >> 2 | (n & 0x33) << 2;
    n = (n & 0xAA) >> 1 | (n & 0x55) << 1;
    return n;
}

uint8_t applySBoxes(uint8_t msg, bool icb) {
    uint8_t leftbits = 0;
    uint8_t rightbits = 0;

    for (int i = 4; i < 8; i++) {
        leftbits = (leftbits << 1) | (msg >> i & 0x01);
    }

    for (int i = 0; i < 4; i++) {
        rightbits = (rightbits << 1) | (msg >> i & 0x01);
    }

    uint8_t res = 0;
    if (icb) {
        return reverseByte((S_BOX_ONE[rightbits] << 4) | S_BOX_ZERO[leftbits]);
    } else {
        return reverseByte((S_BOX_ONE[leftbits] << 4) | S_BOX_ZERO[rightbits]);
    }
}

uint8_t applyPermutation(uint8_t interrupted) {
	// return BYTE_PERMUTE[interrupted];
	return permuteByte(interrupted);
}

void applyDiffusion(uint8_t permuted, uint8_t offset, uint8_t* msg_half) {
	if (((permuted >> 7) & 0x01) == 0x01) {
		msg_half[(7+offset)%8] ^= 128;
	}
	if (((permuted >> 6) & 0x01) == 0x01) {
		msg_half[(6+offset)%8] ^= 64;
	}
	if (((permuted >> 5) & 0x01) == 0x01) {
		msg_half[(2+offset)%8] ^= 32;
	}
	if (((permuted >> 4) & 0x01) == 0x01) {
		msg_half[(1+offset)%8] ^= 16;
	}
	if (((permuted >> 3) & 0x01) == 0x01) {
		msg_half[(5+offset)%8] ^= 8;
	}
	if (((permuted >> 2) & 0x01) == 0x01) {
		msg_half[(0+offset)%8] ^= 4;
	}
	if (((permuted >> 1) & 0x01) == 0x01) {
		msg_half[(3+offset)%8] ^= 2;
	}
	if (((permuted >> 0) & 0x01) == 0x01) {
		msg_half[(4+offset)%8] ^= 1;
	}
}

void stepfn(uint8_t key_byte, uint8_t upper_msg_byte, bool icb, uint8_t step_num, uint8_t* lower_msg_half) {
    uint8_t confused_byte = applySBoxes(upper_msg_byte, icb);
    uint8_t interrupted_byte = confused_byte ^ key_byte;
    uint8_t permuted_byte = applyPermutation(interrupted_byte);
    applyDiffusion(permuted_byte, step_num, lower_msg_half);
}

void swapArray(uint8_t* arr1, uint8_t* arr2, int size) {
    uint8_t tmp;
    for (int i = 0; i < size; i++) {
        tmp = arr1[i];
        arr1[i] = arr2[i];
        arr2[i] = tmp;
    }
}

void encrypt(uint8_t* key, uint8_t* block)
{
    uint8_t msg_lower_half[8];
    uint8_t msg_upper_half[8];

    CopyArray(msg_lower_half, block, 8);
    CopyArray(msg_upper_half, block, 8, 0, 8);

    for (int round = 0; round < 16; round++) {
        uint8_t transform_control_byte = key[(round*7)%16];
        for (int step = 0; step < 8; step++) {
            uint8_t key_byte = key[((round*7)+step)%16];
            uint8_t upper_msg_byte = msg_upper_half[step];
            bool icb  = ((transform_control_byte >> (7 - step)) & 0x01) == 0x01;
            stepfn(key_byte, upper_msg_byte, icb, step, msg_lower_half);
        }
        swapArray(msg_lower_half, msg_upper_half, 8);
    }

    swapArray(msg_lower_half, msg_upper_half, 8);

    CopyArray(block, msg_lower_half, 8);
    CopyArray(block, msg_upper_half, 8, 8, 0);

    return;
}

void decrypt(uint8_t* key, uint8_t* block)
{
    uint8_t msg_lower_half[8];
    uint8_t msg_upper_half[8];

    CopyArray(msg_lower_half, block, 8);
    CopyArray(msg_upper_half, block, 8, 0, 8);

    for (int round = 0; round < 16; round++) {
		uint8_t transform_control_byte = key[((round+1)*9)%16];

		for (int step = 0; step < 8; step++) {
			uint8_t key_byte = key[(((round+1)*9)+step)%16];
			uint8_t upper_msg_byte = msg_upper_half[step];
			bool icb = ((transform_control_byte >> (7 - step)) & 0x01) == 0x01;
			stepfn(key_byte, upper_msg_byte, icb, step, msg_lower_half);
		}

		swapArray(msg_lower_half, msg_upper_half, 8);
	}

	swapArray(msg_lower_half, msg_upper_half, 8);

    CopyArray(block, msg_lower_half, 8);
    CopyArray(block, msg_upper_half, 8, 8, 0);
}

void DumpArray(uint8_t* arr, int size, bool isNewLine=false)
{
    for (int i = 0; i < size; i++) {
        printf("%02x", arr[i]);
    }
    if (isNewLine) {
        printf("\n");
    }
}

std::vector<char> ConvertHexToBytes(const std::string& hex) {
  std::vector<char> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s hexdata\n", argv[0]);
        return -1;
    }

    std::string hexdata(argv[1]);
    std::vector<char> bytes = ConvertHexToBytes(hexdata);

    uint8_t key[16];
    uint8_t iv[16];
    uint8_t encMsg[48];
    // uint8_t decMsg[48];
    memcpy(iv, bytes.data(), 16);
    memcpy(encMsg, bytes.data()+16, 48);

    uint64_t m = 0xf80de74fcbc25ebb;
    uint64_t h = 0x59bee0667c3df72c;

    uint64_t k;
    k = 0;
    k |= (uint64_t)iv[0];
    k |= (uint64_t)iv[1] << 8;
    k |= (uint64_t)iv[2] << (8 * 2);
    k |= (uint64_t)iv[3] << (8 * 3);
    k |= (uint64_t)iv[4] << (8 * 4);
    k |= (uint64_t)iv[5] << (8 * 5);
    k |= (uint64_t)iv[6] << (8 * 6);
    k |= (uint64_t)iv[7] << (8 * 7);

    k *= m; 
    k ^= k >> 43;
    k *= m;
    h ^= k;
    h *= m;

    h ^= h >> 42;
    h *= m;
    h ^= h >> 45;

    for (int i = 0; i < 8; i++){
        key[i] = (h >> (i * 8)) & 0xff;
    }

    k = 0;
    k |= (uint64_t)iv[8];
    k |= (uint64_t)iv[9] << 8;
    k |= (uint64_t)iv[10] << (8 * 2);
    k |= (uint64_t)iv[11] << (8 * 3);
    k |= (uint64_t)iv[12] << (8 * 4);
    k |= (uint64_t)iv[13] << (8 * 5);
    k |= (uint64_t)iv[14] << (8 * 6);
    k |= (uint64_t)iv[15] << (8 * 7);

    k *= m; 
    k ^= k >> 43;
    k *= m;
    h ^= k;
    h *= m;

    h ^= h >> 42;
    h *= m;
    h ^= h >> 45;

    for (int i = 0; i < 8; i++){
        key[8+i] = (h >> (i * 8)) & 0xff;
    }

    printf("key: ");
    DumpArray(key, 16, true);
    printf("iv: ");
    DumpArray(iv, 16, true);

    uint8_t block[16];
    for (int i = 0; i < 3; i++){
        memcpy(block, &encMsg[i*16], 16);

        decrypt(key, block);

        for (int ib = 0; ib < 16; ib++) {
            block[ib] ^= iv[ib];
        }
        DumpArray(block, 16);
        memcpy(iv, &encMsg[i*16], 16);
        // memcpy(&decMsg[i*16], block, 16);
    }
    printf("\n");
}