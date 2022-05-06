#include "JobManager.h"

using namespace std;

JobManager::JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec,
                       vector<OutputPair> outputVec, int multiThreadLevel) {
    fprintf(stdout, "new job manager %p \n", this);
    fflush(stdout);
    this->client = client;
    this->inputVec = inputVec;
    this->outputVec = outputVec;

    init(multiThreadLevel);

    for(int i = 0; i < multiThreadLevel; i++){
        auto* newWorkspace = new IntermediateVec;
        threadWorkspaces->push_back(newWorkspace);
        auto *context =  new ThreadContext(i, this, newWorkspace);
        if(pthread_create(&context->thread, nullptr, thread, context)){
            fprintf(stderr,"Error!");
        }
        threads->push_back(context);
    }

}

void JobManager::safePushBackOutputVec(K3* key, V3* value){
        pthread_mutex_lock(&mutex1);
        auto pair = make_pair(key, value);
        outputVec.push_back(pair);
        pthread_mutex_unlock(&mutex1);
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
    size_t size = 0;
    for (auto workspace : *jobManager->threadWorkspaces) {
        size+=workspace->size();
    }
    jobManager->currentStageElementSize= (int)size;
    while (isNotEmpty(jobManager->threadWorkspaces)){
        auto* keyVec = new IntermediateVec;//TODO change name
        K2* key = findMaxKey(jobManager);
        for (auto workspace : *jobManager->threadWorkspaces) {
            while (!(workspace->empty()) && !(*key < *workspace->back().first) && !(*workspace->back().first < *key)){
                keyVec->push_back(workspace->back());
                workspace->pop_back();
                jobManager->shuffleCounter++;
                jobManager->doneCounter++;
            }
        }
        jobManager->shuffleList->push_back(keyVec);
    }
}


void *thread(void *context2)
{
    auto* context = (ThreadContext*) context2;
    auto workspace = context->workspace;
    auto client = context->jobManager->client;
    size_t n = context->jobManager->inputVec.size();
    //Map
    if(context->id == 0){
        context->jobManager->stage = stage_t::MAP_STAGE;
        context->jobManager->currentStageElementSize = (int)n;
    }
    size_t current = 0;
    while (current < n) {
        current = (size_t)((int)(context->jobManager->mapCounter)++);
        if (current<n) {
            auto currentPair = context->jobManager->inputVec[current];
            auto key = currentPair.first;
            auto value = currentPair.second;
            client->map(key, value, workspace);
            context->jobManager->doneCounter++;
        }
    }
    //Sort
    sort(workspace->begin(), workspace->end(), sortByKey);
    context->jobManager->barrier1->barrier();
    //Shuffle
    if(context->id == 0){
        context->jobManager->stage = stage_t::SHUFFLE_STAGE;
        context->jobManager->doneCounter = 0;
        shuffle(context->jobManager);
        context->jobManager->stage = stage_t::REDUCE_STAGE;
        context->jobManager->doneCounter = 0;
        //todo: change currentStageElementSize to the number of pair!! and update the doneCounter increment currently
        context->jobManager->currentStageElementSize =(int) context->jobManager->shuffleList->size();
    }
    //inappropriate use of barrier - need to think of something smarter
    context->jobManager->barrier1->barrier();


    //Reduce
    current = 0;
    n =(size_t) ((int) context->jobManager->currentStageElementSize);
    while (current < n) {
        current = (size_t)((int)(context->jobManager->reduceCounter)++);
        if (current< n) {
            try {
                auto kvVector = context->jobManager->shuffleList->at(current);
                client->reduce(kvVector, context->jobManager);
                context->jobManager->doneCounter++;
            }
            catch(const std::exception& e){
                printf("fsd");
            }

        }
    }

    //Wait for all threads to finish before exit
//    context->jobManager->barrier1->barrier();

    return nullptr;
}

