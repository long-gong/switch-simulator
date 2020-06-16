#include "schedule.h"
/*! 
   QPS + Serena: QPS runs rQPS rounds in each circle
*/
void QPS_Serena(int rQPS)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


    if (!packet_vector_initialized)
    {
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    for (t = 0;t < rQPS;++ t)
        QPS_ONE_ROUND(N,Packets,B,qps_match,is_input_matched,output_matched);

    assert(is_partial_matching(qps_match,N));
    fix_matching(qps_match,N); /* make it as full match */

    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    free(merged_match);
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
#endif
}

/*!
    QPS + Arrival + Serena
*/
void QPS_Arrival_Serena()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_ARRIVAL_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


    if (!packet_vector_initialized)
    {
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    /* QPS phase */
    QPS_ONE_ROUND(N,Packets,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    fix_matching(qps_match,N); /* make it as full match */
    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    free(merged_match);
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif    
}

/*!
    Arrival + QPS + Serena
*/
void Arrival_QPS_Serena()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef ARRIVAL_QPS_SERENA_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,qps_match,-1); /* set as unmatched */


    if (!packet_vector_initialized)
    {
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    /* QPS phase */
    QPS_ONE_ROUND(N,Packets,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    fix_matching(qps_match,N); /* make it as full match */
    assert(is_matching(qps_match,N));

    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,S);

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    free(merged_match);
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif     
}

/*!
*/
void QPS_Arrival_ALT_Serena(int schedule_opt)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_ARRIVAL_ALT_SERENA_U
    if (schedule_opt % 2 == 0)
        QPS_Serena(1);
    else
    {
       INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */
       Serena();
       REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */
    }
#endif
}
/*! 
   QPS + RRM: QPS runs rQPS rounds in each circle
*/
void QPS_RRM(int rQPS)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_RRM_U
    int t,i;
    bool is_input_matched[P];
    int output_matched[P];

    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,S,-1); /* set as unmatched */

    if (!packet_vector_initialized)
    {
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    for (t = 0;t < rQPS;++ t)
        QPS_ONE_ROUND(N,Packets,B,S,is_input_matched,output_matched);

    assert(is_partial_matching(S,N));
    fix_matching(S,N); /* make it as full match with RRM */

    assert(is_matching(S,N));

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
#endif
}

/*!
   QPS + iSLIP: QPS rQPS rounds and iSLIP logN rounds
*/
void QPS_iSLIP(int rQPS)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_ISLIP_U
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

    if (!packet_vector_initialized){/* if first circle */
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    /*! QPS phase */
    for (t = 0;t < rQPS;++ t)
        QPS_ONE_ROUND(N,Packets,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif
}

/*! QPS + Arrival + iSLIP */
void QPS_Arrival_iSLIP()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_ARRIVAL_ISLIP_U
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

    if (!packet_vector_initialized){/* if first circle */
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    /* QPS phase */
    QPS_ONE_ROUND(N,Packets,B,S,is_input_matched,output_matched);

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }
    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif
}


/*! Arrival + QPS + iSLIP */
void Arrival_QPS_iSLIP()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef ARRIVAL_QPS_ISLIP_U
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

    if (!packet_vector_initialized){/* if first circle */
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    /* Arrival matching phase */
    ARRIVAL_MATCHING(N,A,B,S,is_input_matched,output_matched);

    /* QPS phase */
    QPS_ONE_ROUND(N,Packets,B,S,is_input_matched,output_matched);

    /*! iSLIP phase */
    for (t = 0;t < log_ceil(N);++ t)
    {
        is_first_round = (t == 0);
        iSLIP_ONE_ROUND(N,B,S,is_input_matched,output_matched,check,Accept,Grant,is_first_round);
    }
    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif    
}

/*!
   QPS + Arrival + iSLIP
*/
void QPS_Arrival_ALT_iSLIP(int schedule_opt)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_ARRIVAL_ALT_ISLIP_U
    if (schedule_opt % 2 == 0)
        QPS_iSLIP(1);
    else
        {
             INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */
             Arrival_iSLIP(); 
             REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 
        }
#endif
}

/*! variable-round QPS */
void QPS_VR(int rounds)
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef QPS_VR_U
    int i,t;
    bool is_input_matched[P];
    int output_matched[P];


    ARRAY_INIT(N,is_input_matched,false);
    ARRAY_INIT(N,output_matched,-1);
    ARRAY_INIT(N,S,-1);

    if (!packet_vector_initialized)
    {
        packet_vector_initialized = true;
        for (i = 0;i < N;++ i)
            vector_init(&Packets[i]); /* initialize packet vector for input port i */
    }

    INSTALL_NEW_PACKETS(N,A,B,Packets)   ; /*! update packet vector  */

    for (t = 0;t < rounds;++ t)
        QPS_ONE_ROUND(N,Packets,B,S,is_input_matched,output_matched);  

    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    /* comment the following line */
    assert(is_partial_matching(S,N));
#endif
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
}

void QPS_PIM(int rQPS)
{
    /* TODO */
}


/*!
   QPS + iLQF: QPS 1 round and iLQF logN rounds
*/
void QPS_iLQF()
{
  /* TODO */
}