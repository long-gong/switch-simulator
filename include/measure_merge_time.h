#ifndef MEASURE_MERGE_TIME_H
#define MEASURE_MERGE_TIME_H

#include <stdint.h> /*! for intptr_t */
#include <assert.h> /*! for assert */
#include <string.h> /*! for memcpy */
#include <time.h>
#include "common.h"
#include "graphtypes.h"
#include "util.h"

#define MAX_N 512
extern int WEIGHT[MAX_N][MAX_N];
extern int MATCHX[MAX_N];
extern int MATCHY[MAX_N];
extern int MATCHZ[MAX_N];

/* generate a random matrix */
#define RANDOM_MATRIX(MAX_V,N,M,MAT)                           \                                        
do{                                                            \
    int _i,_j;                                                 \
    int _w;                                                    \
    for (_i = 0;_i < N;++ _i)                                  \
    {                                                          \
        for (_j = 0;_j < M;++ _j)                              \
        {                                                      \
            _w = random_int(MAX_V) + 1; /* 1--MAX_V */         \
            MAT[_i][_j] = _w;                                  \
        }                                                      \
    }                                                          \
}while(0)

#define MERGE(N,B,matchX,matchY,matchZ)                                                                                          \
do{                                                                                                                              \
    Graph _g;                                                                                                                    \
    int _i, _k, _oX, _oY;                                                                                                        \
    _g = NewGraph(2 * N); /* create an empty graph with 2N nodes */                                                              \
                                                                                                                                 \
    /*! construct the merged bipartite graph */                                                                                  \
    /*! note that, the node index is started from 1 */                                                                           \
    for (_i = 0;_i < N; ++ _i)                                                                                                   \
    {/*! multi-graph is not supported */                                                                                         \
        _oX = matchX[_i];                                                                                                        \
        _oY = matchY[_i];                                                                                                        \
        if (_oX == _oY) /*! parallel edges, only insert one */                                                                   \
            AddEdge(_g,_i + 1,_oX + N + 1,0);                                                                                    \
        else                                                                                                                     \
        {                                                                                                                        \
          AddEdge(_g,_i + 1,_oX + N + 1,0);                                                                                      \
          AddEdge(_g,_i + 1,_oY + N + 1,1);                                                                                      \
        }                                                                                                                        \
                                                                                                                                 \
    }                                                                                                                            \
                                                                                                                                 \
                                                                                                                                 \
    int _current_node;                                                                                                           \
    int _weight;                                                                                                                 \
    int _unmatched_inputs = N;                                                                                                   \
                                                                                                                                 \
    int *_visited = (int *)malloc(N * sizeof(int));                                                                              \
                                                                                                                                 \
    ARRAY_INIT(N,_visited,-1); /*! mark all inputs as unvisited */                                                               \
                                                                                                                                 \
    Edge _e;                                                                                                                     \
    int _iteration = 0;                                                                                                          \
    int *_selected_match = (int *)malloc(N * sizeof(int));                                                                       \
                                                                                                                                 \
    ARRAY_INIT(N,_selected_match,0); /*! marked match 0 as selected match */                                                     \
                                                                                                                                 \
    while (_unmatched_inputs > 0)                                                                                                \
    {/*! have unmatched inputs */                                                                                                \
                                                                                                                                 \
        for (_i = 1;_i <= N;++ _i)/*! find the first unmatched input */                                                          \
            if (Degree(_g,_i) > 0) break;                                                                                        \
                                                                                                                                 \
        _current_node = _i;                                                                                                      \
        _weight = 0;                                                                                                             \
        while (Degree(_g,_current_node) > 0)                                                                                     \
        {                                                                                                                        \
            _e = FirstEdge(_g,_current_node); /*! get the first edge from current node */                                        \
            if (_current_node <= N)                                                                                              \
            {/*! current is input */                                                                                             \
                _i = _current_node - 1;                                                                                          \
                if (_visited[_i] == -1)                                                                                          \
                {/*! not visited before */                                                                                       \
                    _visited[_i] = _iteration; /*! marked as visited in this iteration */                                        \
                    -- _unmatched_inputs;                                                                                        \
                }                                                                                                                \
                _k = _e->endpoint - N - 1; /*! get the output port */                                                            \
            }                                                                                                                    \
            else                                                                                                                 \
            {/*! current is output */                                                                                            \
                _i = _e->endpoint - 1; /*! get the input port */                                                                 \
                _k = _current_node - N - 1; /*! get the output port */                                                          \
            }                                                                                                                    \
                                                                                                                                 \
            if (_e->label == 0) _weight += B[_i][_k]; /*! belong to match 0 */                                                   \
            else _weight -= B[_i][_k]; /*! belong to match 1 */                                                                  \
                                                                                                                                 \
            _current_node = _e->endpoint; /*! update current node */                                                             \
            RemoveEdge(_g,_e); /*! remove edge */                                                                                \
        }                                                                                                                        \
                                                                                                                                 \
        if (_weight < 0)                                                                                                         \
            _selected_match[_iteration] = 1;                                                                                     \
                                                                                                                                 \
        ++ _iteration;                                                                                                           \
    }                                                                                                                            \
                                                                                                                                 \
                                                                                                                                 \
    int _selected_match_index;                                                                                                   \
    /*ARRAY_INIT(N,matchZ,-1);*/                                                                                                     \
                                                                                                                                 \
    for (_i = 0;_i < N;++ _i)                                                                                                    \
    {                                                                                                                            \
        _iteration = _visited[_i]; /* in which iteration this input is visited */                                                \
        _selected_match_index = _selected_match[_iteration]; /* in the iteration which match is selected */                      \
        _k = _selected_match_index == 0?matchX[_i]:matchY[_i];                                                                   \
        matchZ[_i] = _k;                                                                                                         \
    }                                                                                                                            \
                                                                                                                                 \
                                                                                                                                 \
                                                                                                                                 \
    free(_visited);                                                                                                              \
    free(_selected_match);                                                                                                       \
    free(_g);                                                                                                                    \
}while(0)


int test_correctness();
int merge_time_test();
#endif