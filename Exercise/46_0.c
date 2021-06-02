#include <stdio.h>
#include <stddef.h>

struct a
{
    long a;
    int b;
    long c;
};

int main()
{
    printf("%ld\n", (long)offsetof(struct a, c) - (long)offsetof(struct a, b) + 8);
    printf("%ld\n", sizeof(struct a) - 8);

    return 0;
}