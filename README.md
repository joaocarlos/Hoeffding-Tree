# Hoeffding-Tree

> This repository is a copy of original Hoeffding-Tree made by @lm-sousa. It will de used for academic purpose in the context of High-performance Computing course at University of Porto 2022/2023 2S

A C/C++ HLS-ready implementation of a Hoeffding Tree.

Statistical storage in nodes is made through quantile estimation using assymetric signum functions. Derived from the work of Lin et al.

## Recomended HLS kernel

```C++
//#define USE_XILINX_AP_TYPES

#include "HoeffdingTree.hpp"

#define ATTRIBUTES 16
#define CLASSES 2
#define NODES 127
#define SAMPLE_ARRAY_SIZE 10000

typedef HoeffdingTree<Node<NodeData<float,      // datatype
                                    ATTRIBUTES, // Attributes
                                    CLASSES     // Classes
                                    >,
                           NODES // Node upperbound
                           >>
    Tree;

typedef Tree::data_t data_t;

template <class T> struct Prediction {
    typename T::class_index_t classification;
    typename T::data_t confidence;
};

template <class T> struct Sample {
    typename T::data_t data[T::_DataClass::N_Attributes];
    typename T::class_index_t classification;
    bool doSplitTrial;
};

extern "C" {

void krnl_Tree(Tree *tree, Sample<Tree> sample[SAMPLE_ARRAY_SIZE], Prediction<Tree> prediction[SAMPLE_ARRAY_SIZE], unsigned int bundleSize) {

 sample_loop: for (unsigned int i = 0; i < bundleSize; i++) {
  if (sample[i]->doSplitTrial) {
   std::tie(prediction[i]->classification, prediction[i]->confidence) =
    tree->train(sample[i]->data, sample[i]->classification, true);
  } else {
   std::tie(prediction[i]->classification, prediction[i]->confidence) =
    tree->infer(sample[i]->data);
  }
 }
}

}
```

## Profiling

The code include several profiling flags based on execution scenarios:

1. Offline Training and Testing
2. Offline Training and Online Training and Testing
3. Online Training and Testing

For each scenario we have applied optional code modifications in order do identify bottlenecks into the code incrementally. These flags can be combined in any form according to the compilation diretives described bellow.

```C
#define ITERATIONS 1

#ifndef _ISNAN_ISINF_
#define _ISNAN_ISINF_ 0
#endif

#ifndef _SGN_ALPHA_OPT_
#define _SGN_ALPHA_OPT_ 0
#endif

#ifndef _POW_OPT_
#define _POW_OPT_ 0
#endif
```

These directives are defined into the `Optimisation.hpp` file but it can also be modified though the Makefile together with the `SCENARIO` and `C_XX` optimisation flags, for instance:

```bash
make -e SCENARIO=3 SGN_ALPHA_OPT=0 POW_OPT=0 ISNAN_ISINF=1 C_XX_OPT_FLAGS="-O3"
```

The Makefile as the following targets:

```Makefile
.PHONY: clean
.PHONY: run
.PHONY: gprof
.PHONY: graph
.PHONY: clean-gprof
.PHONY: hyperfine
```

The target names are self explanatory but it present some dependencies. To run the `gprof` target user must have the `gprof` binaries installed on the target machine. The `graph` target depends on the [Gprof2Dot](https://pypi.org/project/gprof2dot/) and, the `hyperfine` needs the [hyperfine](https://github.com/sharkdp/hyperfine) benchmark tool. All tools are open-source and freely available.
