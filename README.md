# SWITCH SIMULATOR

Simulation codes for our switching paper:

[Long Gong](https://lgong30.github.io/), Paul Tune, Liang Liu, Sen Yang, and [Jun (Jim) Xu](https://www.cc.gatech.edu/home/jx/). 2017. [Queue-Proportional Sampling: A Better Approach to Crossbar Scheduling for Input-Queued Switches](https://www.cc.gatech.edu/home/jx/reprints/Gong%20et%20al.%20-%202017%20-%20Queue-Proportional%20Sampling%20A%20Better%20Approach%20to%20.pdf). Proc. ACM Meas. Anal. Comput. Syst. 1, 1, Article 3 (June 2017), 33 pages. DOI:https://doi.org/10.1145/3084440
  
## Repository Structure

    .
    ├── config_experiment_one.cfg        Confiuration for experiment one
    ├── config_experiment_two.cfg        Confiuration for experiment two  
    ├── cxx                              Directory for C++ code (required C++11 support)
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

1. iSLIP [[1](#1)]
+ [PIM][2]
+ [iLQF][3]
+ [MWM][4]


+ **.gitingore**: Git ignore file (specifying which kinds of files should be ignored while Git).
+ **config.cfg**: Configuration file, configure which algorithms need to be simulated.
+ **run_experiments.sh**: Shell script (not used).
+ **makefile**: Makefile for this project.
+ **switching-simulator.sublime-project**: Sublime Text project file.
+ **switching-simulator.sublime-workspace**: Sublime Text workspace file. 
+ **bin/**: Directory to store final executable file.
+ **build/**: Directory to store object files.
+ **cxx/**: Directory to store C++ source codes.
+ **include/**: Directory to store all include files
+ **src/**: Directory to store C source codes.

## USAGE

### Basic

1. Compile      
```bash
make CC=clang CXX=clang++
```
2. Run   
    bin/swith_simulator [-n `NUMBER OF PORTS`]

    Parameters in `[  ]` is optional. Without the optional parameters, it will run the simulation for the different switch schedulers on the four types of traffic, with the default number of ports `N = 32`. To  change the number of ports, simply add the optional parameters. More details can be obtained by running `bin/swith_simulator -h`. More options will added in the future version.

### Advanced

#### 1. Debug
Uncomment Line 25 of _config.cfg_. Note that, if you only want to print out debug information related to your codes, you can redefine another Macro in _config.cfg_, and use Macro if to trigger your debug information. _e.g.,_ you can define the following Macro in _config.cfg_.
```C++
#define MY_DEBUG
```
and in the source code, you can add the following Macro if condition sentence.
```C++
#ifdef MY_DEBUG
printf("[MY DEBUG INFO]: .... \n");
#endif
```

#### 2. Configurations
+ **which algorithm(s)** to be simulated: from the _config.cfg_ file, you can see, three experiments are defined, and each one corresponding to certain algorithms. For example, from the following lines of _config.cfg_ file, you can tell, **EXPERIMENT ONE** will trigger all the algorithms defined inside the Macro if (Actually not really.).  
```shell
#ifdef EXPERIMENT_ONE
#define SERENA_U 1
#define QPS_ISLIP_U 1
#define QPS_VR_U 1
#define ISLIP_VR_U 1
#define ILQF_U 1
#define QPS_SERENA_U 1

#define QPS_ARRIVAL_ISLIP_U 1
#define ARRIVAL_QPS_ISLIP_U 1
#define QPS_ARRIVAL_SERENA_U 1
#define ARRIVAL_QPS_SERENA_U 1
#define ARRIVAL_ISLIP_U 1
#define QPS_ARRIVAL_ALT_ISLIP_U 1
#define QPS_ARRIVAL_ALT_SERENA_U 1
#define MWM_U 1
#endif
```

Another control of which algorithms to be run is in **src/jq.c**, you can configure the definition of the variable _func\_list_, Line 85 for algorithms in experiment one, Line 173 for algorithms in experiment two, Line 216 for algorithms in experiment three. 

#### 3. Some important options
+ `-m INTEGER`: how many frames to be run (default: N * 500, here N is the number of input/output ports), _e.g.,_ `-m 32000`
+ `-l REAL`: the minimum (normalized) traffic load (default: 0.1), _e.g.,_ `-l 0.8`
+ `-L REAL`: the maximum (normalized) traffic load (default: 1.0), _e.g.,_
`-L 0.9`, note that which you set maximum traffic load to 1, it will automatically translate it to 0.95.
+ `-u REAL`: burst size

####  4. Output

Note that, by default, the results would be printed to the terminal but not to file, if you want to store them to file, you have at least two choices:
+ **Redirection**: `bin/swith_simulator [your options] >output.txt`. 
+ **tee**: `bin/swith_simulator [your options] | tee output.txt`, the advantage of this choice is that the results would be printed to the terminal and the file.


## Benchmark Switching Algorithms

1. `iSLIP`: the standard iSLIP algorithm,
2. `iSLIP_RPS`: rate proportional sampling iSLIP, uses an arrival matrix in the first round, 
3. `PIM`: parallel iterative matching,
4. `PIM_RPS`: rate proportional sampling parallel iterative matching,
5. `MWM`: maximum weighted matching,
6. `THRESHOLD`: threshold based scheduling, and
7. `THRESHOLD_RPS`: rate proportional sampling threshold based scheduling.

Newly added:  

1. `NAIVE_RANDOM`: uniformly and randomly pick one matching from all the possible matchings,
2. `ENHANCED_RANDOM`: uniformly and randomly pick `d` matchings from all the possible matchings, select the best one of them,  
3. `SERENA`: previous matching + arrival graph  
4. `iLQF`: iterative longest queue first

Strategies to be verified:

## Proposed Strategies

1. `QPS_iSLIP`: Queue-proportional sampling + iSLIP

    `Round one`: first uniformly and randomly sample k packets (cells) from each input ports (if less than k, then select all of them); then every input requests from the output where first sampled packet should go. Each output, receiving multiple requests grants the one with highest weight (queue length).

    `Rest ones`: general iSLIP
2. `QPS_PIM`: Queue-proportional sampling + PIM

    `Round one`: first uniformly and randomly sample k packets (cells) from each input ports (if less than k, then select all of them); then every input requests from the output where first sampled packet should go. Each output, receiving multiple requests grants the one with highest weight (queue length).

    `Rest ones`: general PIM
4. `QPS_Serena`: queue-proportional sampling + arrival merge



## Traffic Types

### Distribution

1. uniform,
2. diagonal,
3. log diagonal, and
4. quasi diagonal.

### Arrival Processes

1. Bernoulli,
2. Pareto bursts


## FIXES

### 1. modification of readgraph.c [by Paul Tune]

+ Line 7 and 10, where the **inttypes.h** library (Long changes it to **stdint.h** library) was added and gptr's type was changed to **intptr_t**, instead of **int**. This is because there are problems casting to the integer type for 64-bit machines, causing a segmentation fault. Changing it to a integer pointer fixed the problem. The simulator should be able to run on both 32 and 64 bit machines. [The original code of **readgraph.c** can be accessed from http://compgenomics.utsa.edu/SVMicrO%20Source%20Code/SVMicrO/ViennaRNA-1.7/RNAforester/src/readgraph.c]

+ Aside from that **stdio.h** and **stdlib.h** libraries were included the scripts requiring **malloc** and **printf** to avoid complains from the compiler. 

### 2. modification of wmatch.c [by Long Gong]

+ Line 23: Change the type of **gptr** from **int** to **intptr_t**. Paul suggested a modification from **int** (actually, in the original source code it is **void ***) to **intptr_t** to adapt to 64-bit machine, however, he seemed to forget to change the definition of **Weighted_Match** which still uses **int**, so it will cause a segmentation fault problem when running on 64-bit machines. 

## TODO

switching based on stale information (considering, multi-core, pipeline, parallel etc, e.g., 16 circles' computation without paying 16 * 1 circle's cost **remind Jim to add to proposal**)

## Acknowledgements

Note that some of these files have been modified from Bill Lin's original source code.

## References

[<a id="1">1</a>]: [McKeown, N.](http://yuba.stanford.edu/~nickm/), 1999. [The iSLIP scheduling algorithm for input-queued switches](https://www.cs.rutgers.edu/~sn624/552-F18/papers/islip.pdf). IEEE/ACM transactions on networking, 7(2), pp.188-201.

[2]: Anderson, T.E., Owicki, S.S., Saxe, J.B. and Thacker, C.P., 1993. [High-speed switch scheduling for local-area networks](https://dl.acm.org/doi/abs/10.1145/161541.161736). ACM Transactions on Computer Systems (TOCS), 11(4), pp.319-352.

[3]: [McKeown, N.W.](http://yuba.stanford.edu/~nickm/), 1995. [Scheduling algorithms for input-queued cell switches](http://yuba.stanford.edu/~nickm/papers/nickMcKeown_thesis.pdf) (Doctoral dissertation, University of California, Berkeley).

[4]: [McKeown, N., Mekkittikul](http://yuba.stanford.edu/~nickm/), A., Anantharam, V. and Walrand, J., 1999. [Achieving 100% throughput in an input-queued switch](http://yuba.stanford.edu/~nickm/papers/IEEE_COMM_V3.pdf). IEEE Transactions on Communications, 47(8), pp.1260-1267.