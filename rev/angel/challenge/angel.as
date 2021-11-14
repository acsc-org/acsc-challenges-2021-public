
void encrypt(array<uint8> key, array<uint8> @block)
{
    array<uint8> msg_lower_half(8);
    array<uint8> msg_upper_half(8);

    CopyArray(msg_lower_half, block, 8);
    CopyArray(msg_upper_half, block, 8, 0, 8);

    for (int round = 0; round < 16; round++) {
        uint8 transform_control_byte = key[(round*7)%16];
        for (int step = 0; step < 8; step++) {
            uint8 key_byte = key[((round*7)+step)%16];
            uint8 upper_msg_byte = msg_upper_half[step];
            bool icb  = ((transform_control_byte >> (7 - step)) & 0x01) == 0x01;
            uint8 leftbits = 0;
            uint8 rightbits = 0;

            for (int i = 4; i < 8; i++) {
                leftbits = (leftbits << 1) | (upper_msg_byte >> i & 0x01);
            }

            for (int i = 0; i < 4; i++) {
                rightbits = (rightbits << 1) | (upper_msg_byte >> i & 0x01);
            }

            uint8 res = 0;
            if (icb) {
                switch(leftbits) {
                    case 0:
                        res |= 2;
                        break;
                    case 1:
                        res |= 15;
                        break;
                    case 2:
                        res |= 12;
                        break;
                    case 3:
                        res |= 1;
                        break;
                    case 4:
                        res |= 5;
                        break;
                    case 5:
                        res |= 6;
                        break;
                    case 6:
                        res |= 10;
                        break;
                    case 7:
                        res |= 13;
                        break;
                    case 8:
                        res |= 14;
                        break;
                    case 9:
                        res |= 8;
                        break;
                    case 10:
                        res |= 3;
                        break;
                    case 11:
                        res |= 4;
                        break;
                    case 12:
                        res |= 0;
                        break;
                    case 13:
                        res |= 11;
                        break;
                    case 14:
                        res |= 9;
                        break;
                    case 15:
                        res |= 7;
                        break;
                }
                switch(rightbits) {
                    case 0:
                        res |= 15 << 4;
                        break;
                    case 1:
                        res |= 4 << 4;
                        break;
                    case 2:
                        res |= 5 << 4;
                        break;
                    case 3:
                        res |= 8 << 4;
                        break;
                    case 4:
                        res |= 9 << 4;
                        break;
                    case 5:
                        res |= 7 << 4;
                        break;
                    case 6:
                        res |= 2 << 4;
                        break;
                    case 7:
                        res |= 1 << 4;
                        break;
                    case 8:
                        res |= 10 << 4;
                        break;
                    case 9:
                        res |= 3 << 4;
                        break;
                    case 10:
                        res |= 0 << 4;
                        break;
                    case 11:
                        res |= 14 << 4;
                        break;
                    case 12:
                        res |= 6 << 4;
                        break;
                    case 13:
                        res |= 12 << 4;
                        break;
                    case 14:
                        res |= 13 << 4;
                        break;
                    case 15:
                        res |= 11 << 4;
                        break;
                }
            } else {
                switch(rightbits) {
                    case 0:
                        res |= 2;
                        break;
                    case 1:
                        res |= 15;
                        break;
                    case 2:
                        res |= 12;
                        break;
                    case 3:
                        res |= 1;
                        break;
                    case 4:
                        res |= 5;
                        break;
                    case 5:
                        res |= 6;
                        break;
                    case 6:
                        res |= 10;
                        break;
                    case 7:
                        res |= 13;
                        break;
                    case 8:
                        res |= 14;
                        break;
                    case 9:
                        res |= 8;
                        break;
                    case 10:
                        res |= 3;
                        break;
                    case 11:
                        res |= 4;
                        break;
                    case 12:
                        res |= 0;
                        break;
                    case 13:
                        res |= 11;
                        break;
                    case 14:
                        res |= 9;
                        break;
                    case 15:
                        res |= 7;
                        break;
                }
                switch(leftbits) {
                    case 0:
                        res |= 15 << 4;
                        break;
                    case 1:
                        res |= 4 << 4;
                        break;
                    case 2:
                        res |= 5 << 4;
                        break;
                    case 3:
                        res |= 8 << 4;
                        break;
                    case 4:
                        res |= 9 << 4;
                        break;
                    case 5:
                        res |= 7 << 4;
                        break;
                    case 6:
                        res |= 2 << 4;
                        break;
                    case 7:
                        res |= 1 << 4;
                        break;
                    case 8:
                        res |= 10 << 4;
                        break;
                    case 9:
                        res |= 3 << 4;
                        break;
                    case 10:
                        res |= 0 << 4;
                        break;
                    case 11:
                        res |= 14 << 4;
                        break;
                    case 12:
                        res |= 6 << 4;
                        break;
                    case 13:
                        res |= 12 << 4;
                        break;
                    case 14:
                        res |= 13 << 4;
                        break;
                    case 15:
                        res |= 11 << 4;
                        break;
                }
            }
            res = (res & 0xF0) >> 4 | (res & 0x0F) << 4;
            res = (res & 0xCC) >> 2 | (res & 0x33) << 2;
            res = (res & 0xAA) >> 1 | (res & 0x55) << 1;
            uint8 confused_byte = res;
            uint8 interrupted_byte = confused_byte ^ key_byte;
            uint8 permuted_byte = 0;
            permuted_byte |= (interrupted_byte & 0x1) << 1;
            permuted_byte |= (interrupted_byte & 0x2) >> 1;
            permuted_byte |= (interrupted_byte & 0x4) << 4;
            permuted_byte |= (interrupted_byte & 0x8) << 2;
            permuted_byte |= (interrupted_byte & 0x10) >> 1;
            permuted_byte |= (interrupted_byte & 0x20) << 2;
            permuted_byte |= (interrupted_byte & 0x40) >> 4;
            permuted_byte |= (interrupted_byte & 0x80) >> 3;
            if (((permuted_byte >> 7) & 0x01) == 0x01) {
                msg_lower_half[(7+step)%8] ^= 128;
            }
            if (((permuted_byte >> 6) & 0x01) == 0x01) {
                msg_lower_half[(6+step)%8] ^= 64;
            }
            if (((permuted_byte >> 5) & 0x01) == 0x01) {
                msg_lower_half[(2+step)%8] ^= 32;
            }
            if (((permuted_byte >> 4) & 0x01) == 0x01) {
                msg_lower_half[(1+step)%8] ^= 16;
            }
            if (((permuted_byte >> 3) & 0x01) == 0x01) {
                msg_lower_half[(5+step)%8] ^= 8;
            }
            if (((permuted_byte >> 2) & 0x01) == 0x01) {
                msg_lower_half[(0+step)%8] ^= 4;
            }
            if (((permuted_byte >> 1) & 0x01) == 0x01) {
                msg_lower_half[(3+step)%8] ^= 2;
            }
            if (((permuted_byte >> 0) & 0x01) == 0x01) {
                msg_lower_half[(4+step)%8] ^= 1;
            }
        }
        uint8 tmp;
        int length = msg_lower_half.length();
        for (int i = 0; i < length; i++) {
            tmp = msg_lower_half[i];
            msg_lower_half[i] = msg_upper_half[i];
            msg_upper_half[i] = tmp;
        }
    }

    uint8 tmp;
    int length = msg_lower_half.length();
    for (int i = 0; i < length; i++) {
        tmp = msg_lower_half[i];
        msg_lower_half[i] = msg_upper_half[i];
        msg_upper_half[i] = tmp;
    }

    CopyArray(block, msg_lower_half, 8);
    CopyArray(block, msg_upper_half, 8, 8, 0);

    return;
}

