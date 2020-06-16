#include <stdint.h> /*! for intptr_t */
#include <assert.h> /*! for assert */
#include <string.h> /*! for memcpy */

#include "schedule.h"
#include "measure_merge_time.h"

#ifdef EXPERIMENT_FOUR
int WEIGHT[MAX_N][MAX_N];
int MATCHX[MAX_N];
int MATCHY[MAX_N];
int MATCHZ[MAX_N];
#endif


/* global variables required by all algorithms */
queue_t B[P][P];        /* B[i][k] VOQs at inputs */

int A[P];               /* k = A[i] arrival graph */
int arrival_matrix[P][P];       /* arrival_matrix[i][k] if arrival input i to output k */

int S[P];               /* k = S[i] crossbar schedule */


/* global variables required by MWM */
int mwm_graph;          /* flag to see if mwm graph has been created */
Graph graph;            /* used in MWM */


#ifdef DELAY_V
int DELAY = 0;
int SS[P][P] = {-1};
int CIRCLE_COUNTER = 0;
#endif

/* global variables required by iSLIP */
#ifdef ISLIP_V
int Accept[P];
int Grant[P];
bool pointers_initialized = false;
#endif

/* global variables required by QPS */
#ifdef QPS_V
vector Packets[P]; /* array of all packets, needed by queue proportional sampling */
bool packet_vector_initialized = false;
#endif 

#ifdef FQPS_V
avl_t tree[P];
bool voq_tree_initialized = false; 
/*weight_func tested_wfs[] = {nlogn, nsquare, logn, nsqrt, ncube, nfourth,dummyfun};*/
weight_func tested_wfs[] = {nself,dummyfun};
int current_wfi = 0;
int maximum_wfi = 1;
bool fqps_turn_on = false;
#endif

#ifdef ENHANCED_RANDOM_U
param_t erp = {.type = SINGLE,.value = 32};
#endif

#if defined(QPS_PIM_U) || defined(QPS_RRM_U) || defined(QPS_ISLIP_U) || defined(QPS_SERENA_U) 
param_t qpp  = {.type = RANGE,.value = DEF_MAX_ITER_QPS};
#endif


#ifdef QPS_VR_U
param_t qpsp = {.type = RANGE,.value = DEF_MAX_ITER};
#endif

#ifdef FQPS_VR_U
param_t fqpsp = {.type = RANGE,.value = DEF_MAX_ITER};
#endif

#ifdef ISLIP_VR_U
param_t ip = {.type = RANGE,.value = DEF_MAX_ITER};
#endif

#ifdef QPS_ARRIVAL_ALT_ISLIP_U
param_t qaai = {.type = ALTER,.value = 0};
#endif

#ifdef QPS_ARRIVAL_ALT_SERENA_U
param_t qaas = {.type = ALTER,.value = 0};
#endif

