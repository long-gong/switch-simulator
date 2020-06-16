#include "common.h"
#include "random_variable.h"

/* need to compute maximum for maximum weighted matching */
#define max(a,b) \
 ({__typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })

double pareto_constant;
double pareto_denominator[10001];
double pareto_probability[10001];
double pareto_range[10001];

static void pareto_compute()
{
    int i;
    double total;

    total = 0.0;
    for (i = 1; i <= 10000; i++) {
        pareto_denominator[i] = 1.0 / pow(((double) i), 2.5);
        total = total + pareto_denominator[i];
    }

    pareto_constant = 1.0 / total;

    pareto_range[0] = 0.0;
    for (i = 1; i <= 10000; i++) {
        pareto_probability[i] = pareto_constant * pareto_denominator[i];
        pareto_range[i] = pareto_range[i - 1] + pareto_probability[i];
    }
}

int pareto_burst()
{
    int i, found, length;
    double p;

    p = random_01();

    if (p >= 1.0) {
        length = 10000;
    } else {
        for (found = 0, i = 1; i <= 10000 && !found; i++) {
            if (p < pareto_range[i]) {
                found = 1;
                length = i;
            }
        }
    }

    return length;
}

int geometric_burst()
{
    double p = 1.0 / (average_burst_length + 1);
    int length = CGeometricRandomVariable(p);
#ifdef DEBUG
    printf("p = %.4f, length = %4d\n",p,length);
#endif
    return length;
}

void initialize_queue(queue)
queue_t *queue;
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
}

void free_queue(queue)
queue_t *queue;
{
    int i;
    entry_t *entry;

    while (queue->count > 0) {
        entry = remove_entry(queue);
        FREE(entry);
    }
}

entry_t *create_entry(src, dest, time)
int src;
int dest;
int time;
{
    entry_t *entry;

    entry = ALLOC(struct entry_t, 1);

    entry->packet.src = src;
    entry->packet.dest = dest;
    entry->packet.time = time;

    entry->packet.index = -1; /* add by Long */

    entry->prev = NULL;
    entry->next = NULL;

    return entry;
}

void insert_entry(queue, entry)
queue_t *queue;
entry_t *entry;
{
    entry->prev = NULL;
    entry->next = NULL;

    if (queue->count == 0) {
        queue->head = entry;
        queue->tail = entry;
        queue->count = 1;
    } else {
        entry->prev = queue->tail;
        queue->tail->next = entry;
        queue->tail = entry;
        queue->count++;
    }
}

void create_and_insert_entry(queue, src, dest, time)
queue_t *queue;
int src;
int dest;
int time;
{
    entry_t *entry;

    entry = create_entry(src, dest, time);
    insert_entry(queue, entry);
}

entry_t *remove_entry(queue)
queue_t *queue;
{
    entry_t *entry;

    if (queue->count == 0) {
        (void) fprintf(stderr, "ERROR: remove_entry, queue empty\n");
        (void) exit(-1);
    } else if (queue->count == 1) {
        entry = queue->head;
        queue->count = 0;
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        entry = queue->head;
        queue->count--;
        queue->head = entry->next;
        queue->head->prev = NULL;
    }

    entry->prev = NULL;
    entry->next = NULL;

    return entry;
}

void unlink_entry(queue, entry)
queue_t *queue;
entry_t *entry;
{
    entry_t *prev_entry;
    entry_t *next_entry;

    /* makes strong assumption that entry is in queue !!! */

    if (entry == NULL) {
        (void) fprintf(stderr, "ERROR: unlink_entry, entry NULL\n");
        (void) exit(-1);
    }

    if (queue->count == 0) {
        (void) fprintf(stderr, "ERROR: unlink_entry, queue empty\n");
        (void) exit(-1);
    }

    /* otherwise, consider following cases */

    if (queue->count == 1) {

        queue->count = 0;
        queue->head = NULL;
        queue->tail = NULL;

        entry->prev = NULL;
        entry->next = NULL;

    } else if (entry == queue->head) { /* entry is first item */

        queue->count--;
        queue->head = entry->next;
        queue->head->prev = NULL;

        entry->prev = NULL;
        entry->next = NULL;

    } else if (entry == queue->tail) { /* entry is last item */

        queue->count--;
        queue->tail = entry->prev;
        queue->tail->next = NULL;

        entry->prev = NULL;
        entry->next = NULL;

    } else { /* entry in middle */

        queue->count--;

        prev_entry = entry->prev;
        next_entry = entry->next;

        prev_entry->next = next_entry;
        next_entry->prev = prev_entry;

        entry->prev = NULL;
        entry->next = NULL;

    }

}


