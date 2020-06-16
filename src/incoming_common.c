#include "common.h"

void incoming_phase(m)
int m;
{
    int i, k, sbk;
    double threshold, tn;

    for (i = 0; i < N; i++) {
        if (inject_option == 0) { /* bernoulli */
            if (random_01() < amount) {
                k = generate_traffic(i);
                incoming_step(m, i, k);
            }
        } else if (inject_option == 2){/* new on-off model */
#ifdef OF_DEBUG
            printf("[OF_DEBUG]: current circle status = (%d,%d)\n",i,burst[i].on);
#endif
            do
            {
                sbk = burst[i].on;
                if (burst[i].on) {/* previous circle on */
                    if (random_01() < prob_on) /* transfer to off */
                        burst[i].on = 0;
                } else { /* previous circle off */
                    if (random_01() < prob_off) {/* transfer to on */
                        burst[i].dest = generate_traffic(i);
                        burst[i].on =  1;               
                    }
                } 
            } while (burst[i].on != sbk);

            if (burst[i].on) {/* if on, then generate traffic */
                k = burst[i].dest;
                incoming_step(m, i, k);               
            }

        } 
        else { /* pareto or constant or geometric */
            if (burst[i].length == 0) {
                burst[i].on = (random_01() < amount); 
                if (inject_option == 1) {
                    burst[i].length = pareto_burst();
                } else {
                    burst[i].length = (int)average_burst_length;
                }
                if (burst[i].on) {
                    burst[i].dest = generate_traffic(i);
                }
            }

            if (burst[i].on) {
                k = burst[i].dest;
                incoming_step(m, i, k);
            }
            burst[i].length--;
        }
    }
}
