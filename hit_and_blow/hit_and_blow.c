#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>
#define MAXN 10000
#define NUMBERS 5040

/* 解になり得る4桁の数を保存する配列 */
int LEGALNUMS[NUMBERS];

/* Hit数とBlow数を保存する構造体 */
typedef struct hint {
  int hit, blow;
} Hint;

/* 解の候補を保存する構造体
 * cand[i]がtrueのときLEGALNUMS[i]は解の候補である
 * activeはtrueになっているcand[i]の総数(解の候補数)
 */
typedef struct candidates {
  bool cand[NUMBERS];
  int active;
} Candidates;

/* 解のチェックをするオブジェクト
 * call(guess)するとHit数とBlow数が分かる
 */
typedef struct Oracle {
  Hint (*call)(int guess);
} Oracle;

/* 解を予想するオブジェクト
 * call()すると予想した解を返す
 * feedback(guess, hint)してHit数とBlow数を伝える
 */
typedef struct Guesser {
  int (*call)();
  void (*feedback)(int guess, Hint hint);
} Guesser;

/* bits中に立っているbitの数を返す */
int popcount(int bits) {
  int i;
  int count = 0;
  for (i = bits; i != 0; i &= i - 1) {
    count++;
  }
  return count;
}

/* nが解としてふさわしいか(0〜9を1回ずつ使っている4桁の数か)を判定する */
bool is_legal(int n) {
  int bits = 0;
  int i;
  if (n < 0 || 9999 < n) {
    return false;
  }
  for (i = 0; i < 4; ++i) {
    int mod = n % 10;
    bits |= 1 << mod;
    n /= 10;
  }
  return popcount(bits) == 4;
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

Hint count_hit_blow(int guess, int answer) {
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

bool eq_hint(Hint a, Hint b) {
  return (a.hit == b.hit) && (a.blow == b.blow);
}

bool step_game(Guesser guesser, Oracle oracle) {
  int guess = guesser.call();
  Hint feedback = oracle.call(guess);
  guesser.feedback(guess, feedback);
  return (feedback.hit == 4);
}

void start_game(Guesser guesser, Oracle oracle) {
  bool finished = false;
  while (!finished) {
    finished = step_game(guesser, oracle);
  }
  return;
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

void squeeze_candidates(Candidates *c, int guess, Hint feedback) {
  int i;
  for (i = 0; i < NUMBERS; ++i) {
    Hint h; 
    if (!(c->cand[i])) {
      continue;
    }
    h = count_hit_blow(guess, LEGALNUMS[i]);
    if (!eq_hint(h, feedback)) {
      c->cand[i] = false;
      c->active--;
    }
  }
}

int make_answer() {
  int idx = randbetween(0, NUMBERS);
  return LEGALNUMS[idx];
}

/* 解答モード */

Hint get_feedback_from_user(int guess) {
  Hint hint;
  printf("%04d\n", guess);
  printf("Hit数 > ");
  scanf("%d", &hint.hit);
  printf("Blow数 > ");
  scanf("%d", &hint.blow);
  return hint;
}

Candidates Cands;
int select_from_Cands() {
  return select_candidate_randomly(&Cands);
}

void squeeze_Cands(int guess, Hint hint) {
  squeeze_candidates(&Cands, guess, hint);
}

void guess_answer() {
  Guesser random = { &select_from_Cands, &squeeze_Cands };
  Oracle user = { &get_feedback_from_user };
  Cands = new_candidates();
  printf("各桁に重複のない4桁の数を決めてください。\n");
  start_game(random, user);
}

/* 出題モード */

int Answer;
Hint step_question(int guess) {
  Hint hint = count_hit_blow(guess, Answer);
  return hint;
}

int get_guess_from_user() {
  int guess;
  printf("> ");
  scanf("%d", &guess);
  return guess;
}

void show_hint(int guess, Hint hint) {
  printf("%04d (%d Hit, %d Blow)\n", guess, hint.hit, hint.blow);
  return;
}

void show_hint_and_squeeze_Cands(int guess, Hint hint) {
  printf("%04d (%d Hit, %d Blow)\n", guess, hint.hit, hint.blow);
  squeeze_Cands(guess, hint);
  return;
}

void give_question() {
  Guesser user = { &get_guess_from_user, &show_hint };
  Oracle oracle = { &step_question };
  Answer = make_answer();  
  start_game(user, oracle);
}

/* 自動対戦モード */

void auto_fight() {
  Guesser random = { &select_from_Cands, &show_hint_and_squeeze_Cands };
  Oracle oracle = { &step_question };
  Cands = new_candidates();
  Answer = make_answer();

  start_game(random, oracle);
}

int main(int argc, char *argv[]) {
  srand((unsigned)time(NULL));
  init();
  if (argc > 1 && strcmp(argv[1], "1") == 0) {
    give_question();
  } else if (argc > 1 && strcmp(argv[1], "2") == 0) {
    guess_answer();
  } else if (argc > 1 && strcmp(argv[1], "3") == 0) {
    auto_fight();
  } else {
    printf("使い方:\n");
    printf("%s 1 : 出題モード\n", argv[0]);
    printf("%s 2 : 解答モード\n", argv[0]);
    printf("%s 3 : 自動対戦モード\n", argv[0]);
  }
}