void decrypt(array<uint8> key, array<uint8> @block)
{
    array<uint8> msg_lower_half(8);
    array<uint8> msg_upper_half(8);

    CopyArray(msg_lower_half, block, 8);
    CopyArray(msg_upper_half, block, 8, 0, 8);

    for (int round = 0; round < 16; round++) {
        uint8 transform_control_byte = key[((round+1)*9)%16];

        for (int step = 0; step < 8; step++) {
            uint8 key_byte = key[(((round+1)*9)+step)%16];
            uint8 upper_msg_byte = msg_upper_half[step];
            bool icb = ((transform_control_byte >> (7 - step)) & 0x01) == 0x01;
            uint8 leftbits = 0;
            uint8 rightbits = 0;

            for (int i = 4; i < 8; i++) {
                leftbits = (leftbits << 1) | (upper_msg_byte >> i & 0x01);
            }

            for (int i = 0; i < 4; i++) {
                rightbits = (rightbits << 1) | (upper_msg_byte >> i & 0x01);
            }

            uint8 res = 0;
            if (icb) {
                switch(leftbits) {
                    case 0:
                        res |= 2;
                        break;
                    case 1:
                        res |= 15;
                        break;
                    case 2:
                        res |= 12;
                        break;
                    case 3:
                        res |= 1;
                        break;
                    case 4:
                        res |= 5;
                        break;
                    case 5:
                        res |= 6;
                        break;
                    case 6:
                        res |= 10;
                        break;
                    case 7:
                        res |= 13;
                        break;
                    case 8:
                        res |= 14;
                        break;
                    case 9:
                        res |= 8;
                        break;
                    case 10:
                        res |= 3;
                        break;
                    case 11:
                        res |= 4;
                        break;
                    case 12:
                        res |= 0;
                        break;
                    case 13:
                        res |= 11;
                        break;
                    case 14:
                        res |= 9;
                        break;
                    case 15:
                        res |= 7;
                        break;
                }
                switch(rightbits) {
                    case 0:
                        res |= 15 << 4;
                        break;
                    case 1:
                        res |= 4 << 4;
                        break;
                    case 2:
                        res |= 5 << 4;
                        break;
                    case 3:
                        res |= 8 << 4;
                        break;
                    case 4:
                        res |= 9 << 4;
                        break;
                    case 5:
                        res |= 7 << 4;
                        break;
                    case 6:
                        res |= 2 << 4;
                        break;
                    case 7:
                        res |= 1 << 4;
                        break;
                    case 8:
                        res |= 10 << 4;
                        break;
                    case 9:
                        res |= 3 << 4;
                        break;
                    case 10:
                        res |= 0 << 4;
                        break;
                    case 11:
                        res |= 14 << 4;
                        break;
                    case 12:
                        res |= 6 << 4;
                        break;
                    case 13:
                        res |= 12 << 4;
                        break;
                    case 14:
                        res |= 13 << 4;
                        break;
                    case 15:
                        res |= 11 << 4;
                        break;
                }
            } else {
                switch(rightbits) {
                    case 0:
                        res |= 2;
                        break;
                    case 1:
                        res |= 15;
                        break;
                    case 2:
                        res |= 12;
                        break;
                    case 3:
                        res |= 1;
                        break;
                    case 4:
                        res |= 5;
                        break;
                    case 5:
                        res |= 6;
                        break;
                    case 6:
                        res |= 10;
                        break;
                    case 7:
                        res |= 13;
                        break;
                    case 8:
                        res |= 14;
                        break;
                    case 9:
                        res |= 8;
                        break;
                    case 10:
                        res |= 3;
                        break;
                    case 11:
                        res |= 4;
                        break;
                    case 12:
                        res |= 0;
                        break;
                    case 13:
                        res |= 11;
                        break;
                    case 14:
                        res |= 9;
                        break;
                    case 15:
                        res |= 7;
                        break;
                }
                switch(leftbits) {
                    case 0:
                        res |= 15 << 4;
                        break;
                    case 1:
                        res |= 4 << 4;
                        break;
                    case 2:
                        res |= 5 << 4;
                        break;
                    case 3:
                        res |= 8 << 4;
                        break;
                    case 4:
                        res |= 9 << 4;
                        break;
                    case 5:
                        res |= 7 << 4;
                        break;
                    case 6:
                        res |= 2 << 4;
                        break;
                    case 7:
                        res |= 1 << 4;
                        break;
                    case 8:
                        res |= 10 << 4;
                        break;
                    case 9:
                        res |= 3 << 4;
                        break;
                    case 10:
                        res |= 0 << 4;
                        break;
                    case 11:
                        res |= 14 << 4;
                        break;
                    case 12:
                        res |= 6 << 4;
                        break;
                    case 13:
                        res |= 12 << 4;
                        break;
                    case 14:
                        res |= 13 << 4;
                        break;
                    case 15:
                        res |= 11 << 4;
                        break;
                }
            }
            res = (res & 0xF0) >> 4 | (res & 0x0F) << 4;
            res = (res & 0xCC) >> 2 | (res & 0x33) << 2;
            res = (res & 0xAA) >> 1 | (res & 0x55) << 1;
            uint8 confused_byte = res;
            uint8 interrupted_byte = confused_byte ^ key_byte;
            uint8 permuted_byte = 0;
            permuted_byte |= (interrupted_byte & 0x1) << 1;
            permuted_byte |= (interrupted_byte & 0x2) >> 1;
            permuted_byte |= (interrupted_byte & 0x4) << 4;
            permuted_byte |= (interrupted_byte & 0x8) << 2;
            permuted_byte |= (interrupted_byte & 0x10) >> 1;
            permuted_byte |= (interrupted_byte & 0x20) << 2;
            permuted_byte |= (interrupted_byte & 0x40) >> 4;
            permuted_byte |= (interrupted_byte & 0x80) >> 3;
            if (((permuted_byte >> 7) & 0x01) == 0x01) {
                msg_lower_half[(7+step)%8] ^= 128;
            }
            if (((permuted_byte >> 6) & 0x01) == 0x01) {
                msg_lower_half[(6+step)%8] ^= 64;
            }
            if (((permuted_byte >> 5) & 0x01) == 0x01) {
                msg_lower_half[(2+step)%8] ^= 32;
            }
            if (((permuted_byte >> 4) & 0x01) == 0x01) {
                msg_lower_half[(1+step)%8] ^= 16;
            }
            if (((permuted_byte >> 3) & 0x01) == 0x01) {
                msg_lower_half[(5+step)%8] ^= 8;
            }
            if (((permuted_byte >> 2) & 0x01) == 0x01) {
                msg_lower_half[(0+step)%8] ^= 4;
            }
            if (((permuted_byte >> 1) & 0x01) == 0x01) {
                msg_lower_half[(3+step)%8] ^= 2;
            }
            if (((permuted_byte >> 0) & 0x01) == 0x01) {
                msg_lower_half[(4+step)%8] ^= 1;
            }
        }

        uint8 tmp;
        int length = msg_lower_half.length();
        for (int i = 0; i < length; i++) {
            tmp = msg_lower_half[i];
            msg_lower_half[i] = msg_upper_half[i];
            msg_upper_half[i] = tmp;
        }
    }

    uint8 tmp;
    int length = msg_lower_half.length();
    for (int i = 0; i < length; i++) {
        tmp = msg_lower_half[i];
        msg_lower_half[i] = msg_upper_half[i];
        msg_upper_half[i] = tmp;
    }

    CopyArray(block, msg_lower_half, 8);
    CopyArray(block, msg_upper_half, 8, 8, 0);
}

