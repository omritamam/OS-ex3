#include "MapReduceFramework.h"
#include "JobManager.h"
#include <pthread.h>

JobHandle startMapReduceJob(const MapReduceClient& client,
                            const InputVec& inputVec, OutputVec& outputVec,
                            int multiThreadLevel){
    return new JobManager(&client, inputVec, outputVec,multiThreadLevel);
}

void emit2 (K2* key, V2* value, void* context){
    auto* workspace = (IntermediateVec*) context;
    workspace->push_back(*(new IntermediatePair(key,value)));

}
void emit3 (K3* key, V3* value, void* context){
    auto* jobManager = (JobManager*) context;
    jobManager->safePushBackOutputVec(key, value);
}

void waitForJob(JobHandle job){
    auto* jobManager = (JobManager*) job;
    if(jobManager->joined){
        return;
    }
    jobManager->joined = true;
    //get thread of threads;
    for(auto thread : *jobManager->threads){
        pthread_join(thread->thread, nullptr);
    }
}

void getJobState(JobHandle job, JobState* state){

    auto* jobManager = (JobManager*) job;
    pthread_mutex_lock(&jobManager->changeStateMutex);
    JobState newState;
    do{
        newState.stage = jobManager->stage;
        newState.percentage = (float)((int) jobManager-> doneCounter) / (float) ((int) jobManager->currentStageElementSize) * 100;
        if(isinf(newState.percentage)){
            printf("eze baasaaaa \n");
        }
    } while (newState.stage != jobManager->stage);
    *state = newState;
    pthread_mutex_unlock(&jobManager->changeStateMutex);

}

void closeJobHandle(JobHandle job){
    waitForJob(job);
    auto* jobManager = (JobManager*) job;
    if(pthread_mutex_destroy(&jobManager->outputVecMutex)){
        fprintf(stderr, "outputVecMutex pthread_mutex_destroy bug");
    }
    if(pthread_mutex_destroy(&jobManager->changeStateMutex)){
        fprintf(stderr, "changeStateMutex pthread_mutex_destroy bug");
    }
    jobManager->freeMemory();
}