// void CopyArray(array<uint8> @dst, array<uint8> src, uint size, uint dstOffset=0, uint srcOffset=0);

// array<uint8> S_BOX_ZERO = {12, 15, 7, 10, 14, 13, 11, 0, 2, 6, 3, 1, 9, 4, 5, 8};
// array<uint8> S_BOX_ONE  = {7, 2, 14, 9, 3, 11, 0, 4, 12, 13, 1, 10, 6, 15, 8, 5};

// array<uint8> S_BOX_ZERO = {2, 15, 12, 1, 5, 6, 10, 13, 14, 8, 3, 4, 0, 11, 9, 7};
// array<uint8> S_BOX_ONE  = {15, 4, 5, 8, 9, 7, 2, 1, 10, 3, 0, 14, 6, 12, 13, 11};

uint8 permuteByte(uint8 n) {
    uint8 res = 0;
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

uint8 reverseByte(uint8 n) {
    n = (n & 0xF0) >> 4 | (n & 0x0F) << 4;
    n = (n & 0xCC) >> 2 | (n & 0x33) << 2;
    n = (n & 0xAA) >> 1 | (n & 0x55) << 1;
    return n;
}

uint8 applySBoxes(uint8 msg, bool icb) {
    uint8 leftbits = 0;
    uint8 rightbits = 0;

    for (int i = 4; i < 8; i++) {
        leftbits = (leftbits << 1) | (msg >> i & 0x01);
    }

    for (int i = 0; i < 4; i++) {
        rightbits = (rightbits << 1) | (msg >> i & 0x01);
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
        return reverseByte(res);
        // return reverseByte((S_BOX_ONE[rightbits] << 4) | S_BOX_ZERO[leftbits]);
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
        return reverseByte(res);
        // return reverseByte((S_BOX_ONE[leftbits] << 4) | S_BOX_ZERO[rightbits]);
    }
}

uint8 applyPermutation(uint8 interrupted) {
	// return BYTE_PERMUTE[interrupted];
	return permuteByte(interrupted);
}

void applyDiffusion(uint8 permuted, uint8 offset, array<uint8> @msg_half) {
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

void stepfn(uint8 key_byte, uint8 upper_msg_byte, bool icb, uint8 step_num, array<uint8> @lower_msg_half) {
    uint8 confused_byte = applySBoxes(upper_msg_byte, icb);
    uint8 interrupted_byte = confused_byte ^ key_byte;
    uint8 permuted_byte = applyPermutation(interrupted_byte);
    applyDiffusion(permuted_byte, step_num, lower_msg_half);
}

void swapArray(array<uint8> @arr1, array<uint8> @arr2) {
    if (arr1.length() != arr2.length()) {
        Print("swapArray: Invalid Array Length\n");
        return;
    }
    uint8 tmp;
    int length = arr1.length();
    for (int i = 0; i < length; i++) {
        tmp = arr1[i];
        arr1[i] = arr2[i];
        arr2[i] = tmp;
    }
}

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
            stepfn(key_byte, upper_msg_byte, icb, step, msg_lower_half);
        }
        swapArray(msg_lower_half, msg_upper_half);
    }

    swapArray(msg_lower_half, msg_upper_half);

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
			stepfn(key_byte, upper_msg_byte, icb, step, msg_lower_half);
		}

		swapArray(msg_lower_half, msg_upper_half);
	}

	swapArray(msg_lower_half, msg_upper_half);

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
    // for (int i = 0; i < 64; i++){
    //     Print(params[i], true);
    // }
    array<uint8> key(16);
    array<uint8> msg(48);
    CopyArray(key, params, 16);
    CopyArray(msg, params, 48, 0, 16);

    // Print("key: ");
    // DumpArray(key, 16, true);
    // Print("msg: ");
    // DumpArray(msg, 48, true);

    array<uint8> block(16);
    for (int i = 0; i < 3; i++){
        CopyArray(block, msg, 16, 0, i*16);
        Print("block: ");
        DumpArray(block, 16);
        Print(", ");
        encrypt(key, block);
        DumpArray(block, 16);
        Print(", ");
        decrypt(key, block);
        DumpArray(block, 16, true);
    }

    Print("reverseByte: ");
    Print(32, true);
    Print(reverseByte(32), true);
}