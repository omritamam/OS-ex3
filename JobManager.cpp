#include "JobManager.h"

using namespace std;

JobManager::JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec,
                       vector<OutputPair> outputVec, int multiThreadLevel) {

    this->client = client;
    this->inputVec = inputVec;
    this->outputVec = outputVec;

    init(multiThreadLevel);

    for(int i = 0; i < multiThreadLevel; i++){
        IntermediateVec* newWorkspace = new IntermediateVec;
        threadWorkspaces->push_back(newWorkspace);
        ThreadContext *context =  new ThreadContext(i, this, newWorkspace);
        if(pthread_create(&context->thread, NULL, thread, context)){
            //err
        }
        threads->push_back(context);
    }

}

void JobManager::safePushBackOutputVec(K3* key, V3* value){
        mutex1.lock();
        auto pair = make_pair(key, value);
        outputVec.push_back(pair);
        mutex1.unlock();
    }



K2 *findMaxKey(JobManager *jobManager) {
    K2* max = nullptr;
    for (auto workspace : *jobManager->threadWorkspaces) {
        if (!workspace->empty()){
            K2* current = workspace->back().first;
            if (max == nullptr || *max < *current ){
                max = current;
            }
        }
    }
    return max;
}


bool sortByKey(const IntermediatePair &a, const IntermediatePair &b){
    return *(a.first) < *(b.first);
}


bool isNotEmpty(vector<IntermediateVec *> *vector) {
    for (const auto& workspace : *vector)
        if (!workspace->empty()) {
            return true;
        }
    return false;
}


void shuffle(JobManager *jobManager){
    unsigned long size = 0;
    for (auto workspace : *jobManager->threadWorkspaces) {
        size+=workspace->size();
    }
    jobManager->currentStageElementSize= size;
    while (isNotEmpty(jobManager->threadWorkspaces)){
        auto* keyVec = new IntermediateVec;
        K2* key = findMaxKey(jobManager);
        for (auto workspace : *jobManager->threadWorkspaces) {
            while (!(workspace->empty()) && !(*key < *workspace->back().first) && !(*workspace->back().first < *key)){
                keyVec->push_back(workspace->back());
                workspace->pop_back();
                jobManager->shuffleCounter++;
            }
        }
        jobManager->shuffleList->push_back(keyVec);
    }
}


void *thread(void *context2)
{
    ThreadContext* context = (ThreadContext*) context2;
    auto workspace = context->workspace;
    auto client = context->jobManager->client;
    auto n = context->jobManager->inputVec.size();
    //Map
    if(context->id == 0){
        context->jobManager->stage = stage_t::MAP_STAGE;
        context->jobManager->currentStageElementSize = n;
    }
    int current = 0;
    while (current < n) {
        current = (context->jobManager->mapCounter)++;
        if (current<n) {
            auto currentPair = context->jobManager->inputVec[current];
            auto key = currentPair.first;
            auto value = currentPair.second;
            client->map(key, value, workspace);
        }
    }
    //Sort
    sort(workspace->begin(), workspace->end(), sortByKey);
    context->jobManager->sortCounter++;
    context->jobManager->barrier->barrier();
    //Shuffle
    if(context->id == 0){
        context->jobManager->stage = stage_t::SHUFFLE_STAGE;
        shuffle(context->jobManager);
        context->jobManager->stage = stage_t::REDUCE_STAGE;
        context->jobManager->currentStageElementSize = context->jobManager->shuffleList->size();
    }
    //inappropriate use of barrier - need to think of something smarter
    context->jobManager->barrier->barrier();


    //Reduce
    current = 0;
    n = context->jobManager->shuffleList->size();
    while (current < n) {
        current = (context->jobManager->reduceCounter)++;
        if (current<n) {
            auto kvVector = context->jobManager->shuffleList->at(current);
            client->reduce(kvVector, context->jobManager);
        }

    }

    //Wait for all threads to finish before exit
    context->jobManager->barrier->barrier();

    return NULL;
}

