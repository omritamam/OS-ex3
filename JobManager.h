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
    vector<IntermediateVec*> *threadWorkspaces;
    OutputVec outputVec;
    vector<IntermediateVec*>* shuffleList;
    atomic<int> mapCounter;
    atomic<int> sortCounter;
    atomic<int> doneCounter;
    int shuffleCounter = 0;
    atomic<int> reduceCounter;
    pthread_mutex_t mutex1;
    stage_t stage;
    Barrier *barrier;
    bool joined;
    InputVec inputVec;
    const MapReduceClient *client;
    vector<ThreadContext*> *threads;
    size_t currentStageElementSize;


    JobManager(const MapReduceClient *client, vector<InputPair> inputVec, vector<OutputPair> outputVec,
            int multiThreadLevel);

    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&sortCounter, 0);
        atomic_init(&reduceCounter, 0);
        atomic_init(&doneCounter, 0);

        threadWorkspaces = new vector<IntermediateVec*>();
        threads = new vector<ThreadContext*>();
        barrier = new Barrier(numThreads);
        shuffleList = new vector<IntermediateVec*>;
        pthread_mutex_init(&mutex1, nullptr);

    }

    void safePushBackOutputVec(K3 *key, V3 *value);
    void freeMemory() {
        for(auto workspace : *threadWorkspaces){
            free(workspace);
        }
        free(threadWorkspaces);
        free(threads);
        free(barrier);
        free(shuffleList);
    }
};



void *thread(void *context2);

#endif
