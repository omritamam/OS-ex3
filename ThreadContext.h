#ifndef EX3_THREADCONTEXT_H
#define EX3_THREADCONTEXT_H

#include "MapReduceClient.h"
#include "JobManager.h"
#include "MapReduceFramework.h"
#include <atomic>

using namespace std;

class ThreadContext{
public:
    int id;
    JobManager *jobManager;
    stage_t stage;
    pthread_t thread;



    ThreadContext(int id, JobManager &jobManager) {
        this->id = id;
        this->jobManager = &jobManager;
    }



};
#endif //EX3_THREADCONTEXT_H