/* functions for experiment 1 */
#ifdef EXPERIMENT_ONE
func_t func_list[] = {
/*
#ifdef PIM_U
{.name = "PIM", .func = PIM, .param = NULL},
#endif
#ifdef PIM_RPS_U
{.name = "PIM_RPS", .func = PIM_RPS, .param = NULL},
#endif
#ifdef THRESHOLD_U
{.name = "THRESHOLD", .func = threshold, .param = NULL},
#endif
#ifdef THRESHOLD_RPS_U
{.name = "THRESHOLD_RPS", .func = threshold_RPS, .param = NULL},
#endif
#ifdef NAIVE_RANDOM_U
{.name = "NAIVE_RANDOM", .func = naive_random, .param = NULL},
#endif
#ifdef ENHANCED_RANDOM_U
{.name = "ENHANCED_RANDOM", .func = enhanced_random, .param = &erp},
#endif
#ifdef QPS_PIM_U
{.name = "QPS_PIM", .func = QPS_PIM, .param = &qpp},
#endif
*/
#ifdef ISLIP_VR_U
{.name = "iSLIP", .func = iSLIP_VR, .param = &ip},
#endif
#ifdef SERENA_U
{.name = "Serena", .func = Serena, .param = NULL},
#endif
#ifdef QPS_ISLIP_U
{.name = "QPS_iSLIP", .func = QPS_iSLIP, .param = &qpp},
#endif
#ifdef QPS_VR_U
{.name = "QPS", .func = QPS_VR, .param = &qpsp},
#endif
/*
#ifdef QPS_RRM_U
{.name = "QPS_RRM", .func = QPS_RRM, .param = &qpp},
#endif
*/
#ifdef QPS_SERENA_U
{.name = "QPS_Serena", .func = QPS_Serena, .param = &qpp},
#endif
/*
#ifdef QPS_ARRIVAL_ISLIP_U
{.name = "QPS_ARRIVAL_ISLIP",.func = QPS_Arrival_iSLIP, .param = NULL},
#endif
#ifdef QPS_ARRIVAL_SERENA_U
{.name = "QPS_ARRIVAL_SERENA",.func = QPS_Arrival_Serena, .param = NULL},
#endif
#ifdef ARRIVAL_QPS_ISLIP_U
{.name = "ARRIVAL_QPS_ISLIP", .func = Arrival_QPS_iSLIP, .param = NULL},
#endif
#ifdef ARRIVAL_QPS_SERENA_U
{.name = "ARRIVAL_QPS_SERENA", .func = Arrival_QPS_Serena, .param = NULL},
#endif
#ifdef ARRIVAL_ISLIP_U
{.name = "ARRIVAL_ISLIP", .func = Arrival_iSLIP, .param = NULL},
#endif
#ifdef QPS_ARRIVAL_ALT_ISLIP_U
{.name = "QPS_ARRIVAL_ALT_ISLIP", .func = QPS_Arrival_ALT_iSLIP, .param = &qaai},
#endif
#ifdef QPS_ARRIVAL_ALT_SERENA_U
{.name = "QPS_ARRIVAL_ALT_SERENA", .func = QPS_Arrival_ALT_Serena, .param = &qaas},
#endif
*/
#ifdef ILQF_U
{.name = "iLQF", .func = iLQF, .param = NULL},
#endif
#ifdef MWM_U
{.name = "MWM", .func = MWM, .param = NULL},
#endif
{.name = "#END", .func = NULL, .param = NULL} /* please do not remove this line */
};
#endif

#ifdef FQPS_ARRIVAL_ALT_SERENA_U
param_t fqaas = {.type = ALTER,.value = 0};
#endif

#ifdef FQPS_ARRIVAL_ALT_ISLIP_U
param_t fqaai = {.type = ALTER,.value = 0};
#endif

/* function list for function-queue tests */
#ifdef EXPERIMENT_TWO
#ifdef FQPS_V
func_t func_list[] = {
#ifdef LQF_SERENA_U
    {.name = "LQF_Serena", .func = LQF_Serena, .param = NULL},
#endif
/*
#ifdef FQPS_ARRIVAL_ALT_SERENA_U
    {.name = "FQPS_Arrival_ALT_Serena", .func = FQPS_Arrival_ALT_Serena, .param = &fqaas},
#endif
#ifdef FQPS_ISLIP_U
    {.name = "FQPS_iSLIP", .func = FQPS_iSLIP, .param = NULL},
#endif
#ifdef FQPS_ARRIVAL_ISLIP_U
    {.name = "FQPS_Arrival_iSLIP", .func = FQPS_Arrival_iSLIP, .param = NULL},
#endif
#ifdef ARRIVAL_FQPS_ISLIP_U
    {.name = "Arrival_FQPS_iSLIP", .func = Arrival_FQPS_iSLIP, .param = NULL},
#endif
#ifdef FQPS_ARRIVAL_ALT_ISLIP_U
    {.name = "FQPS_Arrival_ALT_iSLIP", .func = FQPS_Arrival_ALT_iSLIP, .param = &fqaai},
#endif
*/
#ifdef FQPS_VR_U
    {.name = "FQPS_VR_WOREP", .func = FQPS_WOREP_VR, .param = &fqpsp},
    {.name = "FQPS_VR", .func = FQPS_VR, .param = &fqpsp},
#endif
#ifdef FQPS_SERENA_U
    {.name = "FQPS_Serena", .func = FQPS_Serena, .param = NULL},
#endif
/*
#ifdef FQPS_ARRIVAL_SERENA_U
    {.name = "FQPS_Arrival_Serena", .func = FQPS_Arrival_Serena, .param = NULL},
#endif
#ifdef ARRIVAL_FQPS_SERENA_U
    {.name = "Arrival_FQPS_Serena", .func = Arrival_FQPS_Serena, .param = NULL},
#endif
*/
{.name = "#END", .func = NULL, .param = NULL} /* please do not remove this line */   
};
#endif
#endif

