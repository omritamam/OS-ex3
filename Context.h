
#ifndef EX3_CONTEXT_H
#define EX3_CONTEXT_H

#include "MapReduceClient.h"

using namespace std;

class Context{
    const MapReduceClient *client;
    InputVec inputVec;
    IntermediateVec workspace;
    OutputVec outputVec;

public:
    Context(const MapReduceClient *const client, const InputVec &inputVec, OutputVec &outputVec,
            IntermediateVec &workspace) {
        this->client = client;
        this->inputVec = inputVec;
        this->outputVec = outputVec;
        this->workspace = workspace;
    }



    };
#endif //EX3_CONTEXT_H
