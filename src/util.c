#include "util.h"

/*!
   The following two functions are copied from 
   http://cis-linux1.temple.edu/~giorgio/cis71/code/randompermute.c
*/
/*! It returns a random permutation of 0..n-1 */
int * rpermute(int n) {
    int *a = malloc(n*sizeof(int));
    int k;
    for (k = 0; k < n; k++)
	a[k] = k;
    for (k = n-1; k > 0; k--) {
	int j = rand() % (k+1);
	int temp = a[j];
	a[j] = a[k];
	a[k] = temp;
    }
    return a;
}

/*! It returns a random permutation of 0..n-1 */
void rpermute_s(int n,int *a) {
    int k;
    for (k = 0; k < n; k++)
    a[k] = k;
    for (k = n-1; k > 0; k--) {
    int j = rand() % (k+1);
    int temp = a[j];
    a[j] = a[k];
    a[k] = temp;
    }
}
/*! Print a 8 elements per line */
void printarray(int n, int a[n]) {
    int k = 0;
    for (k = 0; k < n; k++) {
	printf("%6d   ", a[k]);
	if (k % 8 == 7)
	    printf("\n");
    } 
}


int log_ceil(int N)
{
    double m, k, h;
    int r;

    m = (double) N;
    k = log2(m);
    h = ceil(k);
    r = (int) h;

    return r;
}

/*! 
   This function checks whether a claimed matching is really a matching.
*/
bool is_matching(int *match,int len)
{
    bool *visited = (bool *)malloc(len * sizeof(bool));
    bool result = true;

    int i = 0;
    for(i = 0;i < len;++ i)
        visited[i] = false;

    for(i = 0;i < len;++ i)
        if ( match[i] >= 0 && match[i] < len)
            visited[match[i]] = true;
        else {
            result = false;
            break;
            }

    for (i = 0;i < len;++ i)
        if (!visited[i]) 
        {
            result = false;
            break;
        }
    
    free(visited);
    return result;
}

bool is_partial_matching(int *match,int len)
{
    bool *visited = (bool *)malloc(len * sizeof(bool));
    bool result = true;

    int i, k;
    for(i = 0;i < len;++ i)
        visited[i] = false;

    for(i = 0;i < len;++ i)
        if (match[i] >= 0 && match[i] < len)
        {
            k = match[i];
            if (!visited[k]) visited[k] = true;
            else 
            {
                result = false; /* multiple inputs to one output */
                break;
            }
        }
    
    free(visited);        
    return result;    
}

/*! 
   This function matches all the unmatched (marked as -1) nodes
*/
void fix_matching(int *match,int len)
{
    bool *is_matched = (bool *)malloc(len * sizeof(bool));
    int *unmatched_in = (int *)malloc(len * sizeof(int));
    int *unmatched_out = (int *)malloc(len * sizeof(int));

    int i, j, k;
    int unmatched_counter = 0;
    
    
    for (i = 0;i < len;++ i) is_matched[i] = false; /*! mark all outputs as unmatched */
    
    /*! find all unmatched inputs & mark all matched inputs */
    for (i = 0;i < len;++ i)
        if (match[i] != -1)
            is_matched[match[i]] = true; /*! mark matched output */
        else
        {
            unmatched_in[unmatched_counter] = i; /*! record unmatched inputs */
            ++ unmatched_counter;
        }

    /*! find all unmatched outputs */
    unmatched_counter = 0;
    for (k = 0;k < len;++ k)
        if (!is_matched[k])
        {
            unmatched_out[unmatched_counter] = k;
            ++ unmatched_counter;
        }
    
    /*! matched all the unmatched inputs and outputs */
    for (j = 0;j < unmatched_counter;++ j)
    {
        i = unmatched_in[j];
        k = unmatched_out[j];
        match[i] = k;
    }

    free(unmatched_out);
    free(unmatched_in);
    free(is_matched);
   
}


/*!
   This function implements the comparison function for CPair 
*/
int compare_cpairs(CPair *cp1,CPair *cp2)
{
    return (cp1->value - cp2->value); 
}

/*! 
   This function implements the function to generate multiple random integers
*/

int *random_int_array(int N,int len)
{
    int i;
    int *arr = (int *)malloc(len * sizeof(int));
    for (i = 0;i < len;++ i)
        arr[i] = random_int(N);

    return arr;
}
/* f(x) = x */
double nself(int n){
    return (double)n;
}
/*! q functions */
double nlogn(int n)
{
    if (n == 0) return 0;
    double x = (double)n;
    return x * log2(x + 1.0);

}
double nsquare(int n)
{
    double x = (double) n;
    return x * x;
}
double logn(int n)
{
    if (n == 0) return 0;
    double x = (double)n;
    return log2(x + 1);
}
double nsqrt(int n)
{
    return sqrt(n);
}
double ncube(int n)
{
    double x = (double) n;
    return pow(x,3.0);
}
double nfourth(int n)
{
    double x = (double) n;
    return pow(x,4.0);
}
double dummyfun(int n)
{
    return 0.0;
}

