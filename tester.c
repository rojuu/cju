#include <stdio.h>

extern float add(float a, float b);

int main() {
    printf("tester.c: result from add(3.0f, 4.0f) = %f\n", add(3.0f, 4.0f));
    return 0;
}