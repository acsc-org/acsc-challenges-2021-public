#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(unsigned char* output) {
  int i;
  for (i = 0; i < 8; i++) printf("%02x", output[i]);
  printf("\n");
}

int main() {
  unsigned char keys[17] = {};
  int i;
  unsigned int cnt = 0;

  srand(218);
  while(1) {
    cnt += 1;
    for (i = 0; i < 16; i++) keys[i] = (int)(rand() % 50) + 40;
    //if (cnt == 3474256457 || cnt == 312772784) {
    if (cnt == 16694217 || cnt == 12858410) {
		printf("%u %s\n", cnt, keys);
		printf("=========================\n");
    }
    if(cnt == 1<<24) break;
  }
}
