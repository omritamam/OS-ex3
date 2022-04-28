//
// Created by omri.tamam on 28/04/2022.
//

#include "MapReduceFramework.h"
#include "JobManager.cpp"
    JobHandle startMapReduceJob(const MapReduceClient& client,
                                const InputVec& inputVec, OutputVec& outputVec,
                                int multiThreadLevel){
        return new JobManager(&client, inputVec, outputVec,multiThreadLevel);
};

void emit2 (K2* key, V2* value, void* context){
    IntermediateVec* workspace = (IntermediateVec*) context;
    workspace->push_back(*(new IntermediatePair(key,value)));

}
