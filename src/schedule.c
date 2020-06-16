#include "schedule.h"
/*
 * GENERAL OPERATIONAL FRAMEWORK
 */

void initialize()
{
    int i, k;

    core_initialize();

    /* input */
    for (i = 0; i < N; i++) {
        for (k = 0; k < N; k++) {
            initialize_queue(&B[i][k]);
        }
        S[i] = i; /* initial schedule */     
    }
}

void garbage_collect()
{
#ifdef DEBUG
    FUNCTION_CALL_PRINT;
#endif
    int i, k;

    /* input */
    for (i = 0; i < N; i++) {
        for (k = 0; k < N; k++) {
            free_queue(&B[i][k]);
        }
#ifdef QPS_V
        if (packet_vector_initialized)  
            vector_free(&Packets[i]); /*! release the memory of packet list */
#endif 
#ifdef FQPS_V
        if (voq_tree_initialized)
            tree_destroy(&tree[i]);
#endif       
    }
#ifdef DELAY_V
    for (i = 0;i < DELAY;++ i)
        ARRAY_INIT(N,SS[i],-1);
    CIRCLE_COUNTER = 0;
#endif
#ifdef QPS_V
    packet_vector_initialized = false; /*! set packet vector as uninitialized */
#endif

#ifdef FQPS_V
    voq_tree_initialized = false;
#endif

#ifdef ISLIP_V
    pointers_initialized = false;
#endif

}

void incoming_step(m, i, k)
int m, i, k;
{
    int cur_time;

    T_arr++;

    cur_time = m;
    create_and_insert_entry(&B[i][k], i, k, cur_time);

    A[i] = k; /* keep arrival track */
}

void schedule_phase()
{
    if (func_list[alg_option].param == NULL)
    {
#ifdef FQPS_V
                 func_list[alg_option].func(tested_wfs[current_wfi]);       
#else 
            func_list[alg_option].func();  
#endif   
    }
    else
    {               
#ifdef FQPS_V
            func_list[alg_option].func(func_list[alg_option].param->value,tested_wfs[current_wfi]);
#else
            func_list[alg_option].func(func_list[alg_option].param->value);
#endif
        if (func_list[alg_option].param->type == ALTER)
            func_list[alg_option].param->value = (func_list[alg_option].param->value + 1) % 2;

    }



    /*
    switch (alg_option) {
    case 0:
        iSLIP();
        break;
    case 1:
        iSLIP_RPS();
        break;
    case 2:
        PIM();
        break;
    case 3:
        PIM_RPS();
        break;
    case 4:
        MWM();
        break;
    case 5:
    	threshold();
    	break;
    case 6:
    	threshold_RPS();
    	break;
    case 7:
        naive_random();
        break;
    case 8:
        enhanced_random(32);
        break;
    case 9:
        QPS_PIM();
        break;
    case 10:       
        Serena();
        break;
    case 11:
        QPS_iSLIP(RQPS);
        break;
    case 12:
        QPS_VR(log_ceil(N));
        break;
    case 13:
        iSLIP_VR(log_ceil(N));
        break;
    case 14:
        QPS_RRM(RQPS);
        break;
    case 15:
        QPS_Serena(RQPS);
        break;
    default:
        MWM();
        break;
    }*/
}

void transfer_phase(m)
int m;
{
    int i, k;
    entry_t *entry;
    int cur_time, delay;

    for (i = 0; i < N; i++) {
        k = S[i];

        if (k >= 0 && k < N) { /* valid schedule */
            if (B[i][k].count > 0) { /* cells available */
                T_dep++;

                entry = remove_entry(&B[i][k]);

                cur_time = m;
                delay = cur_time - entry->packet.time;

                update_delays(delay);

                FREE(entry);
            }
        }
    }
}

void initialize_arrival()
{
    int i;

    for (i = 0; i < N; i++) {
        A[i] = -1; /* initialize no arrival */
    }
}

void process()
{
    int m;

    /* each macroframe = N cycles */
    for (m = 0; m < N * MAX_M; m++) {
        initialize_arrival();
        incoming_phase(m);
        schedule_phase();
        transfer_phase(m);
    }
}


/*
 * LOGISTICS
 */

void set_alg_name()
{
    alg_name = func_list[alg_option].name;
    /*
    switch (alg_option) {
    case 0:
        alg_name = "iSLIP";
        break;
    case 1:
        alg_name = "iSLIP_RPS";
        break;
    case 2:
        alg_name = "PIM";
        break;
    case 3:
        alg_name = "PIM_RPS";
        break;
    case 4:
        alg_name = "MWM";
        break;
    case 5:
    	alg_name = "THRESHOLD";
    	break;
    case 6:
    	alg_name = "THRESHOLD_RPS";
    	break;
    case 7:
        alg_name = "naive_random";
        break;
    case 8:
        alg_name = "enhanced_random";
        break;
    case 9:
        alg_name = "QPS_PIM";
        break;
    case 10:
        alg_name = "Serena";
        break;
    case 11:
    {
        alg_name = (char *)malloc(1024 * sizeof(char));
        sprintf(alg_name,"QPS_iSLIP(%d)",RQPS);
        break;
    }
    case 12:
        alg_name = "QPS_VR";
        break;
    case 13:
        alg_name = "iSLIP_VR";
        break;
    case 14:
    {
        alg_name = (char *)malloc(1024 * sizeof(char));
        sprintf(alg_name,"QPS_RRM(%d)",RQPS);
        break;
    }
    case 15:
    {
        alg_name = (char *)malloc(1024 * sizeof(char));
        sprintf(alg_name,"QPS_Serena(%d)",RQPS);
        break;
    }
    default:
        alg_name = "MWM";
        break;
    }*/
}
