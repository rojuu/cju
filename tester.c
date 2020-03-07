#include <stdio.h>

extern float add(float a, float b);

int main(int argc, char **argv) {
    printf("add(3, 4) = %f\n", add(3, 4));
    return 0;
}