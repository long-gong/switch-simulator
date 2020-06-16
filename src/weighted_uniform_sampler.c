#include "weighted_uniform_sampler.h"
#include "util.h"

double weight_function_def(int a)
{
	return (double)a;
}

void tree_create(avl_t *tree,int _size,weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
    printf("	tree size = %4d\n",_size);
#endif
	assert(_size > 0);
	int i;
	tree->size = _size;
	tree->data = (tree_node **) malloc(sizeof(tree_node *) * tree->size);

	for (i = 0;i < tree->size;++ i)
	{
		tree->data[i] = (tree_node *) malloc(sizeof(tree_node));
		tree->data[i]->x = 0;
		tree->data[i]->total_weight = 0.0;
	}
	if (f == NULL)
		tree->f = weight_function_def;
	else
		tree->f = f;
}

int tree_probe(avl_t *tree,double rn)
{
	assert(rn >= 0 && rn <= 1.0);
	double total_weight = TOTAL_WEIGHT(tree);
	if (total_weight < EPS_DOUBLE) return -1;

	rn *= total_weight;

	int i,left,right;

	for (i = 0;i < tree->size;)
	{
		left = 2 * i + 1;
		right = left + 1;
		if (left >= tree->size) break;
		if (rn >= GET_WEIGHT(tree,left) && rn < GET_WEIGHT(tree,left) + SELF_WEIGHT(tree,i))
			return i;
		else if (rn < GET_WEIGHT(tree,left))
			i = left;
		else 
		{
			if (right >= tree->size) break;
			rn -= GET_WEIGHT(tree,left) + SELF_WEIGHT(tree,i);
			i = right;		
		}
	}

	return i;
}

int* tree_traverse(avl_t *tree)
{
	int i, left, right, count;
	int *in_order = (int *)malloc(sizeof(int) * tree->size);
	

	i = 0;
	left = 2 * i + 1;
	right = left + 1;
	count = 0;

	if (left < tree->size) explore(tree,left,in_order,&count);
	in_order[count] = i;
	++ count;
	if (right < tree->size) explore(tree,right,in_order,&count);

	return in_order;

}

void explore(avl_t *tree,int current,int *in_order,int* count)
{
	int left, right;
	left = 2 * current + 1;
	right = left + 1;
	if (left < tree->size) explore(tree,left,in_order,count);
	in_order[*count] = current;
	++ *count;
	if (right < tree->size) explore(tree,right,in_order,count);	
}

void tree_update(avl_t *tree,int index,update_type ut)
{
#ifdef DEBUG
	printf("index: %d\nsize: %d\n",index,tree->size);
#endif
	assert(index >= 0 && index < tree->size);

    int i, parent;
	double new_weight, delta_weight, previous_weight = SELF_WEIGHT(tree,index);

	if (ut == INCREASE) SELF_INCREASE(tree,index);
	else SELF_DECREASE(tree,index);

	assert(tree->data[index]->x >= 0);

	new_weight = SELF_WEIGHT(tree,index);

	delta_weight = new_weight - previous_weight;

	i = index;

	while (i >= 0)
	{
		tree->data[i]->total_weight += delta_weight;
		i = (i + 1) / 2 - 1;
	}
}

void tree_update_enhanced(avl_t *tree,int index,int delta_x)
{
#ifdef DEBUG
	printf("index: %d\nsize: %d\n",index,tree->size);
#endif
	assert(index >= 0 && index < tree->size);

    int i, parent;
	double new_weight, delta_weight, previous_weight = SELF_WEIGHT(tree,index);

	tree->data[index]->x += delta_x;
	assert(tree->data[index]->x >= 0);

	new_weight = SELF_WEIGHT(tree,index);

	delta_weight = new_weight - previous_weight;

	i = index;

	while (i >= 0)
	{
		tree->data[i]->total_weight += delta_weight;
		i = (i + 1) / 2 - 1;
	}
}

void tree_disable_node(avl_t *tree,int index)
{
#ifdef DEBUG
	printf("index: %d\nsize: %d\n",index,tree->size);
#endif
	assert(index >= 0 && index < tree->size);

    int i, parent;
	double new_weight, delta_weight, previous_weight = SELF_WEIGHT(tree,index);

	tree->data[index]->x = 0;

	new_weight = SELF_WEIGHT(tree,index);

	delta_weight = new_weight - previous_weight;

	i = index;

	while (i >= 0)
	{
		tree->data[i]->total_weight += delta_weight;
		i = (i + 1) / 2 - 1;
	}
}

void tree_print(avl_t *tree)
{
	int i;
	for (i = 0;i < tree->size;++ i)
	{
		printf("%4d:(%4d,%10.2f)\n",i,tree->data[i]->x,tree->data[i]->total_weight);
	}
}

void tree_destroy(avl_t *tree)
{
	int i;
	if (tree != NULL)
	{
		for (i = 0;i < tree->size;++ i)
			free(tree->data[i]);
		free(tree->data);
	}
}