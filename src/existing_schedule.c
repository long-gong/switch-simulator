#include "schedule.h"

/*! maximum weighted matching based */
/*! can be viewed as the optimal */
void MWM() {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif
#ifdef MWM_U
  int i, k;
  FILE *fp;
  Edge edge, other_edge;
  int *Mate;

  (void)fflush(stdout);

  if (mwm_graph == 0) { /* only create graph if not created already */
    graph = NewGraph(2 * N);
    mwm_graph = 1;

    for (i = 0; i < N; i++) {
      for (k = 0; k < N; k++) {
        if (B[i][k].count > 0) {
          AddEdge(graph, i + 1, k + 1 + N, B[i][k].count);
        }
      }
    }
  } else { /* graph already created, just modify edge labels */
    for (i = 0; i < N; i++) {
      for (k = 0; k < N; k++) {
        edge = FindEdge(graph, i + 1, k + 1 + N);
        other_edge = FindEdge(graph, k + 1 + N, i + 1);
        if (edge == NULL && other_edge != NULL) {
          (void)fprintf(stderr, "ERROR: MWM 2\n");
          (void)exit(-1);
        } else if (edge != NULL && other_edge == NULL) {
          (void)fprintf(stderr, "ERROR: MWM 3\n");
          (void)exit(-1);
        } else if (edge == NULL && other_edge == NULL) {
          if (B[i][k].count > 0) {
            AddEdge(graph, i + 1, k + 1 + N, B[i][k].count);
          }
        } else {
          if (B[i][k].count > 0) {
            edge->label = B[i][k].count;
            other_edge->label = B[i][k].count;
          } else {
            RemoveEdge(graph, edge);
          }
        }
      }
    }
  }

  /* perform MWM */
  Mate = Weighted_Match((intptr_t)graph, 1);

  /* read results */
  for (i = 0; i < N; i++) {
    if (Mate[i + 1] > 0) {
      if ((Mate[i + 1] - 1 - N) < 0) {
        (void)fprintf(stderr, "ERROR: MWM 4\n");
        (void)exit(-1);
      }
      S[i] = Mate[i + 1] - 1 - N;
    } else {
      S[i] = -1;
    }
  }

  free(Mate);
#endif
#ifdef DEBUG
  FUNCTION_SUCCESS_PRINT;
#endif
}

/*! iSLIP */
/*! Note that, this implementation is not correct, since it reinitialize the
    accept and grant pointers every circle.
*/
void iSLIP() {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif
#ifdef ISLIP_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int grant[P];
  int accept[P];
  int check[P];

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
    accept[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
    grant[k] = 0;
  }

  for (t = 0; t < log_ceil(N); t++) { /* log N rounds */

    /* request grant phase */
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + grant[k]) % N;
          if (!input_matched[i] && B[i][k].count > 0) {
            found = 1;
            check[k] = i;
          }
        }
      }
    }

    /* accept phase */
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + accept[i]) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;

            /* update pointers */
            if (t == 0) { /* first round */
              grant[k] = (i + 1) % N;
              accept[i] = (k + 1) % N;
            }
          }
        }
      }
    }
  }
#endif
#ifdef DEBUG
  FUNCTION_SUCCESS_PRINT;
#endif
}

void iSLIP_RPS() {
#ifdef ISLIP_RPS_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int grant[P];
  int accept[P];
  int check[P];

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
    accept[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
    grant[k] = 0;
  }

  /* RPS makes use of arrival matrix A(t) in the 1st round */
  for (i = 0; i < N; i++) {
    for (k = 0; k < N; k++) {
      arrival_matrix[i][k] = (k == A[i]) ? 1 : 0;
    }
  }

  for (t = 0; t < (log_ceil(N) + 1); t++) { /* log N + 1 rounds */

    /* request grant phase */
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + grant[k]) % N;
          if (t == 0) {
            if (!input_matched[i] && arrival_matrix[i][k]) {
              found = 1;
              check[k] = i;
            }
          } else {
            if (!input_matched[i] && B[i][k].count > 0) {
              found = 1;
              check[k] = i;
            }
          }
        }
      }
    }

    /* accept phase */
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + accept[i]) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;

            /* update pointers */
            if (t == 0) { /* first round */
              grant[k] = (i + 1) % N;
              accept[i] = (k + 1) % N;
            }
          }
        }
      }
    }
  }
#endif
}

void PIM() {
#ifdef PIM_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int check[P];
  int random_start;

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
  }

  for (t = 0; t < log_ceil(N); t++) { /* log N rounds */

    /* request grant phase */
    random_start = random_int(N);
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + random_start) % N;
          if (!input_matched[i] && B[i][k].count > 0) {
            found = 1;
            check[k] = i;
          }
        }
      }
    }

    /* accept phase */
    random_start = random_int(N);
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + random_start) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;
          }
        }
      }
    }
  }
