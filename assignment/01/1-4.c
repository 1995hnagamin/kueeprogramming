#include<stdio.h>
#include<math.h>

int main() {
  double a, b, c;
  printf("Type a: ");
  scanf("%lf", &a);
  printf("Type b: ");
  scanf("%lf", &b);
  printf("Type c: ");
  scanf("%lf", &c);

  double d = b * b - 4 * a * c;

  if (b * b >= 4 * a * c) {
    double ans1 = (-b + sqrt(d)) / (2 * a),
           ans2 = (-b - sqrt(d)) / (2 * a);
    printf("%f %f\n", ans1, ans2);
  } else {
    printf("no real roots\n");
  }
  return 0;
}
