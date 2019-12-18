// Written by Emmanuel Butor
// test1-simple.c
// Tests all of the public methods defined in dymat.h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
        assert(*x == 50);
        destroy_dymatobj();
    }
    // TEST2: t_calloc
    {
        printf("\n[TEST2] Initiate Test\n");
        int* array = t_calloc(4, sizeof(int));
        array[0] = 5;
        array[1] = 10;
        array[2] = 20;
        array[3] = -10;
        printf("[TEST2] Array of 4 integers have been created.\n");
        printf("[TEST2] array[0] = %d, 5 is expected.\n", array[0]);
        assert(array[0] == 5);
        printf("[TEST2] array[1] = %d, 10 is expected.\n", array[1]);
        assert(array[1] == 10);
        printf("[TEST2] array[2] = %d, 20 is expected.\n", array[2]);
        assert(array[2] == 20);
        printf("[TEST2] array[3] = %d, -10 is expected.\n", array[3]);
        assert(array[3] == -10);
        destroy_dymatobj();
    }
    // TEST3: t_free and is_null
    {
        printf("\n[TEST3] Initiate Test\n");
        int* x = t_malloc(sizeof(int));
        printf("[TEST3] Check: is x null? returned %s, false is expected.\n", (is_null(x)?"true":"false") );
        assert(!is_null(x));
        t_free(x);
        printf("[TEST3] Check: is x null? returned %s, true is expected.\n", (is_null(x)?"true":"false") );
        assert(is_null(x));
        destroy_dymatobj();
    }
    // TEST4: freeall
    {
        printf("\n[TEST4] Initiate Test\n");
        int* x1 = t_malloc(sizeof(int));
        int* x2 = t_calloc(1, sizeof(int));
        int* array = t_calloc(16, sizeof(int));
        printf("[TEST4] Check x1, x2, array if any are null. At this step, none should be null.\n");
        assert(!is_null(x1));
        assert(!is_null(x2));
        assert(!is_null(array));
        freeall();
        printf("[TEST4] freeall() has been called. Now check if x1, x2, array are null. All should be null.\n");
        assert(is_null(x1));
        assert(is_null(x2));
        assert(is_null(array));
        destroy_dymatobj();
    }
}