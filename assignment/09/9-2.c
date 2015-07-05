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

void show(int *arr, int size) {
  int i;
  for (i = 0; i < size; i++) {
    if (i > 0) {
      putchar(' ');
    }
    printf("%d", arr[i]);
  }
  putchar('\n');
  return;
}

void swap(int *x, int *y) {
  int temp = *x;
  *x = *y;
  *y = temp;
  return;
}

void sort(int *arr, int size) {
  int i, j, finished, limit;
  if (size < 2) {
    return;
  }
  finished = 0;
  for (i = 0, limit = 0; (i < size - 1) && (!finished); i++, limit++) {
    finished = 1;
    for (j = 0; j + 1 < size - limit; j++) {
      if (arr[j] > arr[j+1]) {
        swap(&arr[j], &arr[j+1]);
        finished = 0;
      }
    }
  }
  return;
}

int main() {
  srand((unsigned)time(NULL));
  int data[N];
  set(data, N);
  sort(data, N);
  show(data, N);
  return 0;
}
