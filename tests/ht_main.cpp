#include <cstddef>
#include <float.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <math.h> // included for isnan
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

#include "../src/HoeffdingTree.hpp"
#include "../src/Node.hpp"
#include "../src/TypeChooser.hpp"
#include "Optimisation.hpp"

// #define _PLAIN_DATASET_
#define _INPUT_FILE_TRAIN_DATASET_
#define _INPUT_FILE_TEST_DATASET_

/*
 * Scenario 1: Offline Training and Testing without online training
 *   #define _OFFLINE_TRAINING_
 *   #define _TESTING_
 * Scenario 2: Offline Training and Testing with online training
 *   #define _OFFLINE_TRAINING_
 *   #define _ONLINE_TRAINING_AND_TESTING_
 * Scenario 3: Online Training and Testing
 *   #define _ALL_ONLINE_TRAINING_AND_TESTING_
 */

#ifndef SCENARIO
#define SCENARIO 1
#endif

/*
 * Scenario is defined in Makefile though -DSCENARIO=1,2,3
 */
#if SCENARIO == 1
#define _OFFLINE_TRAINING_
#define _TESTING_
#elif SCENARIO == 2
#define _OFFLINE_TRAINING_
#define _ONLINE_TRAINING_AND_TESTING_
#elif SCENARIO == 3
#define _ALL_ONLINE_TRAINING_AND_TESTING_
#endif

#define NUM_TRAINING_SAMPLES 4336
#define NUM_TESTING_SAMPLES 1082
#define NUM_FEATURES 43
#define NUM_CLASSES 6

#ifndef _ACCURACY_CHECK_
#define _ACCURACY_CHECK_
#endif

typedef char CLASS_ID_TYPE;
typedef float DATA_TYPE;

#define MAX_FP_VAL FLT_MAX
#define MIN_FP_VAL FLT_MIN

#ifdef _INPUT_FILE_TRAIN_DATASET_
DATA_TYPE trainDataset[NUM_TRAINING_SAMPLES][NUM_FEATURES + 1] = {
#include "dataset_train.txt"
}; //{{{1,2,3,4,5},'a'}};
#endif

#ifdef _INPUT_FILE_TEST_DATASET_
DATA_TYPE testDataset[NUM_TESTING_SAMPLES][NUM_FEATURES + 1] = {
#include "dataset_test.txt"
}; //{{{1,2,3,4,5},'a'}};
#endif

/*
 * Determine the min and max values for each feature for a
 * set of points.
 * minmax(min, max, NUM_TRAINING_SAMPLES, irisDataset, NUM_FEATURES)
 */
void inline minmax(DATA_TYPE *__restrict min, DATA_TYPE *__restrict max,
                   int num_points, DATA_TYPE known_points[][NUM_FEATURES + 1],
                   int num_features) {

    for (int j = 0; j < num_features; j++) {
        min[j] = MAX_FP_VAL;
        max[j] = MIN_FP_VAL; // MAX in float.h
        // printf("%e, %e\n", MIN_FP_VAL, MAX_FP_VAL);
    }

    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_features; j++) {
            if (known_points[i][j] < min[j])
                min[j] = known_points[i][j];
            if (known_points[i][j] > max[j])
                max[j] = known_points[i][j];
        }
    }
}

/*
 * Normalize the features of each point using minmax normalization.
 * minmax_normalize(min, max, NUM_TRAINING_SAMPLES, irisDataSet, NUM_FEATURES)
 */
void inline minmax_normalize(DATA_TYPE *__restrict min,
                             DATA_TYPE *__restrict max, int num_points,
                             DATA_TYPE points[][NUM_FEATURES + 1],
                             int num_features) {

    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_features; j++) {
            DATA_TYPE nfeature =
                (DATA_TYPE)((points[i][j] - min[j]) / (max[j] - min[j]));
#if _ISNAN_ISINF_ == 0
            // in case the normalization returns a NaN or INF
            if (isnan(nfeature))
                nfeature = (DATA_TYPE)0.0;
            else if (isinf(nfeature))
                nfeature = (DATA_TYPE)1.0;
#endif
            points[i][j] = nfeature;
        }
        // show_point(points[i], num_features); 
    }
}

/*
 * Normalize the features of each point using minmax normalization.
 * minmax_normalize_sample(min, max, inputSample, NUM_FEATURES)
 */
void inline minmax_normalize_sample(DATA_TYPE *__restrict min,
                                    DATA_TYPE *__restrict max,
                                    DATA_TYPE *__restrict point,
                                    int num_features) {
    for (int j = 0; j < num_features; j++) {
        DATA_TYPE nfeature =
            (DATA_TYPE)((point[j] - min[j]) / (max[j] - min[j]));

#if _ISNAN_ISINF_ == 0
        // in case the normalization returns a NaN or INF
        if (isnan(nfeature))
            nfeature = (DATA_TYPE)0.0;
        else if (isinf(nfeature))
            nfeature = (DATA_TYPE)1.0;
#endif
        point[j] = nfeature;
    }
    // show_point(points[i], num_features);  
}

DATA_TYPE min[NUM_FEATURES];
DATA_TYPE max[NUM_FEATURES];

