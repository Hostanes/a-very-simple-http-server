
#ifndef DYNARRAY_H
#define DYNARRAY_H

#define RESIZE_CONSTANT 1.5

#include <stddef.h>

struct dynArray_t {
  size_t capacity; // Maximum number of elements the array can hold
  size_t ptr;      // Current number of elements in the array
  void **data;     // Pointer to the array of void pointers
};

// Function prototypes

struct dynArray_t *create_Array(size_t capacity);

struct dynArray_t *resize_Array(struct dynArray_t *p_varray);

int append(struct dynArray_t **varray, void *element);

void *get(struct dynArray_t *varray, size_t index);

int delete_at(struct dynArray_t *varray, size_t index);

void destroy_Array(struct dynArray_t *varray);


#endif // DYNARRAY_H
