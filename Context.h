//
// Created by otamam on 30/04/2022.
//

#ifndef EX3_CONTEXT_H
#define EX3_CONTEXT_H
using namespace std;

class Context{
    const MapReduceClient *client;
    vector<InputPair> inputVec;
    vector<OutputPair> outputVec;

public:
    Context(const MapReduceClient *const client, const vector<InputPair> inputVec, vector<OutputPair> outputVec) {
        this->client = client;
        this->inputVec = inputVec;
        this->outputVec = outputVec;
    }



    };
#endif //EX3_CONTEXT_H
