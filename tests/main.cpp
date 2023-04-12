#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <math.h> // included for isnan

//#define USE_XILINX_AP_TYPES
//#include "ap_fixed.h"

#ifndef USE_XILINX_AP_TYPES
#pragma clang diagnostic warning "-Wall"
#pragma clang diagnostic warning "-Wextra"
#endif

#include "../src/BinaryTree.hpp"
#include "../src/HoeffdingTree.hpp"
#include "../src/JsonExporter.hpp"
#include "../src/Node.hpp"
#include "../src/TypeChooser.hpp"
#include "Tester.hpp"

#define _INPUT_FILE_TRAIN_DATASET_
#define _INPUT_FILE_TEST_DATASET_
// #define _PLAIN_DATASET_
#define _GENERATE_TREE_JSON_
#define _OFFLINE_TRAINING_
// #define _ONLINE_TRAINING_
#define _TESTING_
#define _ACCURACY_CHECK_

#define NUM_TRAINING_SAMPLES 4336
#define NUM_TESTING_SAMPLES 1082
#define NUM_FEATURES 43
#define NUM_CLASSES 6
#define DATA_TYPE float
#define CLASS_ID_TYPE char

#define MAX_FP_VAL 0
#define MIN_FP_VAL 0

#ifdef _INPUT_FILE_TRAIN_DATASET_
   DATA_TYPE trainDataset[NUM_TRAINING_SAMPLES][NUM_FEATURES+1] = {
      #include "dataset_train.txt"
   }; //{{{1,2,3,4,5},'a'}};
#endif

#ifdef _INPUT_FILE_TEST_DATASET_ 
   DATA_TYPE testDataset[NUM_TESTING_SAMPLES][NUM_FEATURES+1] = {
      #include "dataset_test.txt"
   }; //{{{1,2,3,4,5},'a'}};
#endif

