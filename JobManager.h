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
    atomic<int> doneCounter;
    atomic<int> currentStageElementSize;
    int shuffleCounter = 0;
    atomic<int> reduceCounter;
    pthread_mutex_t mutex1;
    stage_t stage;
    Barrier *barrier1;
    Barrier *barrier2;
    Barrier *barrier3;

    bool joined;
    InputVec inputVec;
    const MapReduceClient *client;
    vector<ThreadContext*> *threads;


    JobManager(const MapReduceClient *client, vector<InputPair> inputVec, vector<OutputPair> outputVec,
            int multiThreadLevel);

    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&reduceCounter, 0);
        atomic_init(&doneCounter, 0);
        atomic_init(&currentStageElementSize, 0);


        threadWorkspaces = new vector<IntermediateVec*>();
        threads = new vector<ThreadContext*>();
        barrier1 = new Barrier(numThreads);
        barrier2 = new Barrier(numThreads);
        barrier3 = new Barrier(numThreads);

        shuffleList = new vector<IntermediateVec*>;
        pthread_mutex_init(&mutex1, nullptr);

    }

    void safePushBackOutputVec(K3 *key, V3 *value);
    void freeMemory() {
        for(auto workspace : *threadWorkspaces){
            free(workspace);
        }
        free(threadWorkspaces);

        for(auto keyVec : *shuffleList){
            for (auto kvp: *keyVec) {
                //free first and second?
            }
            free(keyVec);
        }
        free(shuffleList);
        for(auto thread : *threads){
            free(thread);
        }
        free(threads);

        free(barrier1);
        free(barrier2);
        free(barrier3);
        free(this);
    }
};



void *thread(void *context2);

#endif