int main() {
    std::cout << "############################################################"
              << std::endl;
    std::cout << "Starting Test Scenario " << SCENARIO << std::endl;
    std::cout << "############################################################"
              << std::endl;

    minmax(min, max, NUM_TRAINING_SAMPLES, trainDataset, NUM_FEATURES);
#if SCENARIO == 1 || SCENARIO == 2
    minmax_normalize(min, max, NUM_TRAINING_SAMPLES, trainDataset,
                     NUM_FEATURES);
#endif
    // Run tests
    typedef HoeffdingTree<Node<NodeData<DATA_TYPE, NUM_FEATURES, NUM_CLASSES>>>
        Tree;
    typedef typename Tree::sample_count_t sample_count_t;

    Tree tree(1, 0.01, 0.05); // Default paper parameters
    bool doSplitTrial = true;
    const sample_count_t N_Training_Samples = NUM_TRAINING_SAMPLES;
    const sample_count_t N_Testing_Samples = NUM_TESTING_SAMPLES;

#ifdef _ACCURACY_CHECK_
    Tree::class_index_t classification;
    Tree::data_t confidence;
    int wrong_classification = 0;
#endif

#ifdef _OFFLINE_TRAINING_
    printf("Hoeffding Tree - Offline Training\n");
    for (int i = 0; i < ITERATIONS; i++) {
#ifdef _ACCURACY_CHECK_
        wrong_classification = 0;
#endif
        for (sample_count_t i = 0; i < N_Training_Samples; i++) {
#ifdef _ACCURACY_CHECK_
            std::tie(classification, confidence) = tree.train(
                trainDataset[i], trainDataset[i][NUM_FEATURES], doSplitTrial);

            if (classification != trainDataset[i][NUM_FEATURES])
                wrong_classification++;
#else
            tree.train(trainDataset[i], trainDataset[i][NUM_FEATURES],
                       doSplitTrial);
#endif
        }
#ifdef _ACCURACY_CHECK_
        if (i == ITERATIONS - 1) {
            float accuracy = wrong_classification / (float)N_Training_Samples;
            std::cout << "Accuracy: " << accuracy * 100 << "%" << std::endl;
        }
#endif
    }
#endif

#ifdef _ONLINE_TRAINING_AND_TESTING_
    printf("Hoeffding Tree - Online Training and Testing\n");
    for (int i = 0; i < ITERATIONS; i++) {

#ifdef _ACCURACY_CHECK_
        wrong_classification = 0;
#endif
        for (sample_count_t i = 0; i < N_Testing_Samples; i++) {
            minmax_normalize_sample(min, max, testDataset[i], NUM_FEATURES);
#ifdef _ACCURACY_CHECK_
            std::tie(classification, confidence) = tree.infer(testDataset[i]);
            if (classification != testDataset[i][NUM_FEATURES])
                wrong_classification++;
#else
            tree.infer(testDataset[i]);

#endif
            tree.train(testDataset[i], testDataset[i][NUM_FEATURES],
                       doSplitTrial);
        }

#ifdef _ACCURACY_CHECK_
        if (i == ITERATIONS - 1) {
            float accuracy = wrong_classification / (float)N_Testing_Samples;
            std::cout << "Accuracy: " << accuracy * 100 << "%" << std::endl;
        }
#endif
    }
#endif

#ifdef _ALL_ONLINE_TRAINING_AND_TESTING_
    printf("Hoeffding Tree - All Online Training and Testing\n");
    for (int i = 0; i < ITERATIONS; i++) {

#ifdef _ACCURACY_CHECK_
        wrong_classification = 0;
#endif
        for (sample_count_t i = 0; i < N_Training_Samples; i++) {
            minmax_normalize_sample(min, max, trainDataset[i], NUM_FEATURES);
#ifdef _ACCURACY_CHECK_
            std::tie(classification, confidence) = tree.infer(trainDataset[i]);
            if (classification != trainDataset[i][NUM_FEATURES])
                wrong_classification++;
#else
            tree.infer(trainDataset[i]);
#endif
            tree.train(trainDataset[i], trainDataset[i][NUM_FEATURES],
                       doSplitTrial);
        }

        for (sample_count_t i = 0; i < N_Testing_Samples; i++) {
            minmax_normalize_sample(min, max, testDataset[i], NUM_FEATURES);
#ifdef _ACCURACY_CHECK_
            std::tie(classification, confidence) = tree.infer(testDataset[i]);
            if (classification != testDataset[i][NUM_FEATURES])
                wrong_classification++;

#else
            tree.infer(testDataset[i]);
#endif
            tree.train(testDataset[i], testDataset[i][NUM_FEATURES],
                       doSplitTrial);
        }

#ifdef _ACCURACY_CHECK_
        if (i == ITERATIONS - 1) {
            float accuracy = wrong_classification /
                             (float)(N_Training_Samples + N_Testing_Samples);
            std::cout << "Accuracy: " << accuracy * 100 << "%" << std::endl;
        }
#endif
    }
#endif

#ifdef _TESTING_
    printf("Hoeffding Tree - Testing\n");
    for (int i = 0; i < ITERATIONS; i++) {
#ifdef _ACCURACY_CHECK_
        wrong_classification = 0;
#endif
        for (sample_count_t i = 0; i < N_Testing_Samples; i++) {
            minmax_normalize_sample(min, max, testDataset[i], NUM_FEATURES);
#ifdef _ACCURACY_CHECK_
            std::tie(classification, confidence) = tree.infer(testDataset[i]);
            if (classification != testDataset[i][NUM_FEATURES])
                wrong_classification++;
#else
            tree.infer(testDataset[i]);
#endif
        }

#ifdef _ACCURACY_CHECK_
        if (i == ITERATIONS - 1) {
            float accuracy = wrong_classification / (float)N_Testing_Samples;
            std::cout << "Accuracy: " << accuracy * 100 << "%" << std::endl;
        }
#endif
    }
#endif
    std::cout << "############################################################"
              << std::endl;
    std::cout << "Finish Test Scenario " << SCENARIO << std::endl;
    std::cout << "############################################################"
              << std::endl;
    return 0;
}