int random_int(N)
int N;
{
    double r, n;
    int results;

    r = random() / (RAND_MAX + 1.0);
    n = (double) N;
    r = r * N;
    results = (int) r;

    return results;
}

double random_01()
{
    double r;

    r = random() / (RAND_MAX + 1.0);

    return r;
}

void core_initialize()
{
    int i;
    double tmp;


    time_t seed = time(0);
    /*printf ("random seed: %lld\n",seed);*/
    (void) srandom(seed);

    T_arr = 0.0;
    T_dep = 0.0;

    max_delay = 0;
    mean_delay = 0.0;
    mean_delay_count = 0.0;

    pareto_compute(); /* initialize pareto distribution */
    for (i = 0; i < N; i++) {
        burst[i].on = 0;
        burst[i].length = 0;
        burst[i].dest = 0;
    }

    if (inject_option == 2)
    {/* new on-off burst model */
        prob_on = 1.0 / (1.0 + average_burst_length);
        tmp = amount / (1.0 - amount); /* on2off ratio */
        prob_off = tmp / (tmp + average_burst_length);
#ifdef OF_DEBUG
        printf("[OF_DEBUG]: load = %.2f, q = %.6f, p = %.6f\n",amount,prob_on,prob_off);
#endif
    }
}

int generate_traffic(i)
int i;
{
    int j, r, t, found;
    double c, range, h;
    int max_check;

    switch (model_option) {
    case 0:
        /* uniform traffic */
        r = random_int(N);
        break;
    case 1:
        /* diagonal */
        c = (double) random_int(N);
        range = (((double) N) * 2.0) / 3.0;
        if (c < range) {
            r = i;
        } else {
            r = (i + 1) % N;
        }
        break;
    case 2:
        /* log diagonal traffic */
        c = (double) random_int(N);
        if (c < (0.5 * ((double) N))) {
            r = i;
        } else if (c < (0.75 * ((double) N))) {
            r = (i + 1) % N;
        } else if (c < (0.875 * ((double) N))) {
            r = (i + 2) % N;
        } else if (c < (0.9375 * ((double) N))) {
            r = (i + 3) % N;
        } else if (c < (0.96875 * ((double) N))) {
            r = (i + 4) % N;
        } else {
            r = (i + 5) % N;
        }
        break;
    case 4:
        /* new mode */
        c = (double) random_int(N);
        if (c < (0.5 * ((double) N))) {
            r = 1;
        } else if (c < (0.75 * ((double) N))) {
            r = 2 % N;
        } else if (c < (0.875 * ((double) N))) {
            r = 3 % N;
        } else if (c < (0.9375 * ((double) N))) {
            r = 4 % N;
        } else if (c < (0.96875 * ((double) N))) {
            r = 5 % N;
        } else if (c < (0.984375 * ((double) N))) {
            r = 6 % N;
        } else {
            r = 7 % N;
        }
        break;
    default:
        /* quasi diagonal traffic */
        c = (double) random_int(N);
        if (c < (0.5 * ((double) N))) {
            r = i;
        } else {
	    h = 0.5;
	    for (j = 1; j < N; j++) {
	        h = h + (1.0 / (((double) (N - 1)) * 2.0));
	        if (c < (h * ((double) N))) {
                    r = (i + j) % N;
                    return r;
	        }
	    }
            r = (i + (N - 1)) % N;
            return r;
        }
    }

    return r;
}

static double compute_throughput()
{
    return ((T_dep * 100.0) / T_arr);
}

void update_delays(delay)
int delay;
{
    double temp;

    max_delay = delay > max_delay ? delay : max_delay;
    temp = (mean_delay * mean_delay_count) + ((double) delay);
    mean_delay_count = mean_delay_count + 1.0;
    mean_delay = temp / mean_delay_count;
}

void set_model_name()
{
    switch (model_option) {
    case 0:
        model_name = "uniform";
        break;
    case 1:
        model_name = "diagonal";
        break;
    case 2:
        model_name = "log diagonal";
        break;
    case 4:
        model_name = "qps unfriendly";
        break;
    default:
        model_name = "quasi diagonal";
        break;
    }
}

void set_inject_name()
{
    switch (inject_option) {
    case 0:
        inject_name = "bernoulli";
        break;
    case 1:
        inject_name = "pareto";
        break;
    case 2:
        inject_name = "burst";
        break;
    default:
        inject_name = "bernoulli";
        break;
    }
}

