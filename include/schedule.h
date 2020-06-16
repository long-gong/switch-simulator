#ifndef SCHEDULE_COMMON_H
#define SCHEDULE_COMMON_H

#include <stdint.h> /*! for intptr_t */
#include <assert.h> /*! for assert */
#include <string.h> /*! for memcpy */
#include "common.h"
#include "graphtypes.h"
#include "util.h"
#include "vector.h" /* for vector */
#include "weighted_uniform_sampler.h" /* for avl_t & weight_func */


#define DEF_MIN_ITER 1
#define DEF_ITER 3
#define SINGLE 0
#define RANGE 1
#define ALTER 2
#define MIN_DELAY 1
#define MAX_DELAY 32

typedef struct {
    int type;
    int value;
} param_t;

typedef struct {
    char name[256];
    void (*func)(); /* function pointer */
    param_t *param; /* currently only support int parameter */
} func_t;

/* global variables required by all algorithms */
extern queue_t B[P][P];        /* B[i][k] VOQs at inputs */

extern int A[P];               /* k = A[i] arrival graph */
extern int arrival_matrix[P][P];       /* arrival_matrix[i][k] if arrival input i to output k */

extern int S[P];               /* k = S[i] crossbar schedule */




/* global variables required by MWM */
extern int mwm_graph;          /* flag to see if mwm graph has been created */
extern Graph graph;            /* used in MWM */
extern func_t func_list[];


#ifdef DELAY_V
extern int DELAY;
extern int SS[P][P];
extern int CIRCLE_COUNTER;
#endif

/* global variables required by iSLIP */
#ifdef ISLIP_V
extern int Accept[P];
extern int Grant[P];
extern bool pointers_initialized;
#endif

/* global variables required by QPS */
#ifdef QPS_V
extern vector Packets[P]; /* array of all packets, needed by queue proportional sampling */
extern bool packet_vector_initialized;
#endif 

#ifdef FQPS_V
extern avl_t tree[P];
extern bool voq_tree_initialized;
extern weight_func tested_wfs[];
extern int current_wfi;
extern int maximum_wfi;
extern bool fqps_turn_on;
#endif

#ifdef ENHANCED_RANDOM_U
extern param_t erp;
#endif

#if defined(QPS_PIM_U) || defined(QPS_RRM_U) || defined(QPS_ISLIP_U) || defined(QPS_SERENA_U)
extern param_t qpp;
#endif

#ifdef QPS_VR_U
extern param_t qpsp;
#endif

#ifdef ISLIP_U
extern param_t ip;
#endif


/* for convenience of combining different schedule policies */


/* new install packet procedure */
/*!
   N: port number,
   A: arrival graph, A[i] = k means a packet arrives at input port i
      and wants to go to output port k, A[i] = -1 indicates no arrival
   VOQs: virtual output queues,
   Packets: packet vector 
*/
#define INSTALL_NEW_PACKETS(N,A,VOQs,Packets)                                                             \   
do{                                                                                                       \
    int _i,_k;                                                                                            \
    for (_i = 0;_i < N;++ _i){                                                                            \
        if (A[_i] != -1){/*! has arrival */                                                               \
            _k = A[_i];                                                                                   \
            vector_push_back(&Packets[_i],&VOQs[_i][_k].tail->packet);                                       \
            VOQs[_i][_k].tail->packet.index = vector_size(&Packets[_i]) - 1;                                  \
        }                                                                                                 \
    }                                                                                                     \
}while(0)

/* remove departed packets from the packet vector */
#define REMOVE_DEPART_PACKETS(N,S,VOQs,Packets)                                                \       
do{                                                                                            \
    int _i,_k,_pIndex;                                                                         \
    packet_t *_pkt;                                                                            \
    for (_i = 0;_i < N;++ _i){                                                                 \
        _k = S[_i];                                                                            \
        if (_k != -1 && VOQs[_i][_k].count > 0){                                               \
            _pIndex = VOQs[_i][_k].head->packet.index;                                         \
            assert(_pIndex != -1);                                                             \
            if (_pIndex != vector_size(&Packets[_i]) - 1){                                     \
                _pkt = (packet_t *)vector_back(&Packets[_i]);                                  \
                _pkt->index = _pIndex;                                                         \
            }                                                                                  \
            vector_delete(&Packets[_i],_pIndex);                                               \
        }                                                                                      \
    }                                                                                          \
}while(0)


