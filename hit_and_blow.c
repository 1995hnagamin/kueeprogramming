#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

bool ISLEGAL[10000];
int LEGALNUMS[5040];

typedef struct hint {
  int hit, blow;
} Hint;

typedef struct candidates {
  bool cand[10000];
} Candidates;

Candidates new_candidates() {
  Candidates c;
  int i;
  for (i = 0; i < 10000; i++) {
    c.cand[i] = true;
  }
  return c;
}

int randbetween(int lower, int upper) {
  return lower + rand() % (upper - lower + 1);
}

int min(int x, int y) {
  return (x < y ? x : y);
}

int max(int x, int y) {
  return (x > y ? x : y);
}

int popcount(int bits) {
  int i;
  int count = 0;
  for (i = bits; i != 0; i &= i - 1) {
    count++;
  }
  return count;
}

bool legal_number(int number) {
  bool digit[10];
  int i, mod;
  for (i = 0; i < 10; i++) {
    digit[i] = false;
  }
  while (number > 0) {
    mod = number % 10;
    if (digit[mod]) {
      return false;
    }
    digit[mod] = true;
    number /= 10;
  }
  return true;
}

// count Hits & Blows
Hint get_hint(int guess, int answer) {
  Hint hint = {0, 0};
  int gbits = 0, abits = 0;
  int i, g, a;
  for (i = 0, g = guess, a = answer; i < 4; i++) {
    if (g % 10 == a % 10) {
      hint.hit++;
    }
    gbits |= 1 << (g % 10);
    abits |= 1 << (a % 10);
    g /= 10;
    a /= 10;
  }
  hint.blow = popcount(gbits & abits) - hint.hit;
  return hint;
}

void give_question() {
  int answer, guess;
  Hint hint;

  answer = rand() % 10000;
  printf("ans=%d\n", answer);

  do {
    scanf("%d", &guess);
    hint = get_hint(guess, answer);
    printf("%d Hit, %d Blow\n", hint.hit, hint.blow);
  } while (guess != answer);
}

int main() {
  give_question();
}
