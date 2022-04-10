#include "vector.h"

#include <assert.h>
#include <stdlib.h>

void vector_create_destroy_test()
{
    int* vec = NULL;
    vector_create(&vec, 0);

    assert(vector_size(&vec) == 0);
    assert(vector_cap(&vec) == VECTOR_DEFAULT_CAP);

    vector_destroy(&vec);

    assert(vec == NULL);
}

void vector_fill_reverse_test()
{
    const int value = 5;

    int* vec = NULL;
    vector_create(&vec, 0);

    _VECTOR_HEADER(&vec)->size = _VECTOR_HEADER(&vec)->cap;

    vector_fill(&vec, &value);

    for (size_t i = 0; i < vector_size(&vec); i++)
    {
        assert(vec[i] == value);
    }

    vec[0] = value - 1;
    vec[vector_size(&vec) - 1] = value + 1;

    vector_reverse(&vec);

    assert(vec[0] == value + 1);
    assert(vec[vector_size(&vec) - 1] == value - 1);

    vector_destroy(&vec);
}

void vector_push_pop_test()
{
    int* vec = NULL;
    vector_create(&vec, 0);

    for (size_t i = 0; i < 10; i++)
    {
        vector_push(&vec, &i);
    }

    assert(vector_size(&vec) == 10);

    for (size_t i = 0; i < 10; i++)
    {
        assert(vec[i] == i);
    }

    for (size_t i = 10; i > 0; i--)
    {
        assert(vector_pop(&vec) == i - 1);
    }

    assert(vector_size(&vec) == 0);

    vector_destroy(&vec);
}

int main()
{
    vector_create_destroy_test();
    vector_fill_reverse_test();
    vector_push_pop_test();
}
