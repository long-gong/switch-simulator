#include "vector.h"

/*! 
    Note that this implementation of C vector is modified from 
    https://gist.github.com/EmilHernvall/953968
*/

#define DEF_CAPACITY 128
#define MIN_CAPACITY 8

/*! initialize a C vector with customized capacity */
void vector_init_with_capacity(vector *v,int capacity)
{
    assert(capacity > 0);
    v->size = 0;
    v->capacity = capacity;
    v->data = malloc(sizeof(void *) * v->capacity);
    memset(v->data, '\0', sizeof(void*) * v->capacity);
}

/*! initialize a C vector with default capacity */
void vector_init(vector *v)
{
   vector_init_with_capacity(v,DEF_CAPACITY);  
}

/*! function to get the capacity of the vector */
int vector_capacity(vector *v)
{
    assert(v != NULL);
    return v->capacity;
}

/*! function to get the size of the vector */
int vector_size(vector *v)
{
    assert(v != NULL);
    return v->size;
}

/*! function to add a new element to the end of the vector */
void vector_push_back(vector *v, void *e)
{
    assert(v != NULL && e != NULL);
    /*! check whether the vector is full or not */
    if (v->size == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(void*) * v->capacity);
        assert(v->data != NULL); /*! stop here if realloc fails */
    }

    v->data[v->size] = e;
    v->size ++;
}

/*! function to change an existing element */
void vector_set(vector *v, int index, void *e)
{
    assert(v != NULL && e != NULL);
    if (index < 0 || index >= v->size) {
        /*! invalid index */
        return;
    }

    v->data[index] = e;
}

/*! function to get the value of an existing element */
void *vector_get(vector *v, int index)
{
    assert(v != NULL);
    if (index >= v->size) {
        /*! invalid index */
        return NULL;
    }

    return v->data[index];
}

/*! function to remove an element 
    Note that, the implementation of this delete function 
    is different from standard one. In this implementation,
    we want to achieve O(1) deletion, we cannot remove the 
    element, and then move all the elements after it one before.
    By doing this, we sacrifice the original order of these elements 
*/
void vector_delete(vector *v, int index)
{
    assert(v != NULL);
    if (index < 0 || index >= v->size) {
        /*! invalid index */
        return;
    }

    if (index < v->size - 1) /*! the element is not the last one */
      v->data[index] = v->data[v->size - 1];
    
    v->size --;

    /*! if the occupation rate is too small, then we resize the vector */
    if (v->size == v->capacity / 4 && v->capacity > MIN_CAPACITY) 
    {
        v->capacity /= 2;
        v->data = realloc(v->data, sizeof(void*) * v->capacity);

        assert(v->data != NULL);       
    }   
}

/*! function to remove the last element */
void vector_pop_back(vector *v)
{
     vector_delete(v,v->size - 1);
}

/* get the first element */
void *vector_front(vector *v)
{
    return vector_get(v,0);
}

/* get the last element */
void *vector_back(vector *v)
{
    return vector_get(v,vector_size(v) - 1);
}

/*! delete all elements */
void vector_clear(vector *v)
{
    if (v->size == 0) return; /*! empty */
    v->size = 0;
    v->capacity = MIN_CAPACITY;
    v->data = realloc(v->data,sizeof(void*) * v->capacity);
}
/*! release the memory */
void vector_free(vector *v)
{
    if (v != NULL)
        free(v->data);
}

