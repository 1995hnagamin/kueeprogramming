#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

typedef struct hint {
  int hit, blow;
} Hint;

int randbetween(int lower, int upper) {
  return lower + rand() % (upper - lower + 1);
}

int min(int x, int y) {
  return (x < y ? x : y);
}

int max(int x, int y) {
  return (x > y ? x : y);
}

Hint get_hint(int guess, int answer) {
  Hint hint = {0, 0};
  int countg[10] = {0}, counta[10] = {0};
  int i = 0, g, a;
  for (i = 0; i < 4; i++) {
    g = guess % 10, a = answer % 10;
    countg[g]++, counta[a]++;
    if (g == a) {
      hint.hit++;
    }
    guess /= 10;
    answer /= 10;
  }
  for (i = 0; i < 10; i++) {
    hint.blow += min(counta[i], countg[i]);
  }
  hint.blow -= hint.hit;
  return hint;
}

typedef struct candidate {
  bool cand[10000];
} Candidate;

Candidate new_candidate() {
  Candidate c;
  int i;
  for (i = 0; i < 10000; i++) {
    c.cand[i] = true;
  }
  return c;
}

typedef struct classification {
  int classes[5][5];
} Classification;

Classification new_classification() {
  Classification c;
  int i, j;
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      c.classes[i][j] = 0;
    }
  }
  return c;
}

Classification classify(int guess, Candidate candidate) {
  int i;
  Hint hint;
  Classification classes = new_classification();
  for (i = 0; i < 10000; i++) {
    if (candidate.cand[i]) {
      hint = get_hint(guess, i);
      classes.classes[hint.hit][hint.blow]++;
    }
  }
  return classes;
}

int max_size(Classification classes) {
  int i, j;
  int size = -1;
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      size = max(size, classes.classes[i][j]);
    }
  }
  return size;
}

void solve() {
  Classification classes;
  Candidate candidate = new_candidate();
  int i;
}

void questionnaire() {
  int kaisuu = 0;
  int answer, guess;
  Hint hint;
  guess = -1;
  answer = randbetween(0, 9999);
  while (guess != answer) {
    scanf("%d", &guess);
    hint = get_hint(guess, answer);
    printf("%d Hit, %dBlow\n", hint.hit, hint.blow);
    kaisuu++;
  }
  printf("answer: %d\n", answer);
  printf("kaisuu: %d\n", kaisuu);
  if (kaisuu < 7) {
    printf("tuyoi\n");
  } else {
    printf("yowai\n");
  }
}

int main(int argc, char *argv[]) {
  srand((unsigned)time(NULL));
  questionnaire();
  return 0;
}
