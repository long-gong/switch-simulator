#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)		\
    ((obj) ? (free((char *) (obj)), (obj) = 0) : 0)

#define P 256			/* max number of ports */
#define NUM_MODELS 5		/* number of model options */

int alg_option;   		/* algorithm option (not always used) */
char *alg_name;   		/* algorithm name (not always used) */

int model_option; 		/* traffic model option (default = uniform) */
char *model_name; 		/* traffic model name */

int inject_option; 		/* injection option (default = Bernoulli) */
char *inject_name; 		/* injection name */

int N;				/* number of ports */
int MAX_M;  			/* number of macroframes */

int max_delay;			/* max delay */
double mean_delay;  		/* mean delay */
double mean_delay_count;	/* used in mean delay calc */

double T_arr;  			/* total arrival */
double T_dep;  			/* total departure */

double average_burst_length;  /* average burst length for geometric burst */
double prob_on; /* probability for on to off */
double prob_off; /* probability for off to on */

double minimum_load; /* minimum load */
double maximum_load; /* maximum load */
double load_step; /* load step */

typedef struct packet_t {
    int src;
    int dest;
    int time;
    int index; /*! added by Long, needed by queue proportional sampling to eliminate hash table */
} packet_t;

typedef struct entry_t {
    struct packet_t packet;
    struct entry_t *prev;
    struct entry_t *next;
} entry_t;

typedef struct queue_t {
    struct entry_t *head;
    struct entry_t *tail;
    int count;
} queue_t;

double amount;			/* load related */

int flag1;  			/* user flag 1 */
int flag2;  			/* user flag 2 */
int flag3;  			/* user flag 3 */
int flag4;  			/* user flag 4 */
int flag5;  			/* user flag 5 */

extern void initialize_queue();
extern void free_queue();

extern entry_t *create_entry();
extern void insert_entry();
extern void create_and_insert_entry();
extern entry_t *remove_entry();
extern void unlink_entry();

extern int random_int();
extern double random_01();

extern void core_initialize();
extern int generate_traffic();

extern void update_delays();

extern void parse_options();

extern void print_header();
extern void print_results();

extern void set_model_name();
extern void set_inject_name();

extern void run_main_loop();

/* PARETO */

typedef struct burst_t {
    int on;
    int length;
    int dest;
} burst_t;

burst_t burst[P];

extern int pareto_burst();

#endif