#endif
}

void PIM_RPS() {
#ifdef PIM_RPS_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int check[P];
  int random_start;

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
  }

  /* RPS makes use of arrival matrix A(t) in the 1st round */
  for (i = 0; i < N; i++) {
    for (k = 0; k < N; k++) {
      arrival_matrix[i][k] = (k == A[i]) ? 1 : 0;
    }
  }

  for (t = 0; t < (log_ceil(N) + 1); t++) { /* log N + 1 rounds */

    /* request grant phase */
    random_start = random_int(N);
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + random_start) % N;
          if (t == 0) {
            if (!input_matched[i] && arrival_matrix[i][k]) {
              found = 1;
              check[k] = i;
            }
          } else {
            if (!input_matched[i] && B[i][k].count > 0) {
              found = 1;
              check[k] = i;
            }
          }
        }
      }
    }

    /* accept phase */
    random_start = random_int(N);
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + random_start) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;
          }
        }
      }
    }
  }
#endif
}

void threshold() {
#ifdef THRESHOLD_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int grant[P];
  int accept[P];
  int check[P];
  int threshold;

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
    accept[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
    grant[k] = 0;
  }

  /* set threshold */
  threshold = log_ceil(N);
  /* perform threshold on first round, with iSLIP on the rest */
  for (t = 0; t < (log_ceil(N) + 1); t++) { /* log N + 1 rounds */

    /* request grant phase */
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + grant[k]) % N;
          if (t == 0) {
            if (!input_matched[i] && B[i][k].count > threshold) {
              found = 1;
              check[k] = i;
            }
          } else {
            if (!input_matched[i] && B[i][k].count > 0) {
              found = 1;
              check[k] = i;
            }
          }
        }
      }
    }

    /* accept phase */
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + accept[i]) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;

            /* update pointers */
            grant[k] = (i + 1) % N;
            accept[i] = (k + 1) % N;
          }
        }
      }
    }
  }
#endif
}

void threshold_RPS() {
#ifdef THRESHOLD_RPS_U
  int i, k;
  int t, c, found;
  int input_matched[P];
  int output_matched[P];
  int grant[P];
  int accept[P];
  int check[P];
  int threshold;

  for (i = 0; i < N; i++) {
    S[i] = -1; /* initialize to no match */
    input_matched[i] = 0;
    accept[i] = 0;
  }

  for (k = 0; k < N; k++) {
    output_matched[k] = 0;
    grant[k] = 0;
  }

  /* RPS makes use of arrival matrix A(t) in the 1st round */
  for (i = 0; i < N; i++) {
    for (k = 0; k < N; k++) {
      arrival_matrix[i][k] = (k == A[i]) ? 1 : 0;
    }
  }

  threshold = log_ceil(N);
  /* perform threshold on first round, with iSLIP on the rest */
  for (t = 0; t < (log_ceil(N) + 1); t++) { /* log N + 1 rounds */

    /* request grant phase */
    for (k = 0; k < N; k++) { /* foreach output */
      check[k] = -1;
      if (!output_matched[k]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          i = (c + grant[k]) % N;
          if (t == 0) {
            /* use arrival matrix to drive events */
            if (!input_matched[i] && arrival_matrix[i][k]) {
              if (B[i][k].count > threshold) {
                found = 1;
                check[k] = i;
              }
            }
          } else {
            if (!input_matched[i] && B[i][k].count > 0) {
              found = 1;
              check[k] = i;
            }
          }
        }
      }
    }

    /* accept phase */
    for (i = 0; i < N; i++) {  /* foreach input */
      if (!input_matched[i]) { /* if unmatched */
        for (found = 0, c = 0; c < N && !found; c++) {
          k = (c + accept[i]) % N;
          if (check[k] == i) { /* found match */
            found = 1;
            S[i] = k;
            input_matched[i] = 1;
            output_matched[k] = 1;

            /* update pointers */
            grant[k] = (i + 1) % N;
            accept[i] = (k + 1) % N;
          }
        }
      }
    }
  }
#endif
}

/*!
   Naive random strategy
   Uniformly and randomly pick a matching from N! matchings
*/
void naive_random() {
#ifdef NAIVE_RANDOM_U
  int i, k;

  for (i = 0; i < N; i++) {
    S[i] = -1; /*! initialize to no match */
  }

  int *rand_perm = rpermute(N);
  for (i = 0; i < N; ++i) {
    k = rand_perm[i];
    if (B[i][k].count > 0) { /*! queue is not empty */
      S[i] = k;
    }
  }
#endif
}

