#include "MapReduceFramework.h"
#include "JobManager.cpp"
#include <pthread.h>

JobHandle startMapReduceJob(const MapReduceClient& client,
                            const InputVec& inputVec, OutputVec& outputVec,
                            int multiThreadLevel){
    return new JobManager(&client, inputVec, outputVec,multiThreadLevel);
};

void emit2 (K2* key, V2* value, void* context){
    IntermediateVec* workspace = (IntermediateVec*) context;
    workspace->push_back(*(new IntermediatePair(key,value)));

}
void emit3 (K3* key, V3* value, void* context){
    JobManager* jobManager = (JobManager*) context;
    jobManager->safePushBackOutputVec(key, value);
}

void waitForJob(JobHandle job){
    JobManager* jobManager = (JobManager*) job;
    if(!jobManager->joined){
        return;
    }
    jobManager->joined = true;
    //get thread of threads;
    pthread_join(jobManager->threads[0]), NULL);

}

void getJobState(JobHandle job, JobState* state){
    //math mainly
}

void closeJobHandle(JobHandle job){

}