#define HANDLE_NEW_ARRIVAL_FQ(N,A,VOQs,tree)                                                             \   
do{                                                                                                       \
    int _i,_k;                                                                                            \
    for (_i = 0;_i < N;++ _i){                                                                            \
        if (A[_i] != -1){/*! has arrival */                                                               \
            _k = A[_i];                                                                                   \
            tree_update(&tree[_i],_k,INCREASE);                                     \
        }                                                                                                 \
    }                                                                                                     \
}while(0)

#define HANDLE_NEW_DEPART_FQ(N,S,VOQs,tree)                                                \       
do{                                                                                            \
    int _i,_k;                                                                         \
    for (_i = 0;_i < N;++ _i){                                                                 \
        _k = S[_i];                                                                            \
        if (_k != -1 && VOQs[_i][_k].count > 0){                                               \
            tree_update(&tree[_i],_k,DECREASE);                                       \
        }                                                                                      \
    }                                                                                          \
}while(0)

#define HANDLE_NEW_DEPART_FQ_WOREP(N,S,VOQs,tree)                                  \         
do{                                                                                \  
    int _i,_k,_j;                                                                  \     
    for (_i = 0;_i < N;++ _i){                                                     \
        for (_j = 0;_j < N;++ _j)                                                  \
        {                                                                          \
            if (tree[_i].data[_j]->x == 0)                                        \
                tree_update_enhanced(&tree[_i],_j,VOQs[_i][_j].count);             \
        }                                                                          \
        _k = S[_i];                                                                \  
        if (_k != -1 && VOQs[_i][_k].count > 0){                                   \  
            tree_update(&tree[_i],_k,DECREASE);                                    \  
        }                                                                          \  
    }                                                                              \  
}while(0)
/*! Queue Proportional Sampling */
/*! Parameter Explanations:
    N:  port number
    Packets: vectors of packets for each input port, e.g. Packets[i] 
             represents all the packets for input port i.
    VOQs: virtual output queues for each input, e.g. VOQs[i][k] 
          represents all the packets whose input is i and out is k
    is_input_matched: bool array, e.g. is_input_matched[i] = true represents
                      input i is matched
    output_matched: int array, e.g. output_matched[k] = i represents output
                    k is matched with input i
*/
#define QPS_ONE_ROUND(N,Packets,VOQs,S,is_input_matched,output_matched)                                        \
do{                                                                                                            \
    int _i,_pre_i,_k,_pi,_rsize;                                                                               \
    packet_t *_sampled_packet;                                                                                 \
    for (_i = 0;_i < N;++ _i)                                                                                  \
    {                                                                                                          \
        if (is_input_matched[_i]) continue;                                                                    \
        _rsize = vector_size(&Packets[_i]);                                                                    \
        _pi = _rsize > 0?random_int(_rsize):-1;                                                                \                                                   
        if (_pi != -1)                                                                                         \
        {                                                                                                      \
           _sampled_packet = (packet_t *)vector_get(&Packets[_i],_pi);                                         \
           assert(_sampled_packet != NULL);                                                                    \
           _k = _sampled_packet->dest;                                                                         \
           if (!(_k >=0 && _k < N)) printf("ERROR in %s\n",__func__);                                          \
           if (output_matched[_k] != -1)                                                                       \
           {                                                                                                   \
             _pre_i = output_matched[_k];                                                                      \
             if (is_input_matched[_pre_i]) continue;                                                           \
             if (VOQs[_i][_k].count > VOQs[_pre_i][_k].count)                                                  \
                output_matched[_k] = _i;                                                                       \
           }                                                                                                   \
           else                                                                                                \
           {                                                                                                   \
               output_matched[_k] = _i;                                                                        \
           }                                                                                                   \
        }                                                                                                      \       
    }                                                                                                          \
    for (_k = 0;_k < N;++ _k)                                                                                  \
    {                                                                                                          \
        _i = output_matched[_k];                                                                               \
        if (_i != -1 && !is_input_matched[_i])                                                                 \
        {                                                                                                      \ 
            is_input_matched[_i] = true;                                                                       \
            S[_i] = _k;                                                                                        \
        }                                                                                                      \
    }                                                                                                          \
}while(0)