void CopyArray(array<uint8> @dst, array<uint8> src, uint size, uint dstOffset=0, uint srcOffset=0)
{
    for (uint i = 0; i < size; i++) {
        dst[dstOffset+i] = src[srcOffset+i];
    }
}

void DumpArray(array<uint8> arr, uint size, bool isNewLine=false)
{
    for (uint i = 0; i < size; i++) {
        Print(arr[i]);
    }
    if (isNewLine) {
        Print("\n");
    }
}

void main()
{
    array<uint8> params = GetAngelParams();
    array<uint8> key(16);
    array<uint8> iv(16);
    array<uint8> msg(48);
    file keyFile;
    if (keyFile.open("/dev/urandom", "r") < 0) {
        return;
    }
    for (int i = 0; i < 16; i++){
        iv[i] = keyFile.readUInt(1);
    }

    uint64 m = 0xf80de74fcbc25ebb;
    uint64 h = 0x59bee0667c3df72c;

    uint64 k;
    k = 0;
    k |= uint64(iv[0]);
    k |= uint64(iv[1]) << 8;
    k |= uint64(iv[2]) << (8 * 2);
    k |= uint64(iv[3]) << (8 * 3);
    k |= uint64(iv[4]) << (8 * 4);
    k |= uint64(iv[5]) << (8 * 5);
    k |= uint64(iv[6]) << (8 * 6);
    k |= uint64(iv[7]) << (8 * 7);

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
    k |= uint64(iv[8]);
    k |= uint64(iv[9]) << 8;
    k |= uint64(iv[10]) << (8 * 2);
    k |= uint64(iv[11]) << (8 * 3);
    k |= uint64(iv[12]) << (8 * 4);
    k |= uint64(iv[13]) << (8 * 5);
    k |= uint64(iv[14]) << (8 * 6);
    k |= uint64(iv[15]) << (8 * 7);

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

    keyFile.close();
    CopyArray(msg, params, 48);

    DumpArray(iv, 16);
    array<uint8> block(16);
    for (int i = 0; i < 3; i++){
        CopyArray(block, msg, 16, 0, i*16);
        for (int ib = 0; ib < 16; ib++) {
            block[ib] ^= iv[ib];
        }

        encrypt(key, block);
        DumpArray(block, 16);

        for (int ib = 0; ib < 16; ib++) {
            iv[ib] = block[ib];
        }
    }

    Print("\n");
}