#include <stdio.h>

void modeExamples(int x, int &y)
{
    x = 2 * x;
    y = y + 100;
}

int main()
{
    int a = 7;
    int b = 10;
    printf("a = %d, b = %d\n", a, b);
    modeExamples(a, b);
    printf("a = %d, b = %d\n", a, b);
    return 0;
}