#define FQPS_ONE_ROUND(N,tree,VOQs,S,is_input_matched,output_matched)                                        \
do{                                                                                                            \
    int _i,_pre_i,_k;                                                                               \
    double rn;                                                                                \
    for (_i = 0;_i < N;++ _i)                                                                                  \
    {                                                                                                          \
        if (is_input_matched[_i]) continue;                                                                    \
        rn = random_01();                                                                    \
        _k = tree_probe(&tree[_i],rn);                                                                \                                                   
        if (_k != -1)                                                                                         \
        {                                                                                                      \
           if (output_matched[_k] != -1)                                                                       \
           {                                                                                                   \
             _pre_i = output_matched[_k];                                                                      \
             if (is_input_matched[_pre_i]) continue;                                                           \
             if (VOQs[_i][_k].count > VOQs[_pre_i][_k].count)                                                  \
                output_matched[_k] = _i;                                                                       \
           }                                                                                                   \
           else                                                                                                \
           {                                                                                                   \
               output_matched[_k] = _i;                                                                        \
           }                                                                                                   \
        }                                                                                                      \       
    }                                                                                                          \
    for (_k = 0;_k < N;++ _k)                                                                                  \
    {                                                                                                          \
        _i = output_matched[_k];                                                                               \
        if (_i != -1 && !is_input_matched[_i])                                                                 \
        {                                                                                                      \ 
            is_input_matched[_i] = true;                                                                       \
            S[_i] = _k;                                                                                        \
        }                                                                                                      \
    }                                                                                                          \
}while(0)

/* FQPS without replacements */
#define FQPS_ONE_ROUND_WOREP(N,tree,VOQs,S,is_input_matched,output_matched) \                      
do{                                                                         \                
    int _i,_pre_i,_k;                                                       \                
    double rn;                                                              \                
    for (_i = 0;_i < N;++ _i)                                               \                
    {                                                                       \                
        if (is_input_matched[_i]) continue;                                 \                
        rn = random_01();                                                   \                
        _k = tree_probe(&tree[_i],rn);                                      \                                                          
        if (_k != -1)                                                       \                
        {                                                                   \
           tree_disable_node(&tree[_i], _k);                                \
           if (output_matched[_k] != -1)                                    \                
           {                                                                \                
             _pre_i = output_matched[_k];                                   \                
             if (is_input_matched[_pre_i]) continue;                        \                
             if (VOQs[_i][_k].count > VOQs[_pre_i][_k].count)               \                
                output_matched[_k] = _i;                                    \                
           }                                                                \                
           else                                                             \                
           {                                                                \                
               output_matched[_k] = _i;                                     \                
           }                                                                \                
        }                                                                   \                       
    }                                                                       \                
    for (_k = 0;_k < N;++ _k)                                               \                
    {                                                                       \                
        _i = output_matched[_k];                                            \                
        if (_i != -1 && !is_input_matched[_i])                              \                
        {                                                                   \                 
            is_input_matched[_i] = true;                                    \                
            S[_i] = _k;                                                     \                
        }                                                                   \                
    }                                                                       \                
}while(0)

