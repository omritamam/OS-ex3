
#ifndef EX3_CONTEXT_H
#define EX3_CONTEXT_H

#include "MapReduceClient.h"

using namespace std;

class Context{
    const MapReduceClient *client;
    InputVec inputVec;
    OutputVec outputVec;

public:
    Context(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec,
            vector<OutputPair> vector) {
        this->client = client;
        this->inputVec = inputVec;
        this->outputVec = outputVec;
    }



    };
#endif //EX3_CONTEXT_H
