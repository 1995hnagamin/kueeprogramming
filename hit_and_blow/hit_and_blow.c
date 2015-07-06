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

int max(int x, int y) {
  return (x > y ? x : y);
}

int min(int x, int y) {
  return (x < y ? x : y);
}

int maximum(int arr[], int size) {
  int m = arr[0];
  int i;
  for (i = 0; i < size; ++i) {
    m = max(m, arr[i]);
  }
  return m;
}

void init_array(int arr[], int size) {
  int i;
  for (i = 0; i < size; ++i) {
    arr[i] = 0;
  }
  return;
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
  int gbits = 0, abits = 0; // guess, answerで使われる数の集合
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
 * guesserに解を予想させ、oracleにチェックさせる
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

/* 解の候補cをanswerで割ったときの幅を返す
 * 
 * 解の候補がcの状態でanswerと解答するとき
 * 得られるヒントによって解の絞られ方が変わるが、
 * その中で最も絞れなかった場合の候補数を
 * cをanswerで割ったときの幅と呼ぶことにする
 */
int count_width(Candidates *c, int answer) {
  int hints[30];
  int i;
  init_array(hints, 30);
  for (i = 0; i < NUMBERS; ++i) {
    if (!c->cand[i]) { continue; }
    Hint h = count_hit_blow(LEGALNUMS[i], answer);
    hints[h.hit * 5 + h.blow]++;
  }
  return maximum(hints, 30);
}

/* 解の予想戦略
 * できるだけ候補cの幅を小さくするような解答を返す
 */
int minimize_width(Candidates *c) {
  int i = 0, idx = 0;
  int min_width = INF;
  int cands[NUMBERS / 2]; // 目的の解答を保存する配列
  
  /* 解の候補数が大きいときは無作為に選ぶようにすると成績が良くなった
   * 解の候補数が1のとき以下はうまく動かないので特別に処理する
   */
  if (c->active > 200 || c->active == 1) {
    return select_candidate_randomly(c);
  }
  
  /* 幅を最小にする解を探してcandsに保存する
   */
  for (i = 0; i < NUMBERS; ++i) {
    if (!c->cand[i]) continue; // ありえない解は無視すると成績が良くなった
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
  return cands[randbetween(0,idx)];
}

/* cの中で解の候補となり得ない数を消す
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

/* 解答モード */

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
int minimize_width_of_Cands() {
  return minimize_width(&Cands);
}

void squeeze_Cands(int guess, Hint hint) {
  squeeze_candidates(&Cands, guess, hint);
}

int guess_answer() {
  Guesser random = { &minimize_width_of_Cands, &squeeze_Cands };
  Oracle user = { &get_feedback_from_user };
  Cands = new_candidates();
  int count;
  count = start_game(random, user);
  return count;
}

/* 出題モード */

int make_answer() {
  int idx = randbetween(0, NUMBERS);
  return LEGALNUMS[idx];
}

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
  show_hint(guess, hint);
  squeeze_Cands(guess, hint);
  return;
}

/* answerは解の指定に使う ゼロなら新規作成し非ゼロならそれを使う
 * show_stepがtrueのときはゲームの進行状況を表示する
 */
int auto_fight(int answer, bool show_step) {
  Guesser random = { 
     &minimize_width_of_Cands,
     (show_step ? &show_hint_and_squeeze_Cands : &squeeze_Cands) };
  Oracle oracle = { &check_answer };
  Cands = new_candidates();
  int count;
  Answer = (answer == 0 ? make_answer() : answer);
  count = start_game(random, oracle);
  return count;
}

/* 5040回自動対戦モード */

void auto_fight_5040_times() {
  int i;
  
  /* 成績を保存する配列
   * 得点がiだったゲームはresult[i]個ある
   * 得点が99より大きいゲームはresult[0]個ある
   */
  int result[101]; 
  
  int max_count = -1, sum_count = 0;
  double average;
  init_array(result, 101);
  for (i = 0; i < NUMBERS; ++i) {
    int count;
    count = auto_fight(LEGALNUMS[i], false);
    if (count > 99) {
      result[0]++;
    } else {
      result[count]++;
    }
    max_count = max(max_count, count);
    sum_count += count;
  }
  for (i = 1; i <= min(max_count, 99); ++i) {
    printf("%2d: %4d\n", i, result[i]);
  }
  if (result[0] > 0) {
    printf("99<: %3d\n", result[0]);
  }
  average = ((double)sum_count) / 5040.0;
  printf("平均: %lf\n", average);
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
    auto_fight(0,true);
  } else if (argc > 1 && strcmp(argv[1], "4") == 0) {
    auto_fight_5040_times();
  } else {
    printf("使い方:\n");
    printf("%s 1 : 出題モード\n", argv[0]);
    printf("%s 2 : 解答モード\n", argv[0]);
    printf("%s 3 : 自動対戦モード\n", argv[0]);
    printf("%s 4 : 5040回自動対戦モード\n", argv[0]);
  }
  return 0;
}