/*!
   Enhanced random strategy
   Uniformly and randomly pick d matchings from N! matchings
   pick the best from them
*/
void enhanced_random(int d) {
#ifdef ENHANCED_RANDOM_U
  int i, k;

  for (i = 0; i < N; i++) {
    S[i] = -1; /*! initialize to no match */
  }

  int max_weight = 0, current_weight = 0;
  int iteration = 0;
  int *rand_perm;

  int *tmpS = (int *)malloc(N * sizeof(int)); /*! temporary schedule */

  while (iteration < d) {
    rand_perm = rpermute(N); /*! generate a random permutation of 1..N */
    current_weight = 0; /*! reset the weight of current random permutation */

    for (i = 0; i < N; ++i) tmpS[i] = -1; /*! initialize to no match */

    for (i = 0; i < N; ++i) {
      k = rand_perm[i];
      if (B[i][k].count > 0) { /*! queue is not empty */
        tmpS[i] = k;
        current_weight += B[i][k].count;
      }
    }

    if (current_weight >
        max_weight) { /*! current matching outweighs the previous maximum */
      max_weight = current_weight;      /*! replace maximum weight */
      memcpy(S, tmpS, N * sizeof(int)); /*! replace best matching */
    }

    ++iteration;
  }

  free(tmpS); /*! release the memory of temporary schedule */
#endif
}

/*!
    This function merges two matchings
    It relies on certain global variables.
*/
int *merge(int *matchX, int *matchY) {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif

  assert(is_matching(matchX, N) && is_matching(matchY, N));

  Graph g;
  int i, k, oX, oY;

  g = NewGraph(2 * N); /* create an empty graph with 2N nodes */

  /*! construct the merged bipartite graph */
  /*! note that, the node index is started from 1 */
  for (i = 0; i < N; ++i) { /*! multi-graph is not supported */
    oX = matchX[i];
    oY = matchY[i];
    if (oX == oY) /*! parallel edges, only insert one */
      AddEdge(g, i + 1, oX + N + 1, 0);
    else {
      AddEdge(g, i + 1, oX + N + 1, 0);
      AddEdge(g, i + 1, oY + N + 1, 1);
    }
  }

#ifdef DEBUG
  printf("graph is created ...\n");
#endif

  int current_node;
  int weight;
  int unmatched_inputs = N;

  int *visited = (int *)malloc(N * sizeof(int));

  ARRAY_INIT(N, visited, -1); /*! mark all inputs as unvisited */

  Edge e;
  int iteration = 0;
  int *selected_match = (int *)malloc(N * sizeof(int));

  ARRAY_INIT(N, selected_match, 0); /*! marked match 0 as selected match */

  while (unmatched_inputs > 0) { /*! have unmatched inputs */

    for (i = 1; i <= N; ++i) /*! find the first unmatched input */
      if (Degree(g, i) > 0) break;

    current_node = i;
    weight = 0;
    while (Degree(g, current_node) > 0) {
      e = FirstEdge(g,
                    current_node); /*! get the first edge from current node */
      if (current_node <= N) {     /*! current is input */
        i = current_node - 1;
        if (visited[i] == -1) {   /*! not visited before */
          visited[i] = iteration; /*! marked as visited in this iteration */
          --unmatched_inputs;
        }
        k = e->endpoint - N - 1;  /*! get the output port */
      } else {                    /*! current is output */
        i = e->endpoint - 1;      /*! get the input port */
        k = current_node - N - 1; /*! get the output port */
      }

      if (e->label == 0)
        weight += B[i][k].count; /*! belong to match 0 */
      else
        weight -= B[i][k].count; /*! belong to match 1 */

      current_node = e->endpoint; /*! update current node */
      RemoveEdge(g, e);           /*! remove edge */
    }

    if (weight < 0) selected_match[iteration] = 1;

    ++iteration;
  }

  int *merged_matching = (int *)malloc(N * sizeof(int));
  int selected_match_index;
  ARRAY_INIT(N, merged_matching, -1);

  for (i = 0; i < N; ++i) {
    iteration = visited[i]; /* in which iteration this input is visited */
    selected_match_index = selected_match[iteration]; /* in the iteration which
                                                         match is selected */
    k = selected_match_index == 0 ? matchX[i] : matchY[i];
    merged_matching[i] = k;
  }

  assert(is_matching(merged_matching,
                     N)); /*! merged match should be a full matching */

  free(visited);
  free(selected_match);
  free(g);

  return merged_matching;
}