#ifdef _PLAIN_DATASET_
   float irisDataset[150][5] = {
    {5.1, 3.5, 1.4, 0.2, 0}, {4.9, 3.0, 1.4, 0.2, 0}, {7.0, 3.2, 4.7, 1.4, 1},
    {6.4, 3.2, 4.5, 1.5, 1}, {6.3, 3.3, 6.0, 2.5, 2}, {5.8, 2.7, 5.1, 1.9, 2},
    {5.3, 3.7, 1.5, 0.2, 0}, {5.0, 3.3, 1.4, 0.2, 0}, {6.9, 3.1, 4.9, 1.5, 1},
    {5.5, 2.3, 4.0, 1.3, 1}, {7.1, 3.0, 5.9, 2.1, 2}, {6.3, 2.9, 5.6, 1.8, 2},
    {4.7, 3.2, 1.3, 0.2, 0}, {4.6, 3.1, 1.5, 0.2, 0}, {6.5, 2.8, 4.6, 1.5, 1},
    {5.7, 2.8, 4.5, 1.3, 1}, {6.5, 3.0, 5.8, 2.2, 2}, {7.6, 3.0, 6.6, 2.1, 2},
    {5.0, 3.6, 1.4, 0.2, 0}, {5.4, 3.9, 1.7, 0.4, 0}, {6.3, 3.3, 4.7, 1.6, 1},
    {4.9, 2.4, 3.3, 1.0, 1}, {4.9, 2.5, 4.5, 1.7, 2}, {7.3, 2.9, 6.3, 1.8, 2},
    {4.6, 3.4, 1.4, 0.3, 0}, {5.0, 3.4, 1.5, 0.2, 0}, {6.6, 2.9, 4.6, 1.3, 1},
    {5.2, 2.7, 3.9, 1.4, 1}, {6.7, 2.5, 5.8, 1.8, 2}, {7.2, 3.6, 6.1, 2.5, 2},
    {4.4, 2.9, 1.4, 0.2, 0}, {4.9, 3.1, 1.5, 0.1, 0}, {5.0, 2.0, 3.5, 1.0, 1},
    {5.9, 3.0, 4.2, 1.5, 1}, {6.5, 3.2, 5.1, 2.0, 2}, {6.4, 2.7, 5.3, 1.9, 2},
    {5.4, 3.7, 1.5, 0.2, 0}, {4.8, 3.4, 1.6, 0.2, 0}, {6.0, 2.2, 4.0, 1.0, 1},
    {6.1, 2.9, 4.7, 1.4, 1}, {6.8, 3.0, 5.5, 2.1, 2}, {5.7, 2.5, 5.0, 2.0, 2},
    {4.8, 3.0, 1.4, 0.1, 0}, {4.3, 3.0, 1.1, 0.1, 0}, {5.6, 2.9, 3.6, 1.3, 1},
    {6.7, 3.1, 4.4, 1.4, 1}, {5.8, 2.8, 5.1, 2.4, 2}, {6.4, 3.2, 5.3, 2.3, 2},
    {5.8, 4.0, 1.2, 0.2, 0}, {5.7, 4.4, 1.5, 0.4, 0}, {5.6, 3.0, 4.5, 1.5, 1},
    {5.8, 2.7, 4.1, 1.0, 1}, {6.5, 3.0, 5.5, 1.8, 2}, {7.7, 3.8, 6.7, 2.2, 2},
    {5.4, 3.9, 1.3, 0.4, 0}, {5.1, 3.5, 1.4, 0.3, 0}, {6.2, 2.2, 4.5, 1.5, 1},
    {5.6, 2.5, 3.9, 1.1, 1}, {7.7, 2.6, 6.9, 2.3, 2}, {6.0, 2.2, 5.0, 1.5, 2},
    {5.7, 3.8, 1.7, 0.3, 0}, {5.1, 3.8, 1.5, 0.3, 0}, {5.9, 3.2, 4.8, 1.8, 1},
    {6.1, 2.8, 4.0, 1.3, 1}, {6.9, 3.2, 5.7, 2.3, 2}, {5.6, 2.8, 4.9, 2.0, 2},
    {5.4, 3.4, 1.7, 0.2, 0}, {5.1, 3.7, 1.5, 0.4, 0}, {6.3, 2.5, 4.9, 1.5, 1},
    {6.1, 2.8, 4.7, 1.2, 1}, {7.7, 2.8, 6.7, 2.0, 2}, {6.3, 2.7, 4.9, 1.8, 2},
    {4.6, 3.6, 1.0, 0.2, 0}, {5.1, 3.3, 1.7, 0.5, 0}, {6.4, 2.9, 4.3, 1.3, 1},
    {6.6, 3.0, 4.4, 1.4, 1}, {6.7, 3.3, 5.7, 2.1, 2}, {7.2, 3.2, 6.0, 1.8, 2},
    {4.8, 3.4, 1.9, 0.2, 0}, {5.0, 3.0, 1.6, 0.2, 0}, {6.8, 2.8, 4.8, 1.4, 1},
    {6.7, 3.0, 5.0, 1.7, 1}, {6.2, 2.8, 4.8, 1.8, 2}, {6.1, 3.0, 4.9, 1.8, 2},
    {5.0, 3.4, 1.6, 0.4, 0}, {5.2, 3.5, 1.5, 0.2, 0}, {6.0, 2.9, 4.5, 1.5, 1},
    {5.7, 2.6, 3.5, 1.0, 1}, {6.4, 2.8, 5.6, 2.1, 2}, {7.2, 3.0, 5.8, 1.6, 2},
    {5.2, 3.4, 1.4, 0.2, 0}, {4.7, 3.2, 1.6, 0.2, 0}, {5.5, 2.4, 3.8, 1.1, 1},
    {5.5, 2.4, 3.7, 1.0, 1}, {7.4, 2.8, 6.1, 1.9, 2}, {7.9, 3.8, 6.4, 2.0, 2},
    {4.8, 3.1, 1.6, 0.2, 0}, {5.4, 3.4, 1.5, 0.4, 0}, {5.8, 2.7, 3.9, 1.2, 1},
    {6.0, 2.7, 5.1, 1.6, 1}, {6.4, 2.8, 5.6, 2.2, 2}, {6.3, 2.8, 5.1, 1.5, 2},
    {5.2, 4.1, 1.5, 0.1, 0}, {5.5, 4.2, 1.4, 0.2, 0}, {5.4, 3.0, 4.5, 1.5, 1},
    {6.0, 3.4, 4.5, 1.6, 1}, {6.1, 2.6, 5.6, 1.4, 2}, {7.7, 3.0, 6.1, 2.3, 2},
    {4.9, 3.1, 1.5, 0.2, 0}, {5.0, 3.2, 1.2, 0.2, 0}, {6.7, 3.1, 4.7, 1.5, 1},
    {6.3, 2.3, 4.4, 1.3, 1}, {6.3, 3.4, 5.6, 2.4, 2}, {6.4, 3.1, 5.5, 1.8, 2},
    {5.5, 3.5, 1.3, 0.2, 0}, {4.9, 3.6, 1.4, 0.1, 0}, {5.6, 3.0, 4.1, 1.3, 1},
    {5.5, 2.5, 4.0, 1.3, 1}, {6.0, 3.0, 4.8, 1.8, 2}, {6.9, 3.1, 5.4, 2.1, 2},
    {4.4, 3.0, 1.3, 0.2, 0}, {5.1, 3.4, 1.5, 0.2, 0}, {5.5, 2.6, 4.4, 1.2, 1},
    {6.1, 3.0, 4.6, 1.4, 1}, {6.7, 3.1, 5.6, 2.4, 2}, {6.9, 3.1, 5.1, 2.3, 2},
    {5.0, 3.5, 1.3, 0.3, 0}, {4.5, 2.3, 1.3, 0.3, 0}, {5.8, 2.6, 4.0, 1.2, 1},
    {5.0, 2.3, 3.3, 1.0, 1}, {5.8, 2.7, 5.1, 1.9, 2}, {6.8, 3.2, 5.9, 2.3, 2},
    {4.4, 3.2, 1.3, 0.2, 0}, {5.0, 3.5, 1.6, 0.6, 0}, {5.6, 2.7, 4.2, 1.3, 1},
    {5.7, 3.0, 4.2, 1.2, 1}, {6.7, 3.3, 5.7, 2.5, 2}, {6.7, 3.0, 5.2, 2.3, 2},
    {5.1, 3.8, 1.9, 0.4, 0}, {4.8, 3.0, 1.4, 0.3, 0}, {5.7, 2.9, 4.2, 1.3, 1},
    {6.2, 2.9, 4.3, 1.3, 1}, {6.3, 2.5, 5.0, 1.9, 2}, {6.5, 3.0, 5.2, 2.0, 2},
    {5.1, 3.8, 1.6, 0.2, 0}, {4.6, 3.2, 1.4, 0.2, 0}, {5.1, 2.5, 3.0, 1.1, 1},
    {5.7, 2.8, 4.1, 1.3, 1}, {6.2, 3.4, 5.4, 2.3, 2}, {5.9, 3.0, 5.1, 1.8, 2}};
