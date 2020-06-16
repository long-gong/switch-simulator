#include "schedule.h"
/*! 
   FQPS + Serena: QPS runs rQPS rounds in each circle
*/
void FQPS_Serena(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


    if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */
#ifdef DEBUG
    for (i = 0;i < N;++ i)
        tree_print(&tree[i]);
#endif

    
    FQPS_ONE_ROUND(N,tree,B,qps_match,is_input_matched,output_matched);

    assert(is_partial_matching(qps_match,N));
    fix_matching(qps_match,N); /* make it as full match */

    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    free(merged_match);
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
#endif
}

/*!
    FQPS + Arrival + Serena
*/
void FQPS_Arrival_Serena(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_ARRIVAL_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


   if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    
    

    /* FQPS phase */
    FQPS_ONE_ROUND(N,tree,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    fix_matching(qps_match,N); /* make it as full match */
    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    free(merged_match);
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif    
}

/*!
    Arrival + FQPS + Serena
*/
void Arrival_FQPS_Serena(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef ARRIVAL_FQPS_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


   if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    /* QPS phase */
    FQPS_ONE_ROUND(N,tree,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    fix_matching(qps_match,N); /* make it as full match */
    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    free(merged_match);
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif     
}

/*!
*/
void FQPS_Arrival_ALT_Serena(int schedule_opt,weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_ARRIVAL_ALT_SERENA_U
    if (schedule_opt % 2 == 0)
        FQPS_Serena(f);
    else
    {
       HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */
       Serena();
       HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 
    }
#endif
}
/*! 
   FQPS + RRM: QPS runs rQPS rounds in each circle
*/
void FQPS_RRM()
{
  /* TODO */
}

/*!
   FQPS + iSLIP: FQPS 1 rounds and iSLIP logN rounds
*/
void FQPS_iSLIP(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_ISLIP_U
    int t, i;
    bool is_first_round;
    bool is_input_matched[P];
    int output_matched[P];
    int check[P]; 

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    if (!pointers_initialized){/* if first circle */
        pointers_initialized = true;
        ARRAY_INIT(N,Grant,0);
        ARRAY_INIT(N,Accept,0);
    }
    ARRAY_INIT(N,check,-1);
    ARRAY_INIT(N,S,-1);

   if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    /*! QPS phase */
    FQPS_ONE_ROUND(N,tree,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }

    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif
}

/*! FQPS + Arrival + iSLIP */
void FQPS_Arrival_iSLIP(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_ARRIVAL_ISLIP_U
    int t, i;
    bool is_first_round;
    bool is_input_matched[P];
    int output_matched[P];
    int check[P]; 

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    if (!pointers_initialized){/* if first circle */
        pointers_initialized = true;
        ARRAY_INIT(N,Grant,0);
        ARRAY_INIT(N,Accept,0);
    }
    ARRAY_INIT(N,check,-1);
    ARRAY_INIT(N,S,-1);

    if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    /* QPS phase */
    FQPS_ONE_ROUND(N,tree,B,S,is_input_matched,output_matched);

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }
    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif
}


/*! Arrival + FQPS + iSLIP */
void Arrival_FQPS_iSLIP(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef ARRIVAL_FQPS_ISLIP_U
    int t, i;
    bool is_first_round;
    bool is_input_matched[P];
    int output_matched[P];
    int check[P]; 

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    if (!pointers_initialized){/* if first circle */
        pointers_initialized = true;
        ARRAY_INIT(N,Grant,0);
        ARRAY_INIT(N,Accept,0);
    }
    ARRAY_INIT(N,check,-1);
    ARRAY_INIT(N,S,-1);

    if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,S,is_input_matched,output_matched);

    /* QPS phase */
    FQPS_ONE_ROUND(N,tree,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }
    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif    
}

/*!
   FQPS + Arrival + iSLIP
*/
void FQPS_Arrival_ALT_iSLIP(int schedule_opt,weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_ARRIVAL_ALT_ISLIP_U
    if (schedule_opt % 2 == 0)
        FQPS_iSLIP(f);
    else
        {
             HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */
             Arrival_iSLIP(); 
             HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 
        }
#endif
}

/*! variable-round QPS */
#ifdef CUSTOMIZED_ITERATION_SUPPORT
void FQPS_VR(int customized_round, weight_func f)
#else
void FQPS_VR(weight_func f)
#endif
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_VR_U
    int i,t;
    bool is_input_matched[P];
    int output_matched[P];
#ifdef CUSTOMIZED_ITERATION_SUPPORT
    int rounds = customized_round;
#else
    int rounds = log_ceil(N);
#endif


    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,S,-1);

    if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    for (t = 0;t < rounds;++ t)
        FQPS_ONE_ROUND(N,tree,B,S,is_input_matched,output_matched);  

    HANDLE_NEW_DEPART_FQ(N,S,B,tree); /*! update voq tree  */ 

    /* comment the following line */
   // assert(is_partial_matching(S,N));
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
}

/*! variable-round QPS without replacements */
#ifdef CUSTOMIZED_ITERATION_SUPPORT
void FQPS_WOREP_VR(int customized_round, weight_func f)
#else
void FQPS_WOREP_VR(weight_func f)
#endif
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef FQPS_VR_U
    int i,t;
    bool is_input_matched[P];
    int output_matched[P];
#ifdef CUSTOMIZED_ITERATION_SUPPORT
    int rounds = customized_round;
#else
    int rounds = log_ceil(N);
#endif


    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,S,-1);

    if (!voq_tree_initialized)
    {
        voq_tree_initialized = true;
        for (i = 0;i < N;++ i)
            tree_create(&tree[i],N,f); /* initialize tree for input port i */
    }

    HANDLE_NEW_ARRIVAL_FQ(N,A,B,tree); /*! update voq tree  */

    for (t = 0;t < rounds;++ t)
        FQPS_ONE_ROUND_WOREP(N,tree,B,S,is_input_matched,output_matched);  

    HANDLE_NEW_DEPART_FQ_WOREP(N,S,B,tree); /*! update voq tree, re-enable those disabled */ 

    /* comment the following line */
   // assert(is_partial_matching(S,N));
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
}

/* Longest queue first Serena: here f is a dummy parameter, it would not be used
   
*/
void LQF_Serena(weight_func f)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef LQF_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */
  
    LQF(N,B,qps_match,is_input_matched,output_matched);

    assert(is_partial_matching(qps_match,N));
    fix_matching(qps_match,N); /* make it as full match */

    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    free(merged_match);
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
#endif
}