/*!
    This function implements Serena
*/
void Serena() {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif
#ifdef SERENA_U
  int i, j, k, pre_i;
  int *match = (int *)malloc(N * sizeof(int)); /*! the array format matching */
  int *output_match =
      (int *)malloc(N * sizeof(int)); /*! the array format matching */

  ARRAY_INIT(N, match, -1);        /*! mark all input as unmatched */
  ARRAY_INIT(N, output_match, -1); /*! mark all output as unmatched */

  /*! Step 1: greedily match in-out pairs which have arrival */
  for (i = 0; i < N; ++i) {
    k = A[i];
    if (k != -1 && k < N) { /*! arrival from i to k */
      if (output_match[k] == -1)
        output_match[k] = i;
      else { /*! multiple inputs want to go */
        pre_i = output_match[k];
        if (B[i][k].count >
            B[pre_i][k].count) { /*! current match is `better` */
          output_match[k] = i;
        }
      }
    }
  }

  for (k = 0; k < N; ++k) {
    i = output_match[k];
    if (i != -1) match[i] = k;
  }

  fix_matching(match, N); /*! round-robin match unmatched in-out pairs */

  int *merged_match = merge(S, match);
  ARRAY_COPY(N, merged_match, S);

  free(match);
  free(output_match);
  free(merged_match);
#endif
}

/*! variable round iSLIP */
void iSLIP_VR(int rounds) {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif
#ifdef ISLIP_VR_U
  int t, i;
  bool is_first_round;
  bool is_input_matched[P];
  int output_matched[P];
  int check[P];

  ARRAY_INIT(N, is_input_matched, false);
  ARRAY_INIT(N, output_matched, -1);
  if (!pointers_initialized) {
    pointers_initialized = true;
    ARRAY_INIT(N, Grant, 0);
    ARRAY_INIT(N, Accept, 0);
  }
  ARRAY_INIT(N, check, -1);
  ARRAY_INIT(N, S, -1);

  /* change from rounds + 1 to rounds */
  for (t = 0; t < rounds; ++t) {
    is_first_round = (t == 0);
    iSLIP_ONE_ROUND(N, B, S, is_input_matched, output_matched, check, Accept,
                    Grant, is_first_round);
  }
  /* comment the following line */
  /* assert(is_partial_matching(S,N)); */
#endif
}

void Arrival_iSLIP() {
#ifdef DEBUG
  FUNCTION_CALL_PRINT;
#endif
#ifdef ARRIVAL_ISLIP_U
  int t, i;
  bool is_first_round;
  bool is_input_matched[P];
  int output_matched[P];
  int check[P];

  ARRAY_INIT(N, is_input_matched, false);
  ARRAY_INIT(N, output_matched, -1);
  if (!pointers_initialized) { /* if first circle */
    pointers_initialized = true;
    ARRAY_INIT(N, Grant, 0);
    ARRAY_INIT(N, Accept, 0);
  }
  ARRAY_INIT(N, check, -1);
  ARRAY_INIT(N, S, -1);

  /* Arrival matching phase */
  ARRIVAL_MATCHING(N, A, B, S, is_input_matched, output_matched);

  /*! iSLIP phase */
  for (t = 0; t < log_ceil(N); ++t) {
    is_first_round = (t == 0);
    iSLIP_ONE_ROUND(N, B, S, is_input_matched, output_matched, check, Accept,
                    Grant, is_first_round);
  }

  /* comment the following line */
  assert(is_partial_matching(S, N));

#endif
}

/* */
void iLQF() {
  int t, i, k, c, j;
  bool is_input_matched[P];
  bool is_output_matched[P];
  int check[P];
  int rand_perm[P];

  ARRAY_INIT(N, is_input_matched, false);
  ARRAY_INIT(N, is_output_matched, false);

  ARRAY_INIT(N, check, -1);
  ARRAY_INIT(N, S, -1);

  for (t = 0; t < log_ceil(N) + 1; ++t) {
    /* Request & Grant Phase */
    for (k = 0; k < N; k++) {
      i = -1;
      check[k] = -1;
      if (!is_output_matched[k]) {
        rpermute_s(N, rand_perm);
        for (j = 0; j < N; j++) {
          c = rand_perm[j];
          /* pick the unmatched ingress with largest queue length */
          if (!is_input_matched[c] && B[c][k].count > 0) {
            if (i == -1)
              i = c;
            else if (B[c][k].count > B[i][k].count)
              i = c;
          }
        }
      }
      if (i != -1) check[k] = i;
    }
    /* Accept Phase */
    for (i = 0; i < N; i++) {
      k = -1;
      if (!is_input_matched[i]) {
        rpermute_s(N, rand_perm);
        for (j = 0; j < N; j++) {
          c = rand_perm[j];
          /* pick the unmatched egress with largest queue length */
          if (check[c] == i) {
            if (k == -1)
              k = c;
            else if (B[i][c].count > B[i][k].count)
              k = c;
          }
        }
      }
      if (k != -1) {
        is_input_matched[i] = true;
        is_output_matched[k] = true;
        S[i] = k;
      }
    }
  }
}
