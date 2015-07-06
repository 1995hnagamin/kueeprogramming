#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>
#define MAXN 10000
#define NUMBERS 5040
#define INF 5050

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
  int bits = 0; // 使っている数の集合のビット表現
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

/* lower以上upper未満の乱数を返す */
int randbetween(int lower, int upper) {
  return (rand() % (upper - lower)) + lower;
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

/* ゲームを1ステップ実行する
 * guesserに解を予想させ、oracleに解チェックさせる
 */
bool step_game(Guesser guesser, Oracle oracle) {
  int guess = guesser.call();
  Hint feedback = oracle.call(guess);
  guesser.feedback(guess, feedback);
  return (feedback.hit == 4);
}

/* ゲームを開始する
 * 4 Hit, 0 Blow になるまでstep_gameを繰り返す
 */
int start_game(Guesser guesser, Oracle oracle) {
  bool finished = false;
  int count = 0;
  while (!finished) {
    count++;
    finished = step_game(guesser, oracle);
  }
  return count;
}

/* 解の予想戦略
 * *c中の有効な解の候補の中から無作為に一つ選んで返す
 */
int select_candidate_randomly(Candidates *c) {
  int target = randbetween(0, c->active);
  int i = 0, idx = 0;
  int hoge[NUMBERS];
  for (i = 0; i < NUMBERS; ++i) {
    if (c->cand[i]) {
      hoge[idx++] = LEGALNUMS[i];
    }
    if (idx > target) break;
  }
  return hoge[target];
}

int maximum(int arr[], int size) {
  int max = arr[0];
  int i;
  for (i = 0; i < size; ++i) {
    max = (max < arr[i] ? arr[i] : max);
  }
  return max;
}

int count_width(Candidates *c, int answer) {
  int hints[30];
  int i;
  for (i = 0; i < 30; ++i) { hints[i] = 0; }
  for (i = 0; i < NUMBERS; ++i) {
    if (!c->cand[i]) { continue; }
    Hint h = count_hit_blow(LEGALNUMS[i], answer);
    hints[h.hit * 5 + h.blow]++;
  }
  int max = 0;
  for (i = 0; i < 30; ++i) {
    if (max < hints[i]) max = hints[i];
    //printf("$%d,",hints[i]);
  }
  //putchar('\n');
  return max;
}

int minimize_width(Candidates *c) {
  int i = 0, idx = 0;
  int min_width = INF;
  int cands[NUMBERS];
  if (c->active > 200 || c->active == 1) {
    return select_candidate_randomly(c);
  }
  for (i = 0; i < NUMBERS; ++i) {
    if (!c->cand[i])continue;
    int n = LEGALNUMS[i];
    int width;
    width = count_width(c, n);
    if (min_width > width) {
      idx = 0;
      min_width = width;
      cands[idx++] = n;
    } else if (min_width == width) {
      cands[idx++] = n;
    }
  }
  ////printf("idx: %d\n", idx);
  return cands[randbetween(0,idx)];
}

/* *cの中で解の候補となり得ない数を消す
 */
void squeeze_candidates(Candidates *c, int guess, Hint feedback) {
  int i;
  for (i = 0; i < NUMBERS; ++i) {
    Hint h; 
    if (!c->cand[i]) { continue; }
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

int minimize_width_of_Cands() {
  if (Cands.active == NUMBERS) {
    return select_candidate_randomly(&Cands);
  } else {
    return minimize_width(&Cands);
  }
}

void squeeze_Cands(int guess, Hint hint) {
  squeeze_candidates(&Cands, guess, hint);
}

int guess_answer() {
  Guesser random = { &select_from_Cands, &squeeze_Cands };
  Oracle user = { &get_feedback_from_user };
  Cands = new_candidates();
  int count;
  printf("各桁に重複のない4桁の数を決めてください。\n");
  count = start_game(random, user);
  return count;
}

/* 出題モード */

int Answer;
Hint check_answer(int guess) {
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

int give_question() {
  Guesser user = { &get_guess_from_user, &show_hint };
  Oracle oracle = { &check_answer };
  Answer = make_answer();  
  int count;
  count = start_game(user, oracle);
  return count;
}

/* 自動対戦モード */

void show_hint_and_squeeze_Cands(int guess, Hint hint) {
  printf("%04d (%d Hit, %d Blow ", guess, hint.hit, hint.blow);
  squeeze_Cands(guess, hint);
  printf("解の候補: %4d個)\n", Cands.active);
  return;
}

int auto_fight(bool optimized) {
  Guesser random = { 
    (optimized ? &minimize_width_of_Cands : &select_from_Cands),
     &show_hint_and_squeeze_Cands };
  Oracle oracle = { &check_answer };
  Cands = new_candidates();
  int count;
  Answer = make_answer();
  count = start_game(random, oracle);
  return count;
}

int auto_fight_without_print(int answer, bool optimized) {
  Guesser random = { 
    (optimized ? &minimize_width_of_Cands : &select_from_Cands),
     &squeeze_Cands };
  Oracle oracle = { &check_answer };
  Cands = new_candidates();
  int count;
  Answer = answer;
  count = start_game(random, oracle);
//  printf("count:%d\n", count);
  return count;
}

void auto_fight_5040_times(bool optimized) {
  int i;
  int result[101];
  int max_count = -1, sum_count = 0;
  double average;
  for (i = 0; i < 101; ++i) { result[i] = 0; }
  for (i = 0; i < NUMBERS; ++i) {
    int count;
/*    if (optimized && i % 250 == 0 && i > 0) {
      printf("%4d games have been processed...\n", i);
    }*/
    count = auto_fight_without_print(LEGALNUMS[i], optimized);
    if (count > 99) {
      result[0]++;
    } else {
      result[count]++;
    }
    max_count = (count > max_count ? count : max_count);
    sum_count += count;
  }
  if (max_count > 99) max_count = 99;
  for (i = 1; i <= max_count; ++i) {
    printf("%2d: %4d times\n", i, result[i]);
  }
  if (result[0] > 0) {
    printf("99<: %3d times\n", result[0]);
  }
  average = ((double)sum_count) / 5040.0;
  printf("average: %lf\n", average);
  return;
}

int main(int argc, char *argv[]) {
  srand((unsigned)time(NULL));
  init();
  if (argc > 1 && strcmp(argv[1], "1") == 0) {
    give_question();
  } else if (argc > 1 && strcmp(argv[1], "2") == 0) {
    guess_answer();
  } else if (argc > 1 && strcmp(argv[1], "3") == 0) {
    auto_fight(false);
  } else if (argc > 1 && strcmp(argv[1], "3O") == 0) {
    auto_fight(true);
  } else if (argc > 1 && strcmp(argv[1], "4") == 0) {
    auto_fight_5040_times(false);
  } else if (argc > 1 && strcmp(argv[1], "4O") == 0) {
    auto_fight_5040_times(true);
  } else {
    printf("使い方:\n");
    printf("%s 1 : 出題モード\n", argv[0]);
    printf("%s 2 : 解答モード\n", argv[0]);
    printf("%s 3 : 自動対戦モード\n", argv[0]);
  }
  return 0;
}
