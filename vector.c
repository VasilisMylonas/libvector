//
// The MIT License (MIT)
//
// Copyright (c)  2022 Vasilis Mylonas
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//

#include "vector.h"

#include <stdlib.h>
#include <string.h>

static void x_memcpy(void* dest, const void* src, size_t size)
{
    memcpy(dest, src, size);
}

static void x_memmove(void* dest, const void* src, size_t size)
{
    memmove(dest, src, size);
}

static void x_memswap(void* restrict a, void* restrict b, size_t size)
{
    if (a == b)
    {
        return;
    }

    char* temp[size];
    x_memcpy(temp, a, size);
    x_memcpy(a, b, size);
    x_memcpy(b, temp, size);
}

int _vector_create(void** self, size_t elem_size, size_t capacity)
{
    struct _vector_header* vec =
        malloc(sizeof(struct _vector_header) + VECTOR_DEFAULT_CAP * elem_size);

    if (vec == NULL)
    {
        return ENOMEM;
    }

    vec->cap = capacity == 0 ? VECTOR_DEFAULT_CAP : capacity;
    vec->size = 0;
    vec->elem_size = elem_size;

    *self = vec + 1;
    return 0;
}

size_t vector_size(void* self)
{
    return _VECTOR_HEADER(self)->size;
}

size_t vector_cap(void* self)
{
    return _VECTOR_HEADER(self)->cap;
}

void vector_clear(void* self)
{
    _VECTOR_HEADER(self)->size = 0;
}

void vector_destroy(void* self)
{
    free(_VECTOR_HEADER(self));
    *((void**)self) = NULL;
}

int vector_reserve(void* self, size_t elem_count)
{
    struct _vector_header* vec = _VECTOR_HEADER(self);
    size_t desired_size = vec->size + elem_count;
    size_t new_cap = vec->cap;

    if (desired_size < vec->cap)
    {
        return 0;
    }

    while (desired_size >= new_cap)
    {
        new_cap *= 2;
    }

    struct _vector_header* new_vec =
        realloc(vec, sizeof(struct _vector_header) + new_cap * vec->elem_size);
    if (new_vec == NULL)
    {
        return ENOMEM;
    }

    new_vec->cap = new_cap;
    *((void**)self) = new_vec + 1;

    return 0;
}

int vector_pack(void* self)
{
    struct _vector_header* vec = _VECTOR_HEADER(self);
    size_t new_cap = vec->size == 0 ? 1 : vec->size;

    struct _vector_header* new_vec =
        realloc(vec, sizeof(struct _vector_header) + new_cap * vec->elem_size);
    if (new_vec == NULL)
    {
        return ENOMEM;
    }

    new_vec->cap = new_cap;
    *((void**)self) = new_vec + 1;

    return 0;
}

int vector_dup(void* self, void* new)
{
    size_t size = vector_size(self);
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;

    int result = _vector_create(new, elem_size, size);
    if (result != 0)
    {
        return result;
    }

    x_memcpy(*(void**)new, *(void**)self, size * elem_size);
    _VECTOR_HEADER(new)->size = size;
    _VECTOR_HEADER(new)->elem_size = elem_size;

    return 0;
}

int vector_cat(void* self, size_t count, const void* array)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    int result = vector_reserve(self, count);
    if (result != 0)
    {
        return result;
    }

    char* dest = ((char*)*(void**)self) + vector_size(self) * elem_size;
    x_memcpy(dest, array, count * elem_size);
    _VECTOR_HEADER(self)->size += count;

    return 0;
}

void vector_sort(void* self, int (*cmp_func)(const void*, const void*))
{
    qsort(*(void**)self, vector_size(self), _VECTOR_HEADER(self)->elem_size, cmp_func);
}

size_t vector_bsearch(void* self, const void* value, int (*cmp_func)(const void*, const void*))
{
    void* found =
        bsearch(value, *(void**)self, vector_size(self), _VECTOR_HEADER(self)->elem_size, cmp_func);

    if (found == NULL)
    {
        return VECTOR_NOT_FOUND;
    }

    return found - *(void**)self;
}

static void vector_shl(void* self)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    char* first = *(void**)self;
    size_t size = vector_size(self);

    x_memmove(first, first + elem_size, (size - 1) * elem_size);
}

static void vector_shr(void* self)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    char* first = *(void**)self;
    size_t size = vector_size(self);

    vector_reserve(self, 1);
    x_memmove(first + elem_size, first, (size - 1) * elem_size);
}

size_t vector_find(void* self, const void* value, int (*cmp_func)(const void*, const void*))
{
    char* first = *(void**)self;

    size_t size = vector_size(self);
    for (size_t i = 0; i < size; i++)
    {
        if (cmp_func(first, value) == 0)
        {
            return i;
        }

        first += _VECTOR_HEADER(self)->elem_size;
    }

    return VECTOR_NOT_FOUND;
}

size_t vector_rfind(void* self, const void* value, int (*cmp_func)(const void*, const void*))
{
    size_t size = vector_size(self);

    char* last = *(void**)self;
    last += (size - 1) * _VECTOR_HEADER(self)->elem_size;

    for (size_t i = size; i > 0; i--)
    {
        if (cmp_func(last, value) == 0)
        {
            return i - 1;
        }

        last -= _VECTOR_HEADER(self)->elem_size;
    }

    return VECTOR_NOT_FOUND;
}

void vector_reverse(void* self)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    size_t size = vector_size(self);
    char* first = *(void**)self;
    char* last = first + (size - 1) * elem_size;

    while (first < last)
    {
        x_memswap(first, last, elem_size);
        first += elem_size;
        last -= elem_size;
    }
}

void vector_rotl(void* self)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    size_t size = vector_size(self);

    char* first = *(void**)self;
    char* temp[elem_size];
    x_memcpy(temp, first, elem_size);

    vector_shl(self);
    x_memcpy(first + (size - 1) * elem_size, temp, elem_size);
}

void vector_rotr(void* self)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    size_t size = vector_size(self);

    char* last = ((char*)*(void**)self) + (size - 1) * elem_size;
    char* temp[elem_size];
    x_memcpy(temp, last, elem_size);

    vector_shr(self);
    x_memcpy(*(void**)self, temp, elem_size);
}

void vector_fill(void* self, const void* value)
{
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    size_t size = vector_size(self);
    for (size_t i = 0; i < size; i++)
    {
        x_memcpy((char*)(*(void**)self) + i * elem_size, value, elem_size);
    }
}

int vector_push(void* self, const void* value)
{
    int result = vector_reserve(self, 1);
    if (result != 0)
    {
        return result;
    }

    size_t size = vector_size(self);
    size_t elem_size = _VECTOR_HEADER(self)->elem_size;
    x_memcpy((char*)(*(void**)self) + size * elem_size, value, elem_size);
    _VECTOR_HEADER(self)->size++;

    return 0;
}

bool vector_eq(void* self, void* other, int (*cmp_func)(const void* a, const void* b))
{
    size_t size1 = vector_size(self);
    size_t size2 = vector_size(other);

    if (size1 != size2)
    {
        return false;
    }

    size_t elem_size = _VECTOR_HEADER(self)->elem_size;

    for (size_t i = 0; i < size1; i++)
    {
        if (cmp_func(&(*(char**)self)[i * elem_size], &(*(char**)other)[i * elem_size]) != 0)
        {
            return false;
        }
    }

    return true;
}
