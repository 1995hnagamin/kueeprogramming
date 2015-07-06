#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define N 10

double rand0to1() {
  return ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
}

int get_random(int minimum, int width) {
  return minimum + (int)(rand0to1() * width);
}

void set(int *arr, int size) {
  int i;
  for (i = 0; i < size; i++) {
    arr[i] = get_random(0, 10000);
  }
  return;
}

int show_to_file(int *arr, int size, FILE *fpw) {
  int i, success;
  for (i = 0; i < size; i++) {
    if (i > 0) {
      success = fprintf(fpw, " %d", arr[i]);
    } else {
      success = fprintf(fpw, "%d", arr[i]);
    }
    if (success < 0) {
      return 1;
    }
  }
  success = fprintf(fpw, "\n");
  return (success >= 0 ? 0 : 1);
}

void swap(int *x, int *y) {
  int temp = *x;
  *x = *y;
  *y = temp;
  return;
}

void sort(int *arr, int size) {
  int i, finished, limit;
  if (size < 2) {
    return;
  }
  finished = 0;
  for (limit = size; !finished; limit--) {
    finished = 1;
    for (i = 0; i + 1 < limit; i++) {
      if (arr[i] > arr[i+1]) {
        swap(&arr[i], &arr[i+1]);
        finished = 0;
      }
    }
  }
  return;
}

int main(int argc, char *argv[]) {
  int num = 0;
  
  if (argc <= 1) {
    printf("specify size of array\n");
    return 0;
  } else if (argc <= 2) {
    printf("specify finename\n");
    return 0;
  }

  int size = atoi(argv[1]);
  if (size > N || N < 1) {
    printf("invalid size of array: %d (must be between 1 to %d)\n",size, N);
    return 0;
  }

  FILE *fpw = NULL;
  fpw = fopen(argv[2], "w");
  if (fpw == NULL) {
    printf("failed to open file: %s\n", argv[2]);
    fclose(fpw);
    return 0;
  }

  srand((unsigned)time(NULL));
  int data[N];
  set(data, N);
  sort(data, N);
  int result = show_to_file(data, N, fpw);
  if (result != 0) {
    printf("failed to write to file: %s\n", argv[2]);
  }
  fclose(fpw);
  return 0;
}
