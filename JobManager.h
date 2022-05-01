//
// Created by shirayarhi on 01/05/2022.
//

#ifndef EX3_JOBMANAGER_H
#define EX3_JOBMANAGER_H
#include <armadillo>
#include <vector>
#include <mutex>
#include <atomic>
#include "MapReduceClient.h"
#include "ThreadContext.h"
#include "demo/Barrier/Barrier.h"

class JobManager{


public:
    vector<IntermediateVec> *threadWorkspaces;
    OutputVec outputVec;
    vector<IntermediateVec*>* shuffleList;
    atomic<int> mapCounter;
    atomic<int> sortCounter;
    atomic<int> reduceCounter;
    mutex mutex1;
    stage_t stage;
    Barrier *barrier;
    bool joined;
    InputVec inputVec;
    const MapReduceClient *client;
    vector<ThreadContext*> *threads;

    JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec,
            int multiThreadLevel);

    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&sortCounter, 0);
        atomic_init(&reduceCounter, 0);

        threadWorkspaces = new vector<IntermediateVec>(numThreads);
        threads = new vector<ThreadContext*>();
        barrier = new Barrier(numThreads);
        shuffleList = new vector<IntermediateVec*>;

    }

    void safePushBackOutputVec(K3 *key, V3 *value);
};



void *thread(void *context2);

#endif //EX3_JOBMANAGER_H
