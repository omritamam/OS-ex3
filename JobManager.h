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
    atomic<int> changingState;
    int shuffleCounter = 0;
    atomic<int> reduceCounter;
//    atomic<pair<atomic<int>*,int*>> curState;
    pthread_mutex_t outputVecMutex;
    pthread_mutex_t changeStateMutex;
    stage_t stage;
    Barrier *barrier1;


    bool joined = false;
    InputVec inputVec;
    const MapReduceClient *client;
    vector<ThreadContext*> *threads;


    JobManager(const MapReduceClient *client, vector<InputPair> inputVec, vector<OutputPair> outputVec,
            int multiThreadLevel);

    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&reduceCounter, 0);
        atomic_init(&doneCounter, 0);
        atomic_init(&currentStageElementSize, (int) inputVec.size());
        atomic_init(&changingState, 0);
//        auto init_pair = make_pair(&doneCounter, stage_t::UNDEFINED_STAGE);
//        atomic_init(&curState,&init_pair);

        stage = stage_t::UNDEFINED_STAGE;
        threadWorkspaces = new vector<IntermediateVec*>();
        threads = new vector<ThreadContext*>();
        barrier1 = new Barrier(numThreads);
//        barrier2 = new Barrier(numThreads);
//        barrier3 = new Barrier(numThreads);

        shuffleList = new vector<IntermediateVec*>;
        pthread_mutex_init(&outputVecMutex, nullptr);
        pthread_mutex_init(&changeStateMutex, nullptr);

        //if input vector - exit
    }

    void safePushBackOutputVec(K3 *key, V3 *value);

    void freeMemory() {

        for(auto workspace : *threadWorkspaces){
            delete workspace;
        }
        delete threadWorkspaces;

        for(auto keyVec : *shuffleList){
         delete keyVec;
        }
        delete shuffleList;
        for(auto thread : *threads){
            delete thread;
        }
        delete threads;

//        delete barrier1;
//        delete barrier2;
//        delete barrier3;

        delete this;
    }
};



void *thread(void *context2);

#endif
