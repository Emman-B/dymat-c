// TEST_PUBLIC1.c
// Tests all of the public methods defined in dymat.h

#include <stdio.h>
#include <stdlib.h>

#include "../src/dymat.h" // relative to where this file is located compared to where "dymat.h" is

int main()
{
    printf("Running TEST_PUBLIC1.c\n\n");
    printf("All tests will run destroy_dymatobj() after each test.\n\n");
    
    // TEST1: t_malloc
    {
        printf("[TEST1] Initiate Test\n");
        int* x = t_malloc(sizeof(int));
        *x = 50;
        printf("[TEST1] integer pointer x has value %d, 50 is expected\n", *x);
        destroy_dymatobj();
    }
    // TEST2: t_calloc
    {
        printf("[TEST2] Initiate Test\n");
        int* array = t_calloc(4, sizeof(int));
        array[0] = 5;
        array[1] = 10;
        array[2] = 20;
        array[3] = -10;
        printf("[TEST2] Array of 4 integers have been created.\n");
        printf("[TEST2] array[0] = %d, 5 is expected.\n", array[0]);
        printf("[TEST2] array[1] = %d, 10 is expected.\n", array[1]);
        printf("[TEST2] array[2] = %d, 20 is expected.\n", array[2]);
        printf("[TEST2] array[3] = %d, -10 is expected.\n", array[3]);
        destroy_dymatobj();
    }
}