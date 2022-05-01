
#include <armadillo>
#include <vector>
#include <mutex>
#include <atomic>
#include "MapReduceClient.h"
#include "ThreadContext.h"
#include "demo/Barrier/Barrier.h"

using namespace std;

void *thread(void *context2);

class JobManager{
    const MapReduceClient *client;
    InputVec inputVec;
    vector<IntermediateVec> *threadWorkspaces;
    OutputVec outputVec;

   vector<IntermediateVec*>* shuffleList;

    atomic<int> mapCounter;
    atomic<int> sortCounter;
    atomic<int> reduceCounter;
    mutex mutex;
    Barrier *barrier;


public:
    bool joined;
    vector<ThreadContext*> *threads;

    JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec,
               int multiThreadLevel){
        this->client = client;
        this->inputVec = inputVec;
        this->outputVec = outputVec;

        init(multiThreadLevel);

        for(int i = 0; i < multiThreadLevel; i++){
            auto* newWorkspace = new IntermediateVec;
            threadWorkspaces->push_back(*newWorkspace);
            ThreadContext *context =  new ThreadContext(i, this);
            if(pthread_create(&context->thread, NULL, thread, context)){
                //err
            }
            threads->push_back(context);
        }
    }

    void safePushBackOutputVec(K3* key, V3* value){
        mutex.lock();
        auto pair = make_pair(key, value);
        outputVec.push_back(pair);
        mutex.unlock();
    }

private:
    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&sortCounter, 0);
        atomic_init(&reduceCounter, 0);

        threadWorkspaces = new vector<IntermediateVec>(numThreads);
        threads = new vector<ThreadContext*>();
        barrier = new Barrier(numThreads);
        shuffleList = new vector<IntermediateVec*>;

    }


    K2 *findMaxKey(JobManager *jobManager) {
        K2* max = nullptr;
        for (auto workspace : *jobManager->threadWorkspaces) {
            if (!workspace.empty()){
                K2* current = workspace.back().first;
                if (max == nullptr || *max < *current ){
                    max = current;
                }
            }
        }
        return max;
    }



    void shuffle(JobManager *jobManager){
        auto n = jobManager->inputVec.size();
        int counter = 0;
        while (counter < n){
            auto* keyVec = new IntermediateVec;
            K2* key = findMaxKey(jobManager);
            for (auto workspace : *jobManager->threadWorkspaces) {
                while (!(workspace.empty()) && !(*key < *workspace.back().first) && !(*workspace.back().first < *key)){
                    keyVec->push_back(workspace.back());
                    workspace.pop_back();
                    counter++;
                }
            }
            jobManager->shuffleList->push_back(keyVec);
        }
    }


};
bool sortByKey(const IntermediatePair &a, const IntermediatePair &b){
    return (a.first < b.first);
}


void *thread(void *context2)
{
    ThreadContext* context = (ThreadContext*) context2;
    auto workspace = context->jobManager->workspace;
    auto client = context->jobManager->client;
    auto n = context->jobManager->inputVec->size();
    //Map
    context->stage = stage_t::MAP_STAGE;
    int current = 0;
    while (current < n) {
        current = ++(context->jobManager->mapCounter);
        if (current<n) {
            auto currentPair = context->jobManager->inputVec[current];
            auto key = currentPair->first;
            auto value = currentPair->second;
            client->map(key, value, workspace);
        }
    }
    //Sort
    sort(workspace.begin(), workspace.end(), sortByKey);
    context->jobManager->sortCounter++;

    context->jobManager->barrier->barrier();
    //Shuffle
    context->stage = stage_t::SHUFFLE_STAGE;
    if(context->id == 0){
        shuffle(context->jobManager);
    }
    //inappropriate use of barrier - need to think of something smarter
    context->jobManager->barrier->barrier();

    //Reduce
    context->stage = stage_t::REDUCE_STAGE;
    current = 0;
    while (current < n-1) {
        current = ++(context->jobManager->reduceCounter);
        auto currentVec = context->jobManager->shuffleList.at(current);
        for(auto kvp :currentVec ){
            auto key = currentVec->first;
            auto value = currentVec->second;
            client->reduce(key, value, context->jobManager);
        }
    }

    //Wait for all threads to finish before exit
    context->jobManager->barrier->barrier();

    return NULL;
}

