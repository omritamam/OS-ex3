
#include <armadillo>
#include <atomic>
#include "MapReduceClient.h"
#include "Context.h"

using namespace std;

void *mapper(void *context)
{
    Context* context2 = (Context*) context;
    auto workspace = context2->workspace;
    auto client = tuple->second;
///////////
    client->map(key, value, workspace);

    sleep(1);
    printf("Printing GeeksQuiz from Thread \n");

    return NULL;
}

class JobManager{
    vector<IntermediateVec> *threadWorkspaces;
    atomic<int> counter;

public:
    JobManager(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec,
               int multiThreadLevel){
        init();
        threadWorkspaces = new vector<IntermediateVec>(multiThreadLevel);

        for(int i = 0; i < multiThreadLevel; i++){
            auto* newWorkspace = new vector<pair<K2*,V2*>>;
            threadWorkspaces->push_back(*newWorkspace);
            pthread_t thread_id;
            Context* context = new Context(newWorkspace,client,inputVec,outputVec);
            pthread_create(&thread_id, NULL, mapper, context);
        }
    }
    void init(){
        atomic_init(&counter, 0);
    }



};
