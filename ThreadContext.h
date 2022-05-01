#ifndef EX3_THREADCONTEXT_H
#define EX3_THREADCONTEXT_H

#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <atomic>

using namespace std;

class JobManager;

class ThreadContext{
public:

    JobManager* jobManager;
    int id;
    pthread_t thread;
    IntermediateVec *workspace;

    ThreadContext(int id, JobManager *pManager, IntermediateVec *workspace){
        this->id = id;
        this->jobManager = pManager;
        this->workspace = workspace;
    }


};
#endif //EX3_THREADCONTEXT_H
