#include <stdio.h>
#include "../hash.h"

int main() {
    uint8_t test[] = { 0, 5, 3 };
    uint8_t t_result[4] = { 0 };
    int n = sizeof(test);

    hash_packet(test, t_result, &n);

    printf("%c\n", t_result[0]);
    return 0;
}
