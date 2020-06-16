#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h> /*! for bool type */


#include "common.h" /*! for random_int */

typedef struct CPair {
	int index;
	int value;
} CPair;


/*! initialize all the elements of a to value */
#define ARRAY_INIT(len,a,value)                                \
do{                                                            \
        int _k;                                                \
        for (_k = 0;_k < len;++ _k)                            \
        	a[_k] = value;                                     \
}while(0)

/*! copy len element from a to b */
#define ARRAY_COPY(len,src,dest)                                \
do{                                                            \
        int _k;                                                \
        for (_k = 0;_k < len;++ _k)                            \
        	dest[_k] = src[_k];                                 \
}while(0)


/*! print out an array with each line place n elements*/
#define ARRAY_PRINT(len,a,fmt,n)                                \
do{                                                            \
        int _k;                                                \
        for (_k = 0;_k < len;++ _k){                            \
            printf(fmt,a[_k]);                                   \
            if ((_k + 1) % n == 0) printf("\n");                 \
        }                                                       \
        printf("\n");                                           \
}while(0)

#define ARRAY_PRINT_W_INDEX(len,a,fmt)                                \
do{                                                            \
        int _k;                                                \
        for (_k = 0;_k < len;++ _k){                            \
            printf(fmt,_k,a[_k]);                                   \
        }                                                       \
        printf("\n");                                           \
}while(0)

#define FUNCTION_CALL_PRINT                                    \
        printf("%s is called ...\n",__func__)

#define FUNCTION_SUCCESS_PRINT                                    \
        printf("%s succeeded ...\n",__func__)

#define PACKET_PRINT(pkt)                                     \
        printf("src = %3d, dest = %3d\n",pkt->src,pkt->dest)

 



/*! declarations for functions */
int *rpermute(int n);
void rpermute_s(int n,int *a);
void printarray(int n, int a[n]);
int log_ceil(int);
bool is_matching(int *match,int len);
bool is_partial_matching(int *match,int len);
void fix_matching(int *match,int len);
int compare_cpairs(CPair *cp1,CPair *cp2);
int *random_int_array(int N,int len);

/*! functions on queue length */
double nself(int n);
double nlogn(int);
double nsquare(int);
double logn(int);
double nsqrt(int);
double ncube(int n);
double nfourth(int n);
double dummyfun(int n);

#endif
