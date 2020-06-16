# SWITCH SIMULATOR

[![Build Status](https://travis-ci.org/long-gong/switch-simulator.svg?branch=master)](https://travis-ci.org/long-gong/switch-simulator)

Simulation codes for our switching paper:

[Long Gong](https://lgong30.github.io/), Paul Tune, Liang Liu, Sen Yang, and [Jun (Jim) Xu](https://www.cc.gatech.edu/home/jx/). 2017. [Queue-Proportional Sampling: A Better Approach to Crossbar Scheduling for Input-Queued Switches](https://www.cc.gatech.edu/home/jx/reprints/Gong%20et%20al.%20-%202017%20-%20Queue-Proportional%20Sampling%20A%20Better%20Approach%20to%20.pdf). Proc. ACM Meas. Anal. Comput. Syst. 1, 1, Article 3 (June 2017), 33 pages. DOI:https://doi.org/10.1145/3084440
  
## Repository Structure

    .
    ├── config_experiment_one.cfg        Confiuration for experiment one
    ├── config_experiment_two.cfg        Confiuration for experiment two  
    ├── cxx                              Directory for C++ code (C++11 support REQUIRED)
    │   └── random_variable.cpp             
    ├── include                          Directory for C header files
    │   ├── common.h
    │   ├── graphtypes.h
    │   ├── match.defs
    │   ├── measure_merge_time.h
    │   ├── pairs.c
    │   ├── pointer.c
    │   ├── random_variable.h
    │   ├── readgraph.c
    │   ├── schedule.h
    │   ├── term.c
    │   ├── unpairs.c
    │   ├── uthash.h
    │   ├── util.h
    │   ├── vector.h
    │   └── weighted_uniform_sampler.h
    ├── Makefile                           Makefile for this project
    ├── README.md                          This file
    └── src                                Directory for C (some codes use C89) source codes
        ├── common.c
        ├── delayed_schedule.c              
        ├── existing_schedule.c
        ├── fqps_based_schedules.c
        ├── glib.c
        ├── incoming_common.c
        ├── jq.c
        ├── measure_merge_time.c
        ├── qps_based_schedules.c
        ├── schedule.c
        ├── util.c
        ├── vector.c
        ├── weighted_uniform_sampler.c
        └── wmatch.c

## Benchmark Switching Algorithms (Selected)

Algorithms from literature:

1. **iSLIP** [[1](#1)]: the standard iSLIP algorithm,
2. **PIM** [[2](#2)]: parallel iterative matching,
3. **MWM** [[4](#4)]: maximum weighted matching,
4. **SERENA** [[5](#5)]: previous matching + arrival graph,
5. **iLQF** [[3](#3)]: iterative longest queue first, and
6. **THRESHOLD**: threshold based scheduling.

Early proposals (**NOT USED**):

1. **iSLIP_RPS**: rate proportional sampling iSLIP, uses an arrival matrix in the first round,
2. **PIM_RPS**: rate proportional sampling parallel iterative matching,
3. **THRESHOLD_RPS**: rate proportional sampling threshold based scheduling.

Final proposals (**USED IN OUR PAPER**):

1. **QPS-iSLIP** [[6](#6)]: Queue Proportional Sampling (QPS) augmented iSLIP
2. **QPS-SERENA** [[6](#7)]: Queue Proportional Sampling (QPS) augmented SERENA

## Usage

Compilation: `make` : two binaries will be generated if succeeded: `ss_experiment_one` and `ss_experiment_two` in folder `build`: more details, please refer to the [**Makefile**](./Makefile)

Execution of these binaries:

    usage: [options]

    -h		print usage
    -p		use Pareto distribution (default Bernoulli)
    -u average_burst_length		use geometric burst distribution (default: mean = N)
    -w		use skewed input loading (default Bernoulli)
    -f		set user flag 1
    -g		set user flag 2
    -a		set user flag 3
    -s		set user flag 4
    -b		set user flag 5
    -n size		specify N, number of input/output ports (default 32)
    -m frames	specify number of macroframes (default N*500)
    -l minimum_traffic_load	specify minimum traffic load (default 0.1, including)
    -L maximum_traffic_load	specify maximum traffic load (default 1.0, including)
    -S traffic_load_step	specify traffic load step (default 0.1)

## Traffic Modules

### Traffic Patterns

1. uniform,
2. diagonal,
3. log diagonal, and
4. quasi diagonal.

### Arrival Processes

1. Bernoulli,
2. Pareto bursts

## Fixes

1. Modification of readgraph.c [by Paul Tune]
   + Lines 7 and 10, where the **inttypes.h** library (Long changes it to **stdint.h** library) was added and gptr's type was changed to **intptr_t**, instead of **int**. This is because there are problems casting to the integer type for 64-bit machines, causing a segmentation fault. Changing it to a integer pointer fixed the problem. The simulator should be able to run on both 32 and 64 bit machines. The original code of **readgraph.c** can be accessed from http://compgenomics.utsa.edu/SVMicrO%20Source%20Code/SVMicrO/ViennaRNA-1.7/RNAforester/src/readgraph.c
   + Aside from that **stdio.h** and **stdlib.h** libraries were included the scripts requiring **malloc** and **printf** to avoid complains from the compiler. 
2. Modification of wmatch.c [by Long Gong]
   + Line 23: Change the type of **gptr** from **int** to **intptr_t**. Paul suggested a modification from **int** (actually, in the original source code it is **void ***) to **intptr_t** to adapt to 64-bit machine, however, he seemed to forget to change the definition of **Weighted_Match** which still uses **int**, so it will cause a segmentation fault problem when running on 64-bit machines. 

## Acknowledgements

This project was modified from [Dr. Bill Lin](http://cwcserv.ucsd.edu/~billlin/)'s original source codes.

## Authors

+ Long Gong long.github@gmail.com
+ Paul Tune

## References

[<a id="1">1</a>] [McKeown, N.](http://yuba.stanford.edu/~nickm/), 1999. [The iSLIP scheduling algorithm for input-queued switches](https://www.cs.rutgers.edu/~sn624/552-F18/papers/islip.pdf). IEEE/ACM transactions on networking, 7(2), pp.188-201.  
[<a id="2">2</a>] Anderson, T.E., Owicki, S.S., Saxe, J.B. and Thacker, C.P., 1993. [High-speed switch scheduling for local-area networks](https://dl.acm.org/doi/abs/10.1145/161541.161736). ACM Transactions on Computer Systems (TOCS), 11(4), pp.319-352.  
[<a id="3">3</a>] [McKeown, N.W.](http://yuba.stanford.edu/~nickm/), 1995. [Scheduling algorithms for input-queued cell switches](http://yuba.stanford.edu/~nickm/papers/nickMcKeown_thesis.pdf) (Doctoral dissertation, University of California, Berkeley).  
[<a id="4">4</a>] [McKeown, N., Mekkittikul](http://yuba.stanford.edu/~nickm/), A., Anantharam, V. and Walrand, J., 1999. [Achieving 100% throughput in an input-queued switch](http://yuba.stanford.edu/~nickm/papers/IEEE_COMM_V3.pdf). IEEE Transactions on Communications, 47(8), pp.1260-1267.  
[<a id="5">5</a>] Giaccone, P., Prabhakar, B. and [Shah, D.](https://devavrat.mit.edu/), 2003. [Randomized scheduling algorithms for high-aggregate bandwidth switches](https://ieeexplore.ieee.org/document/1197700). IEEE Journal on Selected Areas in Communications, 21(4), pp.546-559.
[<a id="6">6</a>] Gong, L., Tune, P., Liu, L., Yang, S. and Xu, J., 2017. [Queue-Proportional Sampling: A Better Approach to Crossbar Scheduling for Input-Queued Switches](https://www.cc.gatech.edu/home/jx/reprints/Gong%20et%20al.%20-%202017%20-%20Queue-Proportional%20Sampling%20A%20Better%20Approach%20to%20.pdf). Proceedings of the ACM on Measurement and Analysis of Computing Systems, 1(1), pp.1-33.
