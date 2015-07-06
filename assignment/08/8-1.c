#include<stdio.h>
#include<string.h>
#define N 1024

void swap(int* x, int* y) {
  int tmp = *x;
  *x = *y;
  *y = tmp;
}

void bsort(int* xs, int n) {
  int flag = 1;
  int i, j;
  for (j = 1;; ++j) {
    flag = 0;
    for (i = 0; i < n - 1; ++i) {
      if (xs[i] > xs[i + 1]) {
        swap(&xs[i], &xs[i + 1]);
        flag = 1;
      }
    }
    if (!flag) return;
    printf("loop%d:", j);
    for (i = 0; i < n; ++i) {
      printf(" %d", xs[i]);
    }
    putchar('\n');
  }
}

int main() {
  char line[N] = "";
  char buffer[N] = "";
  int xs[N] = {};
  int i = 0;
  int n;
  printf("Data: ");
  fgets(line, sizeof(line), stdin);
  printf("N: ");
  fgets(buffer, sizeof(buffer), stdin);
  sscanf(buffer, "%d", &n);

  while (1) {
    char* p = strtok((i == 0 ? line:NULL), " ");
    if (p == NULL) break;
    sscanf(p, "%d", &xs[i]);
    ++i;
  }
  n = i;

  bsort(xs, n);

  return 0;
}