#endif

// ap_fixed<32, 8>

typedef HoeffdingTree<Node<NodeData<float, 4, 3>>> FixedTree;

FixedTree::data_t fixedDataset[NUM_TRAINING_SAMPLES][NUM_FEATURES+1];

FixedTree::data_t scale(FixedTree::data_t a) { return a * 8; }

/*
* Determine the min and max values for each feature for a 
* set of points.
* minmax(min, max, NUM_TRAINING_SAMPLES, irisDataset, NUM_FEATURES)
*/
void minmax(DATA_TYPE *min, DATA_TYPE *max, int num_points, DATA_TYPE known_points[][NUM_FEATURES+1], int num_features) {

    for (int j = 0; j < num_features; j++) {
        min[j] = MAX_FP_VAL;
        max[j] = MIN_FP_VAL;
        //printf("%e, %e\n", MIN_FP_VAL, MAX_FP_VAL);
    }

    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_features; j++) {
            if(known_points[i][j] < min[j])
                min[j] = known_points[i][j];
            if(known_points[i][j] > max[j])
                max[j] = known_points[i][j];
        }
    }
}

/*
* Normalize the features of each point using minmax normalization.
* minmax_normalize(min, max, NUM_TRAINING_SAMPLES, irisDataSet, NUM_FEATURES)
*/
void minmax_normalize(DATA_TYPE *min, DATA_TYPE *max, int num_points, DATA_TYPE points[][NUM_FEATURES+1], int num_features) {
    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_features; j++) {
            DATA_TYPE nfeature = (DATA_TYPE) ((points[i][j] - min[j])/(max[j] - min[j]));

            // in case the normalization returns a NaN or INF
            if(isnan(nfeature)) nfeature = (DATA_TYPE) 0.0;
            else if(isinf(nfeature)) nfeature = (DATA_TYPE) 1.0;

            points[i][j] = nfeature;
        }
        //show_point(points[i], num_features); 
    }
}


/*
* Normalize the features of each point using minmax normalization.
* minmax_normalize_sample(min, max, NUM_TRAINING_SAMPLES, inputSample, NUM_FEATURES)
*/
void minmax_normalize_sample(DATA_TYPE *min, DATA_TYPE *max, int num_points, DATA_TYPE *point, int num_features) {
    for (int j = 0; j < num_features; j++) {
        DATA_TYPE nfeature = (DATA_TYPE) ((point[j] - min[j])/(max[j] - min[j]));

        // in case the normalization returns a NaN or INF
        if(isnan(nfeature)) nfeature = (DATA_TYPE) 0.0;
        else if(isinf(nfeature)) nfeature = (DATA_TYPE) 1.0;

        point[j] = nfeature;
    }
        //show_point(points[i], num_features); 
}

