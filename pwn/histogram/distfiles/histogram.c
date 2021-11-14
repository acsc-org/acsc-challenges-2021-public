#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define WEIGHT_MAX 600 // kg
#define HEIGHT_MAX 300 // cm
#define WEIGHT_STRIDE 10
#define HEIGHT_STRIDE 10
#define WSIZE (WEIGHT_MAX/WEIGHT_STRIDE)
#define HSIZE (HEIGHT_MAX/HEIGHT_STRIDE)

int map[WSIZE][HSIZE] = {0};
int wsum[WSIZE] = {0};
int hsum[HSIZE] = {0};

/* Fatal error */
void fatal(const char *msg) {
  printf("{\"status\":\"error\",\"reason\":\"%s\"}", msg);
  exit(1);
}

/* Call this function to get the flag! */
void win(void) {
  char flag[0x100];
  FILE *fp = fopen("flag.txt", "r");
  int n = fread(flag, 1, sizeof(flag), fp);
  printf("%s", flag);
  exit(0);
}

int read_data(FILE *fp) {
  /* Read data */
  double weight, height;
  int n = fscanf(fp, "%lf,%lf", &weight, &height);
  if (n == -1)
    return 1; /* End of data */
  else if (n != 2)
    fatal("Invalid input");

  /* Validate input */
  if (weight < 1.0 || weight >= WEIGHT_MAX)
    fatal("Invalid weight");
  if (height < 1.0 || height >= HEIGHT_MAX)
    fatal("Invalid height");

  /* Store to map */
  short i, j;
  i = (short)ceil(weight / WEIGHT_STRIDE) - 1;
  j = (short)ceil(height / HEIGHT_STRIDE) - 1;
  
  map[i][j]++;
  wsum[i]++;
  hsum[j]++;

  return 0;
}

/* Print an array in JSON format */
void json_print_array(int *arr, short n) {
  putchar('[');
  for (short i = 0; i < n; i++) {
    printf("%d", arr[i]);
    if (i != n-1) putchar(',');
  }
  putchar(']');
}

int main(int argc, char **argv) {
  if (argc < 2)
    fatal("No input file");

  /* Open CSV */
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL)
    fatal("Cannot open the file");

  /* Read data from the file */
  int n = 0;
  while (read_data(fp) == 0)
    if (++n > SHRT_MAX)
      fatal("Too many input");

  /* Show result */
  printf("{\"status\":\"success\",\"result\":{\"wsum\":");
  json_print_array(wsum, WSIZE);
  printf(",\"hsum\":");
  json_print_array(hsum, HSIZE);
  printf(",\"map\":[");
  for (short i = 0; i < WSIZE; i++) {
    json_print_array(map[i], HSIZE);
    if (i != WSIZE-1) putchar(',');
  }
  printf("]}}");

  fclose(fp);
  return 0;
}
