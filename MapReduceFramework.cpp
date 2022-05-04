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
    if(!jobManager->joined){
        return;
    }
    jobManager->joined = true;
    //get thread of threads;
    pthread_join(jobManager->threads->at(0)->thread, nullptr);

}

void getJobState(JobHandle job, JobState* state){

    auto* jobManager = (JobManager*) job;
    JobState newState;
    newState.stage = jobManager->stage;
    //HANDLE CONTEXT SWITCH WHILE CALCULATING
    newState.percentage = (float)((int) jobManager-> doneCounter) / (float) ((int) jobManager->currentStageElementSize) * 100;
    *state = newState;
}

void closeJobHandle(JobHandle job){
    waitForJob(job);
    auto* jobManager = (JobManager*) job;
    if(pthread_mutex_destroy(&jobManager->mutex1)){
        fprintf(stderr, "pthread_mutex_destroy bug");
    }
    jobManager->freeMemory();
}