static void usage()
{
    (void) fprintf(stderr, "usage: [options]\n\n");
    (void) fprintf(stderr, "-%s\t\t%s\n", "h", "print usage");
    (void) fprintf(stderr, "-%s\t\t%s\n",
        "p", "use Pareto distribution (default Bernoulli)");
    (void) fprintf(stderr, "-%s %s\t\t%s\n",
        "u", "average_burst_length", "use geometric burst distribution (default: mean = N)");
    (void) fprintf(stderr, "-%s\t\t%s\n",
        "w", "use skewed input loading (default Bernoulli)");
    (void) fprintf(stderr, "-%s\t\t%s\n", "f", "set user flag 1");
    (void) fprintf(stderr, "-%s\t\t%s\n", "g", "set user flag 2");
    (void) fprintf(stderr, "-%s\t\t%s\n", "a", "set user flag 3");
    (void) fprintf(stderr, "-%s\t\t%s\n", "s", "set user flag 4");
    (void) fprintf(stderr, "-%s\t\t%s\n", "b", "set user flag 5");
    (void) fprintf(stderr, "-%s %s\t\t%s\n",
        "n", "size", "specify N, number of input/output ports (default 32)");
    (void) fprintf(stderr, "-%s %s\t%s\n",
        "m", "frames", "specify number of macroframes (default N*500)");
    (void) fprintf(stderr, "-%s %s\t%s\n",
        "l", "minimum_traffic_load", "specify minimum traffic load (default 0.1, including)");
    (void) fprintf(stderr, "-%s %s\t%s\n",
        "L", "maximum_traffic_load", "specify maximum traffic load (default 1.0, including)");
    (void) fprintf(stderr, "-%s %s\t%s\n",
        "S", "traffic_load_step", "specify traffic load step (default 0.1)");
    (void) exit(-1);
}

void parse_options(argc, argv)
int argc;
char **argv;
{
    extern int optind;
    extern char *optarg;
    int c;

    N = 32;
    MAX_M = -1;

    inject_option = 0;

    average_burst_length = -1;
    minimum_load = 0.1;
    maximum_load = 1.0;
    load_step = 0.1;

    flag1 = 0;
    flag2 = 0;
    flag3 = 0;
    flag4 = 0;


    while ((c=getopt(argc,argv,"hpu:fgasbn:m:l:L:S:"))!=EOF) {
        switch(c) {
        case 'h':
            usage();
            break;
        case 'p':
            inject_option = 1;
            break;
        case 'u':
            inject_option = 2;
            average_burst_length = atof(optarg); /* added by Long Gong */
            break;
        case 'f':
            flag1 = 1;
            break;
        case 'g':
            flag2 = 1;
            break;
        case 'a':
            flag3 = 1;
            break;
        case 's':
            flag4 = 1;
            break;
        case 'b':
            flag5 = 1;
            break;
        case 'n':
            N = atoi(optarg);
            break;
        case 'm':
            MAX_M = atoi(optarg);
            break;
        case 'l':
            minimum_load = atof(optarg);
            break;
        case 'L':
            maximum_load = atof(optarg);
            break;
        case 'S':
            load_step = atof(optarg);
            break;
        default:
            usage();
        }
    }

    
    if (MAX_M < 0) MAX_M = N * 500;
    if (average_burst_length < 0) average_burst_length = N; /* added by Long Gong */

    if (optind != argc) {
        usage();
    }

    set_inject_name();
}

void print_header()
{
    printf("\n[ ");
    printf("N = %d | ", N);
    printf("SLOTS = %d | ", N*MAX_M);
    
    printf("%s | ", alg_name);
    printf("%s ", model_name);
    if (inject_option != 0) { /* pareto or burst */
        printf("- %s ", inject_name);
    }
    if (flag1) {
        printf("| flag1 = yes ");
    }
    printf("]\n\n");

    printf("%4s\t", "Load");
    printf("%10s\t", "Throughput");
    printf("%10s\t", "Mean Delay");
    printf("%9s\n", "Max Delay");

    (void) fflush(stdout);
}

void print_results()
{
    printf("%4.2f\t", amount);
    printf("%10.2f\t", compute_throughput());
    printf("%10.2f\t", mean_delay);
    printf("%9d\n", max_delay);

    (void) fflush(stdout);
}

void run_main_loop()
{
    double alpha;

    /* run through traffic models */
    for (model_option = 0; model_option < NUM_MODELS; model_option++) {
        set_model_name();
        print_header();

        for (alpha = maximum_load; alpha >= minimum_load; alpha = alpha - load_step) {
            /*amount = (alpha == 1.0) ? 0.95 : alpha;*/
            amount = (alpha == 1.0) ? 0.99 : alpha; /* for throughput */

            if (model_option == 4) {
                amount = 2 * amount / N;
            }
            initialize();
            process();
            garbage_collect();
            print_results();
        }
    }
}
