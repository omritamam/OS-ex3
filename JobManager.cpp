//
// Created by omri.tamam on 28/04/2022.
//


#include <armadillo>
#include <atomic>
#include "MapReduceClient.h"

using namespace std;

void *mapper(void *context)
{
    auto tuple = (pair< IntermediateVec ,const MapReduceClient*>*) context;
    auto workspace = tuple->first;
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
    JobManager(const MapReduceClient *const client, const vector<InputPair> vector1, vector<OutputPair> vector2,
               int multiThreadLevel){
        init();
        threadWorkspaces = new vector<IntermediateVec>(multiThreadLevel);

        for(int i = 0; i < multiThreadLevel; i++){
            auto* newWorkspace = new vector<pair<K2*,V2*>>;
            threadWorkspaces->push_back(*newWorkspace);
            pthread_t thread_id;
            auto tuple = new pair< vector<pair<K2*,V2*>>,const MapReduceClient*>(
                    *newWorkspace,reinterpret_cast<const MapReduceClient *const>(&client));
            // send object instead of the tuple with access to atomic counter
            pthread_create(&thread_id, NULL, mapper, tuple);
        }
    }
    void init(){
        atomic_init(&counter, 0);
    }



};
