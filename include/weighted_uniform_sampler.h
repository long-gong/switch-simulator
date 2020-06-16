/*
   logN weighted uniform sampler based balanced binary tree
*/

#ifndef WEIGHTED_UNIFORM_SAMPLER_H
#define WEIGHTED_UNIFORM_SAMPLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /*! for memset */
#include <assert.h>

#define EPS_DOUBLE 1e-8

typedef double (*weight_func)(int);
typedef struct {
	int x;
	double total_weight; /* total weight of the subtree rooted @me, i.e., sum()*/
} tree_node;

typedef struct {
    tree_node** data; /*! data field */
    int size; /*! # of elements */
    weight_func f;
} avl_t;

typedef enum
{
	INCREASE, DECREASE
} update_type;

#define GET_ROOT(tree)  tree->data[0]
#define TOTAL_WEIGHT(tree) tree->data[0]->total_weight
#define SELF_WEIGHT(tree,i) tree->f(tree->data[i]->x)
#define GET_WEIGHT(tree,i) (i<tree->size?tree->data[i]->total_weight:0)
#define SELF_INCREASE(tree,i) (++tree->data[i]->x)
#define SELF_DECREASE(tree,i) (--tree->data[i]->x)

double weight_function_def(int);
void tree_create(avl_t *,int,weight_func);
int tree_probe(avl_t *,double);
void tree_update(avl_t *,int,update_type);
void tree_update_enhanced(avl_t *tree,int index,int delta_x);
void tree_disable_node(avl_t *tree,int index);
void tree_destroy(avl_t *);
void tree_print(avl_t *);
void explore(avl_t *,int,int *,int *);
int* tree_traverse(avl_t *);

#endif
