#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <stdio.h>
#include <map>

using namespace std;

#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

#define L1access 7
#define L2access 12
#define memoryaccess 15




struct config{
    unsigned long L1blocksize;
    unsigned long L1setsize;
    unsigned long L1size;
    unsigned long L2blocksize;
    unsigned long L2setsize;
    unsigned long L2size;
};


class cache {
    config config_params;
    unsigned long setIndexBit_L1, tag_L1, blockOffset_L1, setIndexBit_L2, tag_L2, blockOffset_L2;
    vector<vector<unsigned long> > cache_L1;
    vector<vector<unsigned long> > cache_L2;
    vector<vector<unsigned long> > validBit_L1, validBit_L2;


    public :
        cache(config config_param){
            this->config_params = config_param;

            this->blockOffset_L1 = (unsigned long)log2(config_params.L1blocksize);
            this->blockOffset_L2 = (unsigned long)log2(config_params.L2blocksize);

            this->setIndexBit_L1 = (unsigned long)log2((config_params.L1size)*(pow(2,10))/(config_params.L1blocksize*config_params.L1setsize));
            this->setIndexBit_L2 = (unsigned long)log2((config_params.L2size)*(pow(2,10))/(config_params.L2blocksize*config_params.L2setsize));

            this->tag_L1 = 32 - setIndexBit_L1 - blockOffset_L1;
            this->tag_L2 = 32 - setIndexBit_L2 - blockOffset_L2;

            unsigned long indexRow_L1 = (unsigned long)pow(2, setIndexBit_L1);
            unsigned long indexRow_L2 = (unsigned long)pow(2, setIndexBit_L2);

            this->validBit_L1.resize(this->config_params.L1setsize);
            for(int i=0;i<this->config_params.L1setsize;i++){
                this->validBit_L1[i].resize(indexRow_L1);
            }

            this->validBit_L2.resize(this->config_params.L2setsize);
            for(int i=0;i<this->config_params.L2setsize;i++){
                this->validBit_L2[i].resize(indexRow_L2);
            }


            unsigned long blockSize_L1 = (unsigned long)pow(2, blockOffset_L1);
            unsigned long blockSize_L2 = (unsigned long)pow(2, blockOffset_L2);

            this->cache_L1 = createCache(config_params.L1setsize, indexRow_L1, blockSize_L1);
            this->cache_L2 = createCache(config_params.L2setsize, indexRow_L2, blockSize_L2);
        }

        

        vector<string> getBits(bitset<32> addr) {
            string stringAddr = addr.to_string();
            vector<string> bits(6);
            bits[0] = stringAddr.substr(0, this->tag_L1);
            bits[1] = stringAddr.substr(this->tag_L1, this->setIndexBit_L1);
            bits[2] = stringAddr.substr(this->tag_L1+this->setIndexBit_L1, this->blockOffset_L1);

            bits[3] = stringAddr.substr(0, this->tag_L2);
            bits[4] = stringAddr.substr(this->tag_L2, this->setIndexBit_L2);
            bits[5] = stringAddr.substr(this->tag_L2+this->setIndexBit_L2,this->blockOffset_L2);
            return bits;
        }

        vector<vector<unsigned long> > createCache(unsigned long &way, unsigned long &indexRow, unsigned long &blockSize){
            vector<vector<unsigned long> > myCache;

            myCache.resize((unsigned long)way);
            for(int i=0; i<way;i++) {
                myCache[i].resize(indexRow);

            }

            return myCache;
        }

    vector<vector<unsigned long> > getL1Cache(){
        return this->cache_L1;
    }

    vector<vector<unsigned long> > getL2Cache(){
        return this->cache_L2;
    }

    vector<vector<unsigned long> > getValidL1(){
        return this->validBit_L1;
    }

    vector<vector<unsigned long> > getValidL2(){
        return this->validBit_L2;
    }

};

map<unsigned long, bool> isWaysFull(vector<vector<unsigned long> > &L_cache, unsigned long way_L, long index_L){

    map<unsigned long, bool> cacheMap;

    for(unsigned long i=0;i<way_L;i++){

        cacheMap[i] = L_cache[i][index_L] != 0;
    }
    
    return cacheMap;
}

void loadTag(vector<vector<unsigned long> > &L_cache, vector<vector<unsigned long> > &L_validBit, unsigned long &way_L,unsigned long &wayNumber, long &index_L, long tag_L){

    L_cache[wayNumber][index_L] = (unsigned long)tag_L;
    L_validBit[wayNumber][index_L] = 1;

}

unsigned long incCounter (unsigned long &way_L, unsigned long &wayNumber){

return (wayNumber+1)%way_L;

}

int l1_miss = 0;
int l1_hit = 0;
int l2_miss = 0;
int l2_hit = 0;
int cycle = 0;
