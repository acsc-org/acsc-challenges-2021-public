/*
    g++ solve.c -lcrypto -Ofast -o solve
*/

#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/rc4.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void aes(unsigned char* key, unsigned char* output) {
  unsigned char input[16] = {};

  AES_KEY aeskey;
  AES_set_encrypt_key(key, 128, &aeskey);
  AES_ecb_encrypt(input, output, &aeskey, AES_ENCRYPT);
}

void des(unsigned char* key, unsigned char *out) {
  unsigned char input[8] = {};
  unsigned char output[8] = {};

  DES_cblock deskey;
  DES_key_schedule schedule;
  memcpy(deskey, key, 8);
  DES_set_key_unchecked(&deskey, &schedule);
  DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
  memcpy(out, output, 8);
}

void rc4(unsigned char* key, unsigned char* output) {
  unsigned char input[16] = {};

  RC4_KEY rc4key;
  RC4_set_key(&rc4key, 16, key);
  RC4(&rc4key, 16, input, output);
}

void calc(unsigned char* keys, unsigned char* output) {
  aes(keys, output);
  rc4(output, output);
  des(output, output);
}

void bxor(unsigned char* output, unsigned char* data, unsigned char* xor_data) {
  int i;
  for (i = 0; i < 6; i++) xor_data[i] = output[i] ^ data[i];
}

void mwrite(unsigned char* data, FILE* filep, int val) {
  int i;
  char tmp[8];
  memcpy(tmp, (unsigned char*)&val, 4);
  fwrite(data, 1, 5, filep);
  fwrite(tmp, 1, 4, filep);
}

int main() {
  unsigned char data[6] = {252, 176, 32, 159, 111, 158};
  unsigned char xor_data[6] = {};
  unsigned char output[16] = {};
  unsigned char keys[17] = {};
  int i;
  unsigned int cnt = 0;
  FILE* files[256];
  FILE* filesx[256];
  char fname[256];

  for(i=0;i<256;i++) {
	  sprintf(fname, "out1/val_%02d", i);
	  files[i] = fopen(fname, "w");
	  sprintf(fname, "out2/val_%02d", i);
	  filesx[i] = fopen(fname, "w");
  }
  srand(218);
  while(1) {
    cnt += 1;
    for (i = 0; i < 16; i++) keys[i] = (int)(rand() % 50) + 40;
    calc(keys, output);
    mwrite(output, files[(int)output[5]], cnt);
    bxor(output, data, xor_data);
    mwrite(xor_data, filesx[(int)xor_data[5]], cnt);
    if (cnt == 1<<24) {
		break;
    }
  }
  for(i=0;i<256;i++) {
	 fclose(files[i]);
	 fclose(filesx[i]);
  }
}
