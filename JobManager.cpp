
#include <armadillo>
#include <mutex>
#include <atomic>
#include "MapReduceClient.h"
#include "ThreadContext.h"
#include "demo/Barrier/Barrier.h"

using namespace std;

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
    context->stage = stage_t.MAP_STAGE;
    int current = 0;
    while (current < n-1) {
        current = ++(context->jobManager->mapCounter);
        auto currentPair = context->inputVec[current];
        auto key = currentPair->first;
        auto value = currentPair->second;
        client->map(key, value, workspace);
    }
    //Sort
    context->stage = stage_t.SORT_STAGE;
    sort(workspace.begin(), workspace.end(), sortByKey);
    context->jobManager->sortCounter++;

    context->jobManager->barrier->barrier();
    //Shuffle
    context->stage = stage_t.SHUFFLE_STAGE;
    if(context->id == 0){
        shuffle(context->jobManager);
    }
    //inappropriate use of barrier - need to think of something smarter
    context->jobManager->barrier->barrier();

    //Reduce
    context->stage = stage_t.REDUCE_STAGE;
    current = 0;
    while (current < n-1) {
        current = ++(context->jobManager->reduceCounter);
        auto currentPair = context->jobManager->shuffleList[current];
        auto key = currentPair->first;
        auto value = currentPair->second;
        client->reduce(key, value, context->jobManager);
    }

    //Wait for all threads to finish before exit
    context->jobManager->barrier->barrier();

    return NULL;
}

void shuffle(JobManager &jobManager){
    while()
}



class JobManager{
    const MapReduceClient *client;
    InputVec inputVec;
    vector<IntermediateVec> *threadWorkspaces;
    OutputVec outputVec;

    IntermediateVec shuffleList;

    atomic<int> mapCounter;
    atomic<int> sortCounter;
    atomic<int> reduceCounter;
    mutex mutex;
    Barrier barrier;


public:
    bool joined;
    vector<ThreadContext*> *threads

    JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec,
               int multiThreadLevel){
        this->client = client;
        this->inputVec = inputVec;
        this->outputVec = outputVec;

        init(numThreads);

        for(int i = 0; i < multiThreadLevel; i++){
            auto* newWorkspace = new IntermediateVec;
            threadWorkspaces->push_back(*newWorkspace);
            ThreadContext *context =  new ThreadContext(i, this);
            context->theard = pthread_create(&thread, NULL, thread, context);
            threads->push_back(context);
        }
    }

    int safePushBackOutputVec(K3* key, V3* value){
        mutex.lock();
        auto pair = new OutputPair (key, value);
        int rt = shuffleList.push_back(pair);
        mutex.unlock();
        return rt;
    }

private:
    void init(int numThreads){
        atomic_init(&mapCounter, 0);
        atomic_init(&sortCounter, 0);
        atomic_init(&reduceCounter, 0);

        threadWorkspaces = new vector<IntermediateVec>(multiThreadLevel);
        threads = vector<ThreadContext*>();

        barrier = new Barrier(numThreads);

    }


};