/* longest queue first */
#define LQF(N,VOQs,S,is_input_matched,output_matched)                     \                        
do{                                                                       \                 
    int _i,_pre_i,_k,_j;                                                  \                    
    int *rand_perm = (int *) malloc(sizeof(int) * N);                                                       \
    for (_i = 0;_i < N;++ _i){                                            \                                          
        if (is_input_matched[_i]) continue; /* already matched */         \
    /* pick the longest VOQ */                                        \
         rpermute_s(N,rand_perm);                                             \
    _k = rand_perm[0];                                                \
    for (_j = 1;_j < N;++ _j)                                         \
      if (VOQs[_i][rand_perm[_j]].count > VOQs[_i][_k].count)       \
        _k = rand_perm[_j];                                       \                                                                                                                                    
     if (output_matched[_k] != -1)                                      \             
     {                                                                  \             
     _pre_i = output_matched[_k];                                     \             
     if (is_input_matched[_pre_i]) continue;                          \             
     if (VOQs[_i][_k].count > VOQs[_pre_i][_k].count)                 \             
      output_matched[_k] = _i;                                      \             
     }                                                                  \             
     else                                                               \             
     {                                                                  \             
       output_matched[_k] = _i;                                       \             
     }                                                                  \             
    }                                                                     \
    free(rand_perm);                                                    \
    for (_k = 0;_k < N;++ _k){                                            \                                          
        _i = output_matched[_k];                                          \                 
        if (_i != -1 && !is_input_matched[_i])                            \                 
        {                                                                 \                 
            is_input_matched[_i] = true;                                  \                 
            S[_i] = _k;                                                   \                 
        }                                                                 \                 
    }                                                                     \                 
}while(0)
/*! iSLIP One Round */
/*! Parameter Explanations:
    N:  port number
    VOQs: virtual output queues for each input, e.g. VOQs[i][k] 
          represents all the packets whose input is i and out is k
    is_input_matched: bool array, e.g. is_input_matched[i] = true represents
                      input i is matched
    output_matched: int array, e.g. output_matched[k] = i represents output
                    k is matched with input i
    accept: accept points
    grant: grant points
    is_first_round: bool variable, whether it is the first round
*/
#define iSLIP_ONE_ROUND(N,VOQs,S,is_input_matched,output_matched,check,accept,grant,is_first_round)           \
do{                                                                                                           \           
   int _t,_i,_k,_c,_found;                                                                                    \
    for (_k = 0; _k < N; _k++) {                                                                              \
        check[_k] = -1;                                                                                       \
        if (output_matched[_k] == -1) {                                                                       \
            for (_found = 0, _c = 0; _c < N && !_found; _c++) {                                               \
                _i = (_c + grant[_k]) % N;                                                                    \
                if (!is_input_matched[_i] && VOQs[_i][_k].count > 0) {                                           \
                    _found = 1;                                                                               \
                    check[_k] = _i;                                                                           \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
    }                                                                                                         \
    for (_i = 0; _i < N; _i++) {                                                                              \
        if (!is_input_matched[_i]) {                                                                          \
            for (_found = 0, _c = 0; _c < N && !_found; _c++) {                                               \
                _k = (_c + accept[_i]) % N;                                                                   \
                if (check[_k] == _i) {                                                                        \
                    _found = 1;                                                                               \
                    S[_i] = _k;                                                                               \
                    is_input_matched[_i] = true;                                                              \
                    output_matched[_k] = _i;                                                                  \
                    if (is_first_round){                                                                      \
                    grant[_k] = (_i + 1) % N;                                                          \
                    accept[_i] = (_k + 1) % N;}                                                       \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
    }                                                                                                         \
}while(0)

/*! arrival matching */
#define ARRIVAL_MATCHING(N,AG,VOQs,arrival_match,is_input_matched,output_matched)                             \
do{                                                                                                             \
  int _i, _pre_i,_k;                                                                                          \
   /*! Step 1: greedily match in-out pairs which have arrival */                                                \
   for (_i = 0;_i < N;++ _i){                                                                                   \
     if (is_input_matched[_i]) continue; /* matched before */                                                   \
     _k = AG[_i];                                                                                               \
     if (_k != -1 && _k < N){/*! arrival from _i to _k */                                                       \
        if (output_matched[_k] == -1)                                                                             \
            output_matched[_k] = _i;                                                                              \
        else {/*! multiple inputs want to go */                                                                 \
            _pre_i = output_matched[_k];                                                                          \
            if (is_input_matched[_pre_i]) continue; /* matched before */                                        \
            if (VOQs[_i][_k].count > VOQs[_pre_i][_k].count) {/*! current match is `better` */                  \
                output_matched[_k] = _i;                                                                          \
            }                                                                                                   \
        }                                                                                                       \
     }                                                                                                          \
   }                                                                                                            \
   for (_k = 0;_k < N;++ _k) {                                                                                  \
     _i = output_matched[_k];                                                                                     \
     if (_i != -1 && !is_input_matched[_i]){ /* unmatched before */                                             \
        is_input_matched[_i] = true;                                                                            \
        arrival_match[_i] = _k;                                                                                 \
    }                                                                                                       \
   }                                                                                                            \
}while(0)

/* i-LQF */
#define ILQF_ONE_ROUND(N,VOQs,S,is_input_matched,is_output_matched,check,rand_perm)                      \
do{                                                                                                      \
    int _i,_j,_k,_c;                                                                                     \
    /* Request & Grant Phase */                                                                          \
    for (_k = 0; _k < N; _k++) {/* for each egress */                                                    \                         
        _i = -1;                                                                                         \
        check[_k] = -1;                                                                                  \  
        if (!is_output_matched[_k]) {/* unmatched egress */                                              \
            rpermute_s(N,rand_perm);                                                                     \
            for (_j = 0; _j < N; _j++) {                                                                 \
                _c = rand_perm[_j];                                                                      \
                /* pick the unmatched ingress with largest queue length */                               \                                                                                  
                if ((!is_input_matched[_c]) && B[_c][_k].count > 0) {                                    \      
                    if (_i == -1) _i = _c;                                                               \
                    else if (B[_c][_k].count > B[_i][_k].count) _i = _c;                                 \                                        
                }                                                                                        \    
            }                                                                                            \    
        }                                                                                                \
        if (_i != -1) check[_k] = _i;                                                                    \                               
    }                                                                                                    \
    /* Accept Phase */                                                                                   \
    for (_i = 0;_i < N;_i ++){/* for each ingress */                                                     \
        _k = -1;                                                                                         \
        if (!is_input_matched[_i]){/* unmatched ingress */                                               \
            rpermute_s(N,rand_perm);                                                                     \
            for (_j = 0;_j < N;_j ++){                                                                   \
                _c = rand_perm[_j];                                                                      \
                /* pick the unmatched egress with largest queue length */                                \
                if (check[_c] == _i){                                                                    \
                    if (_k == -1) _k = _c;                                                               \
                    else if (B[_i][_c].count > B[_i][_k].count) _k = _c;                                 \
                }                                                                                        \
            }                                                                                            \
        }                                                                                                \
        if (_k != -1){                                                                                   \
            is_input_matched[_i] = true;                                                                 \
            is_output_matched[_k] = true;                                                                \
            S[_i] = _k;                                                                                  \
        }                                                                                                \
    }                                                                                                    \
} while(0)


/* general functions */
void incoming_phase();
void initialize();
void garbage_collect();
void incoming_step(int, int, int);
void schedule_phase();
void transfer_phase(int);
void initialize_arrival();
void process();
void set_alg_name();

/* benchmark algorithms */
int *merge(int *,int *); /* required by Serena */
void iSLIP();
void PIM();
void naive_random();
void enhanced_random(int);
void MWM();
void threshold();
void Serena();
void iSLIP_VR(int);
void iLQF();
void Arrival_iSLIP();

/* previous probe: rate proportional sampling */
void iSLIP_RPS();
void PIM_RPS();
void threshold_RPS();

/* new probes */     
/* queue proportional sampling (QPS) */         
void QPS_PIM(int);
void QPS_iSLIP(int);
void QPS_iLQF();
void QPS_VR(int);
void QPS_RRM(int);
void QPS_Serena(int);

/* QPS + Arrival (QPS higher priority)*/
void QPS_Arrival_iSLIP();
void QPS_Arrival_Serena();

/* Arrival + QPS (Arrival higher priority) */
void Arrival_QPS_iSLIP();
void Arrival_QPS_Serena();

/* Arrival + QPS (equal priority, alternatively use one of them after another) */
void QPS_Arrival_ALT_iSLIP(int);
void QPS_Arrival_ALT_Serena(int);

/* function queue */
void FQPS_Serena(weight_func f);
void FQPS_Arrival_Serena(weight_func f);
void Arrival_FQPS_Serena(weight_func f);
void FQPS_Arrival_ALT_Serena(int schedule_opt,weight_func f);
void FQPS_iSLIP(weight_func f);
void FQPS_Arrival_iSLIP(weight_func f);
void Arrival_FQPS_iSLIP(weight_func f);
void FQPS_Arrival_ALT_iSLIP(int schedule_opt,weight_func f);

#ifdef CUSTOMIZED_ITERATION_SUPPORT
void FQPS_VR(int customized_round, weight_func f);
void FQPS_WOREP_VR(int customized_round, weight_func f);
#else
void FQPS_VR(weight_func f);
void FQPS_WOREP_VR(weight_func f);
#endif

void LQF_Serena(weight_func f);

/* delayed schedules */
void Delayed_Serena();
void Delayed_QPS_Serena();

#endif



