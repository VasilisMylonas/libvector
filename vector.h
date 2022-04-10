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

#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @author Vasilis Mylonas <vasilismylonas@protonmail.com>
 * @brief Resizable array implementation for C.
 * @version 0.1
 * @date 2022-04-03
 * @copyright Copyright (c) 2022 Vasilis Mylonas
 *
 * Here the term vector is used to refer to a resizable array for consistency with C++, Rust and
 * other programming languages.
 *
 * To declare a vector of type T just declare a normal pointer like so:
 *
 * @code
 * T* example_vector;
 * @endcode
 *
 * Although a vector acts like a normal pointer, it is NOT a normal pointer. To be usable, a vector
 * needs to be initialized via a call to vector_create and properly discarded using vector_destroy.
 * DO NOT pass this pointer to free or realloc. Accessing the vector's elements with [] is fine as
 * long as it is within the vector's bounds.
 *
 * Example usage:
 *
 * @code
 * int* numbers;
 * vector_create(&numbers, 0);
 *
 * vector_push(&numbers, 1);
 * vector_push(&numbers, 2);
 * vector_push(&numbers, 3);
 * vector_push(&numbers, 4);
 *
 * for (size_t i = 0; i < vector_size(&numbers); i++)
 * {
 *     printf("%i ", numbers[i]);
 * }
 *
 * vector_destroy(&numbers);
 * @endcode
 *
 * A vector is not pinned in memory and may reallocate occasionally. As such it is not advised to
 * keep pointers to a vector's elements.
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * The default value for a vector's capacity.
 */
#define VECTOR_DEFAULT_CAP 8

/**
 * The value returned when an element is not found.
 */
#define VECTOR_NOT_FOUND ((size_t)-1)

/**
 * Initializes a vector.
 *
 * @param self The address of the vector.
 * @param capacity The vector's desired initial capacity. A value of 0 is the same as
 *                 VECTOR_DEFAULT_CAP.
 * @return 0 on success, ENOMEM on allocation failure.
 */
#define vector_create(self, capacity) _vector_create((void**)self, sizeof(**(self)), capacity)

/**
 * Destroys a vector.
 *
 * @param self The address of the vector.
 */
void vector_destroy(void* self);

/**
 * Increases a vector's capacity - size value.
 *
 * @param self The address of the vector.
 * @param count The number of elements for which to ensure capacity.
 * @return 0 on success, ENOMEM on allocation failure.
 */
int vector_reserve(void* self, size_t count);

/**
 * Decreases a vector's capacity to match its size.
 *
 * @param self The address of the vector.
 * @return 0 on success, ENOMEM on allocation failure.
 */
int vector_pack(void* self);

/**
 * Returns a vector's size (in elements).
 *
 * @param self The address of the vector.
 * @return The vector's size.
 */
size_t vector_size(void* self);

/**
 * Returns a vector's capacity.
 *
 * @param self The address of the vector.
 * @return The vector's capacity.
 */
size_t vector_cap(void* self);

/**
 * Changes a vector's size to 0.
 *
 * @param self The address of the vector.
 */
void vector_clear(void* self);

/**
 * Initializes a new vector with the same contents as another.
 *
 * @param self The address of the original vector.
 * @param new The address of the new vector.
 * @return 0 on success, ENOMEM on allocation failure.
 */
int vector_dup(void* self, void* new);

/**
 * Appends an array of elements to the end of a vector, increasing its size.
 *
 * @param self The address of the vector.
 * @param count The number of elements to append.
 * @param array The array.
 * @return 0 on success, ENOMEM on allocation failure.
 */
int vector_cat(void* self, size_t count, const void* array);

/**
 * Appends an element to the end of a vector, increasing its size.
 *
 * @param self The address of the vector.
 * @param value A pointer to the element to append.
 * @return 0 on success, ENOMEM on allocation failure.
 */
int vector_push(void* self, const void* value);

/**
 * Removes and returns an element from the end of a vector, decreasing its size.
 *
 * @param self The address of the vector.
 * @return The removed element.
 *
 * @note This macro evaluates self more than once.
 */
#define vector_pop(self)                                                                           \
    (_VECTOR_HEADER(self)->size == 0 ? abort(), 0 : (*(self))[--_VECTOR_HEADER(self)->size])

/**
 * Reverses the order of elements in a vector.
 *
 * @param self The address of the vector.
 */
void vector_reverse(void* self);

/**
 * Fills a vector with the specified value.
 *
 * @param self The address of the vector.
 * @param value A pointer to the value.
 */
void vector_fill(void* self, const void* value);

/**
 * Performs a left rotation on a vector.
 *
 * @param self The address of the vector.
 */
void vector_rotl(void* self);

/**
 * Performs a right rotation on a vector.
 *
 * @param self The address of the vector.
 */
void vector_rotr(void* self);

/**
 * Sorts the elements of a vector.
 *
 * @param self The address of the vector.
 * @param cmp_func A user supplied comparison function.
 */
void vector_sort(void* self, int (*cmp_func)(const void*, const void*));

/**
 * Searches a vector for the first element equal to value and returns its index.
 *
 * @param self The address of the vector.
 * @param value A pointer to the value to search for.
 * @param cmp_func A user supplied comparison function.
 * @return The element index, or VECTOR_NOT_FOUND if not found.
 */
size_t vector_find(void* self, const void* value, int (*cmp_func)(const void*, const void*));

/**
 * Searches a vector for the last element equal to value and returns its index.
 *
 * @param self The address of the vector.
 * @param value A pointer to the value to search for.
 * @param cmp_func A user supplied comparison function.
 * @return The element index, or VECTOR_NOT_FOUND if not found.
 */
size_t vector_rfind(void* self, const void* value, int (*cmp_func)(const void*, const void*));

/**
 * @brief Performs a binary search to a vector.
 *
 * The vector must be sorted.
 *
 * @param self The address of the vector.
 * @param value The value to search for.
 * @param cmp_func A user supplied comparison function.
 * @return The element index, or VECTOR_NOT_FOUND if not found.
 */
size_t vector_bsearch(void* self, const void* value, int (*cmp_func)(const void*, const void*));

/**
 * Compares two vectors for equality.
 *
 * Two vectors are equal if and only if they are of the same size and their elements compare equal.
 *
 * @param self The address of the vector.
 * @param other The address of the vector to compare to.
 * @param cmp_func A user supplied comparison function.
 * @return true if the vectors are equal, otherwise false.
 */
bool vector_eq(void* self, void* other, int (*cmp_func)(const void* a, const void* b));

#ifndef DOXYGEN
struct _vector_header
{
    size_t unused;
    size_t size;
    size_t cap;
    size_t elem_size;
};
#define _VECTOR_HEADER(self) ((*(struct _vector_header**)(self)) - 1)
int _vector_create(void** self, size_t elem_size, size_t capacity);
#endif

#endif // VECTOR_H