int main() {

    DATA_TYPE min[NUM_FEATURES];
    DATA_TYPE max[NUM_FEATURES];

    minmax(min, max, NUM_TRAINING_SAMPLES, trainDataset, NUM_FEATURES);
    minmax_normalize(min, max, NUM_TRAINING_SAMPLES, trainDataset, NUM_FEATURES);

    // Run tests
    Tester ts;

#ifdef __HOEFFDING_TREE_HPP__
    
    #ifdef _OFFLINE_TRAINING_
        ts.addTest("Hoeffding Tree - Offline Training", []() {
            typedef HoeffdingTree<Node<NodeData<DATA_TYPE, NUM_FEATURES, NUM_CLASSES>>> Tree;
            typedef typename Tree::sample_count_t sample_count_t;

            Tree tree(1, 0.01, 0.05); // Default paper parameters 
            bool doSplitTrial = true;
            const sample_count_t N_Samples = NUM_TRAINING_SAMPLES;

            #ifdef _ACCURACY_CHECK_
                Tree::class_index_t classification;
                Tree::data_t confidence;            
                int wrong_classification = 0;
            #endif

            for (sample_count_t i = 0; i < N_Samples; i++) {
                #ifdef _ACCURACY_CHECK_
                    std::tie(classification, confidence) =
                    tree.train(trainDataset[i], trainDataset[i][NUM_FEATURES], doSplitTrial);
                
                    if(classification != testDataset[i][NUM_FEATURES])
                        wrong_classification++;
                #else 
                    tree.train(trainDataset[i], trainDataset[i][NUM_FEATURES], doSplitTrial);
                #endif
            }

            #ifdef _ACCURACY_CHECK_
                float accuracy = wrong_classification / (float) N_Samples;
                std::cout<<"Accuracy: " << accuracy*100 <<"%" << std::endl;
            #endif

            // tree.getRootNode()->getData().evaluateSplit();
            #ifdef _GENERATE_TREE_JSON_
                Tree treeCopy(tree.getR(), tree.getSigma(), tree.getTau());

                JsonExporter::copyNode(tree, treeCopy, tree.getRootNode(),
                                    treeCopy.getRootNode());

                JsonExporter::inferDataset(treeCopy, trainDataset, N_Samples);

                std::string result = JsonExporter::treeToJson(treeCopy);

                std::ofstream file("offline_training_tree.json");
                file << result;
                file.close();
            #endif

            return std::make_pair(true, "Will always return true");
        });
    #endif

    #ifdef _ONLINE_TRAINING_
        // TODO: accuracy check and JSON export
        ts.addTest("Hoeffding Tree - Online Training and Test", []() {
            typedef HoeffdingTree<Node<NodeData<float, 4, 3>>> Tree;
            typedef typename Tree::sample_count_t sample_count_t;

            Tree tree(1, 0.01, 0.05);
            bool doSplitTrial = true;
            const sample_count_t N_Samples = NUM_TRAINING_SAMPLES;

            for (sample_count_t i = 0; i < N_Samples; i++) {
                tree.train(testDataset[i], testDataset[i][4], doSplitTrial);
            }

            // tree.getRootNode()->getData().evaluateSplit();

            return std::make_pair(true, "Will always return true");
        });
    #endif
    #ifdef _TESTING_
        ts.addTest("Hoeffding Tree - Test", []() {
            typedef HoeffdingTree<Node<NodeData<DATA_TYPE, NUM_FEATURES, NUM_CLASSES>>> Tree;
            typedef typename Tree::sample_count_t sample_count_t;

            Tree tree(1, 0.01, 0.05);
            bool doSplitTrial = true;
            const sample_count_t N_Samples = NUM_TESTING_SAMPLES;
            
            #ifdef _ACCURACY_CHECK_
                Tree::class_index_t classification;
                Tree::data_t confidence;
                int wrong_classification = 0;
            #endif

            for (sample_count_t i = 0; i < N_Samples; i++) {
                #ifdef _ACCURACY_CHECK_
                    std::tie(classification, confidence) =
                    tree.infer(testDataset[i]);
                    if(classification != testDataset[i][NUM_FEATURES])
                        wrong_classification++;
                #else 
                    tree.infer(testDataset[i]);
                #endif
            }

            #ifdef _ACCURACY_CHECK_
                float accuracy = wrong_classification / (float) N_Samples;
                std::cout<<"Accuracy: " << accuracy*100 <<"%" << std::endl;
            #endif

            #ifdef _GENERATE_TREE_JSON_
                Tree treeCopy(tree.getR(), tree.getSigma(), tree.getTau());

                JsonExporter::copyNode(tree, treeCopy, tree.getRootNode(),
                                    treeCopy.getRootNode());

                JsonExporter::inferDataset(treeCopy, testDataset, N_Samples);

                std::string result = JsonExporter::treeToJson(treeCopy);

                std::ofstream file("test_tree.json");
                file << result;
                file.close();
            #endif

            // tree.getRootNode()->getData().evaluateSplit();

            return std::make_pair(true, "Will always return true");
        });
    #endif
#endif
    return ts.runTestSuite(true, false);
}
