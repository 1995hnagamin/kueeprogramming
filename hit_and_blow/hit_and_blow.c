#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>
#define MAXN 10000
#define NUMBERS 5040

int LEGALNUMS[NUMBERS];

typedef struct hint {
  int hit, blow;
} Hint;

typedef struct candidates {
  bool cand[NUMBERS];
  int active;
} Candidates;

typedef struct checker {
  Hint (*call)(int);
} Checker;

typedef struct guesser {
  int (*call)();
} Guesser;

bool is_legal(int n) {
  bool num[10];
  memset(num, false, 10);
  int i, mod;
  for (i = 0; i < 4; ++i) {
    mod = n % 10;
    if (num[mod]) {
      return false;
    }
    num[mod] = true;
    n /= 10;
  }
  return true;
}

void init() {
  int n;
  int idx = 0;
  for (n = 0; n < 10000; ++n) {
    if (is_legal(n)) {
      LEGALNUMS[idx] = n;
      ++idx;
    }
  }
}

Candidates new_candidates() {
  Candidates c;
  int i;
  for (i = 0; i < NUMBERS; i++) {
    c.cand[i] = true;
  }
  c.active = NUMBERS;
  return c;
}

int randbetween(int lower, int upper) {
  return (rand() % (upper - lower + 1)) + lower;
}

int popcount(int bits) {
  int i;
  int count = 0;
  for (i = bits; i != 0; i &= i - 1) {
    count++;
  }
  return count;
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

int make_answer() {
  int idx = randbetween(0, NUMBERS);
  return LEGALNUMS[idx];
}

int Answer;
Hint step_question(Guesser guesser) {
  int guess = guesser.call();
  Hint hint = get_hint(guess, Answer);
  return hint;
}

int get_guess_from_user() {
  int guess;
  printf("> ");
  scanf("%d", &guess);
  return guess;
}

void give_question() {
  Hint hint;
  Answer = make_answer();
  Guesser user;
  user.call = &get_guess_from_user;

  do {
    hint = step_question(user);
    printf("%d Hit, %d Blow\n", hint.hit, hint.blow);
  } while (hint.hit != 4);
}

int select_candidate_randomly(Candidates *c) {
  int idx = randbetween(0, c->active);
  int i = 0, count = 0;
  while (count < idx) {
    i++;
    while (!(c->cand[i])) { i++; }
    count++;
  }
  return LEGALNUMS[i];
}

bool eq_hint(Hint a, Hint b) {
  return (a.hit == b.hit) && (a.blow == b.blow);
}

void squeeze_candidates(Candidates *c, int guess, Hint feedback) {
  int i;
  for (i = 0; i < NUMBERS; ++i) {
    Hint h; 
    if (!(c->cand[i])) {
      continue;
    }
    h = get_hint(guess, LEGALNUMS[i]);
    if (!eq_hint(h, feedback)) {
      c->cand[i] = false;
      c->active--;
    }
  }
}

Hint get_feedback_from_user(int guess) {
  Hint hint;
  printf("%04d\n", guess);
  printf("Hit > ");
  scanf("%d", &hint.hit);
  printf("Blow > ");
  scanf("%d", &hint.blow);
  return hint;
}

Candidates Cands;
bool step_guess(Checker oracle) {
  int guess = select_candidate_randomly(&Cands);
  Hint feedback = oracle.call(guess);
  if (feedback.hit == 4) {
    return true;
  } else {
    squeeze_candidates(&Cands, guess, feedback);
    return false;
  }
}

void guess_answer() {
  Cands = new_candidates();
  bool finished = false;
  Checker user;
  user.call = &get_feedback_from_user;
  while (!finished) {
    finished = step_guess(user);
  }
}

int main() {
  srand((unsigned)time(NULL));
  init();
  give_question();
}
