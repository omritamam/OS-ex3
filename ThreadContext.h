#ifndef EX3_THREADCONTEXT_H
#define EX3_THREADCONTEXT_H

#include "MapReduceClient.h"
#include "JobManager.h"
#include "MapReduceFramework.h"
#include <atomic>

using namespace std;

class ThreadContext{
public:

    ThreadContext(int i, JobManager *pManager);

    ThreadContext(int i, JobManager *pManager);

    ThreadContext(int i, JobManager manager);

    ThreadContext(int i, JobManager manager);

    ThreadContext(int i, JobManager manager);

    int id;
    JobManager *jobManager;
    stage_t stage;
    pthread_t thread;



    ThreadContext(int id, JobManager *jobManager) {
        this->id = id;
        this->jobManager = jobManager;
    }


};
#endif //EX3_THREADCONTEXT_H
