#include<stdio.h>
#include<math.h>

typedef struct {
  double x, y;
} Vector2d;

double GetLength(Vector2d v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

void ScaleVector(Vector2d *v, double s) {
  v->x = v->x * s;
  v->y = v->y * s;
  return;
}

int main() {
  Vector2d v;
  double s;
  printf("Input 2D Vector: ");
  scanf("%lf %lf", &v.x, &v.y);
  printf("Input scale value: ");
  scanf("%lf", &s);
  ScaleVector(&v, s);
  printf("Result : %lf %lf\n", v.x, v.y);
  printf("Length : %lf\n", GetLength(v));
 
  return 0;
}
