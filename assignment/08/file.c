#include<stdio.h>

int main() {
  FILE *fp;
  char str[128];

  fp = fopen("text.txt", "r");
  if (!fp) {
    printf("File cannot open\n");
    return 0;
  }
  while ( fgets(str, 128, fp) )
    printf("%s", str);
  printf("End\n");
  fclose(fp);
  return 0;
}
