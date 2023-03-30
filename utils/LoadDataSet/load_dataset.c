#include <stdio.h>

#define _TRAINING_SAMPLES_
// #define _TESTING_SAMPLES_

#define NUM_TRAINING_SAMPLES 4336
#define NUM_TESTING_SAMPLES 1082
#define NUM_FEATURES 43
#define NUM_CLASSES 6
#define DATA_TYPE float
#define CLASS_ID_TYPE char

typedef struct {
    DATA_TYPE features[NUM_FEATURES];
    CLASS_ID_TYPE classification_id;
} Point;

Point known_points1[NUM_TRAINING_SAMPLES] = {
#include "train.dat"
}; //{{{1,2,3,4,5},'a'}};

Point *known_points = known_points1;

Point new_points1[NUM_TESTING_SAMPLES] = {
#include "test.dat"
}; //{{{1,2,3,4,5},'a'}};

Point *new_points = new_points1;

int main() {
    int num_points = NUM_TESTING_SAMPLES;
    int num_features = NUM_FEATURES;

#ifdef _TRAINING_SAMPLES_
    for (int i = 0; i < num_points; i++) {
        printf("{");
        for (int j = 0; j < num_features; j++) {
            printf("%f,", known_points[i].features[j]);
        }
        printf("%d", known_points[i].classification_id);

        if (i != num_points - 1)
            printf(" },");
        else
            printf(" }");
    }
#endif
#ifdef _TESTING_SAMPLES_
    for (int i = 0; i < num_points; i++) {
        printf("{");
        for (int j = 0; j < num_features; j++) {
            printf("%f,", new_points1[i].features[j]);
        }
        printf("%d", new_points1[i].classification_id);

        if (i != num_points - 1)
            printf(" },");
        else
            printf(" }");
    }
#endif
    return 0;
}