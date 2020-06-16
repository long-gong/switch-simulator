#include "schedule.h"
/*! 
    This function implements Serena
*/
void Delayed_Serena()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef DELAYED_SERENA_U
   int i, j, k, pre_i, c;
   int *match = (int *)malloc(N * sizeof(int)); /*! the array format matching */
   int *tmp_match = (int *)malloc(N * sizeof(int)); 
   int *output_match = (int *)malloc(N * sizeof(int)); /*! the array format matching */

   ARRAY_INIT(N,match,-1); /*! mark all input as unmatched */
   ARRAY_INIT(N,output_match,-1); /*! mark all output as unmatched */

   /*! Step 1: greedily match in-out pairs which have arrival */
   for (i = 0;i < N;++ i)
   {
     k = A[i]; 
     if (k != -1 && k < N)
     {/*! arrival from i to k */
        if (output_match[k] == -1) 
            output_match[k] = i;
        else
        {/*! multiple inputs want to go */
            pre_i = output_match[k];
            if (B[i][k].count > B[pre_i][k].count)
            {/*! current match is `better` */
                output_match[k] = i;
            }
        }
     }
   }

   for (k = 0;k < N;++ k)
   {
     i = output_match[k];
     if (i != -1)
        match[i] = k;
   }

   c = CIRCLE_COUNTER % DELAY;
   if (CIRCLE_COUNTER < DELAY)
      ARRAY_COPY(N,match,tmp_match);
   else
       ARRAY_COPY(N,SS[c],tmp_match);
   
if (CIRCLE_COUNTER <= DELAY)
   fix_matching(S,N);

   fix_matching(match,N);/*! round-robin match unmatched in-out pairs */


   int* merged_match = merge(S,match);
   ARRAY_COPY(N,merged_match,SS[c]);

   ARRAY_COPY(N,tmp_match,S);

   free(match);
   free(output_match);
   free(merged_match);
   free(tmp_match);

   ++ CIRCLE_COUNTER;
#endif
}

/*! 
   QPS + Serena: QPS runs rQPS rounds in each circle
*/
void Delayed_QPS_Serena()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
#ifdef DELAYED_QPS_SERENA_U
    int t,i,c;
    bool is_input_matched[P];
    int output_matched[P];
    int qps_match[P];
    int tmp_match[P];

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


    QPS_ONE_ROUND(N,Packets,B,qps_match,is_input_matched,output_matched);
    assert(is_partial_matching(qps_match,N));

    c = CIRCLE_COUNTER % DELAY;

    if (CIRCLE_COUNTER < DELAY)
      ARRAY_COPY(N,qps_match,tmp_match);
    else
      ARRAY_COPY(N,SS[c],tmp_match);

    if (CIRCLE_COUNTER <= DELAY)
      fix_matching(S,N);
    fix_matching(qps_match,N); /* make it as full match */
    assert(is_matching(qps_match,N));
    int* merged_match = merge(S,qps_match);
    ARRAY_COPY(N,merged_match,SS[c]);

    ARRAY_COPY(N,tmp_match,S);
    REMOVE_DEPART_PACKETS(N,S,B,Packets); /*! update packet vector  */ 

    free(merged_match);
    ++ CIRCLE_COUNTER;
#ifdef DEBUG
    FUNCTION_SUCCESS_PRINT;
#endif
#endif
}