/* function list for delayed schedule test */
#ifdef EXPERIMENT_THREE
#ifdef DELAY_V
func_t func_list[] = {
#ifdef DELAYED_QPS_SERENA_U
    {.name = "Delayed_QPS_Serena", .func = Delayed_QPS_Serena, .param = NULL},
#endif
#ifdef DELAYED_SERENA_U
    {.name = "Delayed_Serena", .func = Delayed_Serena, .param = NULL},
#endif
{.name = "#END", .func = NULL, .param = NULL} /* please do not remove this line */  
};
#endif
#endif


#if !(defined(EXPERIMENT_ONE) || defined(EXPERIMENT_TWO) || defined(EXPERIMENT_THREE))
func_t func_list[] = {
    {.name = "#END", .func = NULL, .param = NULL}
};
#endif

int main(int argc,char **argv)
{
    parse_options(argc, argv);
    
    int i, param_bk = 0; 
    char alg_name_bk[1024];
    alg_option = 0;
#ifdef EXPERIMENT_ONE
    while (strcmp(func_list[alg_option].name,"#END") != 0)
    {      
        if (func_list[alg_option].param != NULL && func_list[alg_option].param->type == RANGE)
        {
            param_bk = func_list[alg_option].param->value;
            memset(alg_name_bk, 0, 1024);
            strcpy(alg_name_bk, func_list[alg_option].name);
            while (func_list[alg_option].param->value > 0)
            {
                sprintf(func_list[alg_option].name,"%s(%d)",alg_name_bk, func_list[alg_option].param->value);
                set_alg_name();
                run_main_loop();
                -- func_list[alg_option].param->value;  
            }
            func_list[alg_option].param->value = param_bk;
        }
        else
        {
             set_alg_name();
             run_main_loop();          
        }
        ++ alg_option;
              
    }
#endif

#ifdef EXPERIMENT_TWO 
    fqps_turn_on = true;   
#ifdef FQPS_V
    /* fq experiment */
    alg_option = 0;
    current_wfi = 0;
    while (strcmp(func_list[alg_option].name,"#END") != 0)
    {
        if (NULL != strstr(func_list[alg_option].name, "FQPS_VR")){
            /* function name contain FQPS_VR */
            param_bk = func_list[alg_option].param->value;
            memset(alg_name_bk, 0, 1024);
            strcpy(alg_name_bk, func_list[alg_option].name);
            while (func_list[alg_option].param->value > 0)
            {
                sprintf(func_list[alg_option].name,"%s(%d)",alg_name_bk, func_list[alg_option].param->value);
                set_alg_name();
                run_main_loop();
                -- func_list[alg_option].param->value;  
            }
            func_list[alg_option].param->value = param_bk;
        }
        else
        {
            current_wfi = maximum_wfi;
            set_alg_name();
            run_main_loop();           
        }
        ++ alg_option;
    }
    fqps_turn_on = false;
#endif
#endif

#ifdef EXPERIMENT_THREE    
#ifdef DELAY_V
    /* delayed schedule experiments */
    alg_option = 0;

    while (strcmp(func_list[alg_option].name,"#END") != 0)
    {
        sprintf(alg_name_bk,"%s",func_list[alg_option].name);
        for (DELAY = MIN_DELAY;DELAY <= MAX_DELAY;DELAY *= 2)
        {
            sprintf(func_list[alg_option].name,"%s(%d)",alg_name_bk,DELAY);
            CIRCLE_COUNTER = 0;
            set_alg_name();
            run_main_loop();           
        }
        ++ alg_option;
        memset(alg_name_bk,0,1024);
    }
#endif 
#endif   

#ifdef EXPERIMENT_FOUR
    test_correctness();
    merge_time_test();
#endif
    return 0;
}
