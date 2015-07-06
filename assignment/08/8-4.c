#include<stdio.h>

int main() {
  FILE *fpr = NULL, *fpw = NULL;
  char str[128];
  fpr = fopen("test.txt", "r");
  if (!fpr) {
    printf("File cannot open: test.txt\n");
    return 0;
  }

  fpw = fopen("test.bak", "w");

  while (fgets(str, 128, fpr))
    fputs(str, fpw);

  fclose(fpr);
  fclose(fpw);

  return 0;
}
