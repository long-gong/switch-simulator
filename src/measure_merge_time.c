#ifdef EXPERIMENT_FOUR
#include "measure_merge_time.h"

int test_correctness()
{
	int i,j,N = 32;
	int MAX_WEIGHT = 1e4;
	int TEST_NUM = 1e4;

	clock_t t;
	double time_taken_gen_random, time_taken_total;
	srand(time(0));
    
    printf("test random generator ...\n");

    printf("	test 1.1: random matrix test ...\n");

    for (i = 0;i < TEST_NUM;++ i)
    {
    	RANDOM_MATRIX(MAX_WEIGHT,N,N,WEIGHT);
    	if ((i + 1) % 1000 == 0)
    		printf("%4d/%4d finished\n",i + 1,TEST_NUM);
    }

    printf("	test 1.2: random permutation test ...\n");
    for (i = 0;i < TEST_NUM;++ i)
    {
    	RANDOM_MATRIX(MAX_WEIGHT,N,N,WEIGHT);
    	int *ma = rpermute(N);
    	int *mb = rpermute(N);

    	ARRAY_COPY(N,ma,MATCHX);
    	ARRAY_COPY(N,mb,MATCHY);

    	free(ma);
    	free(mb);

    	assert(is_matching(MATCHX,N));
    	assert(is_matching(MATCHY,N));

    	if ((i + 1) % 1000 == 0)
    		printf("%4d/%4d finished\n",i + 1,TEST_NUM);
    }
    printf("	passed\n");
    printf("	test 2: merge test ...\n");
    for (i = 0;i < TEST_NUM;++ i)
    {
    	RANDOM_MATRIX(MAX_WEIGHT,N,N,WEIGHT);
    	int *ma = rpermute(N);
    	int *mb = rpermute(N);
    	ARRAY_COPY(N,ma,MATCHX);
    	ARRAY_COPY(N,mb,MATCHY);
    	free(ma);
    	free(mb);

    	assert(is_matching(MATCHX,N));
    	assert(is_matching(MATCHY,N));

    	MERGE(N,WEIGHT,MATCHX,MATCHY,MATCHZ);
    	assert(is_matching(MATCHZ,N));
    	if ((i + 1) % 1000 == 0)
    		printf("%4d/%4d finished\n",i + 1,TEST_NUM);
    }
    printf("	passed\n");
    return 0;		
}

int merge_time_test()
{
	srand(time(0));
	int i,j,N;
	int MAX_WEIGHT = 1e4;
	int TEST_NUM = 1e6;
	clock_t t;
	double time_taken_gen_random, time_taken_total;
	printf("%-8s | %-12s\n","N","time (micro seconds)");
	for (N = 4;N <= MAX_N;N *= 2)
	{
	    t = clock();
	    for (i = 0;i < TEST_NUM;++ i)
	    {
	    	RANDOM_MATRIX(MAX_WEIGHT,N,N,WEIGHT);
	    	int *ma = rpermute(N);
	    	int *mb = rpermute(N);
	    	ARRAY_COPY(N,ma,MATCHX);
	    	ARRAY_COPY(N,mb,MATCHY);
	    	ARRAY_INIT(N,MATCHZ,-1);
	    	free(ma);
	    	free(mb);
	    }
	    t = clock() - t;
	    time_taken_gen_random = ((double)t)/CLOCKS_PER_SEC; 
	    t = clock();
	    for (i = 0;i < TEST_NUM;++ i)
	    {
	    	RANDOM_MATRIX(MAX_WEIGHT,N,N,WEIGHT);
	    	int *ma = rpermute(N);
	    	int *mb = rpermute(N);
	    	ARRAY_COPY(N,ma,MATCHX);
	    	ARRAY_COPY(N,mb,MATCHY);
	    	free(ma);
	    	free(mb);
	    	assert(is_matching(MATCHX,N));
	    	assert(is_matching(MATCHY,N));
	    	ARRAY_INIT(N,MATCHZ,-1);
	    	MERGE(N,WEIGHT,MATCHX,MATCHY,MATCHZ);
	    	assert(is_matching(MATCHZ,N));
	    }
	    t = clock() - t;
	    time_taken_total = ((double)t)/CLOCKS_PER_SEC;
	    printf("%8d %.2f\n",N,time_taken_total - time_taken_gen_random);
	}
	return 0;
}
#endif