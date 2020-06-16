#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /*! for memset */
#include <assert.h>

typedef struct vector_ {
    void** data; /*! data field */
    int size; /*! # of elements */
    int capacity;/*! capacity */
} vector;


void vector_init(vector*);
void vector_init_with_capacity(vector *,int);
int vector_capacity(vector*);
int vector_size(vector *);
void vector_push_back(vector*, void*);
void vector_pop_back(vector *);
void vector_set(vector*, int, void*);
void *vector_get(vector*, int);
void vector_delete(vector*, int);
void vector_free(vector*);
void vector_clear(vector*);
void *vector_back(vector*);
void *vector_front(vector*);

#endif