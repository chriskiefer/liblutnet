#pragma once

#include <vector>
#include <cmath>
#include <stdexcept>
#include <random>
// #include <thread>
// #include <future>

#ifdef PYTHON_BUILD

#include <pybind11/pybind11.h>
namespace py = pybind11;

#endif

#ifndef uint
using uint = unsigned int;
#endif

struct LUT4 {
  uint ttable[16];
  uint output=0;
  LUT4 *inputs[4] = {0,0,0,0};
  uint idx=0;
};


using LUT4Layer = std::vector<LUT4>;

class FFLUT4Net {
public:


  FFLUT4Net(uint numInputs) {
    addInputLayer(numInputs);
  }

  void addLayer(uint numNodes, bool randomiseTtables=true) {

    std::random_device rd;
    std::mt19937_64 mersenne;
    std::bernoulli_distribution bd;

    mersenne = std::mt19937_64(rd());
    bd = std::bernoulli_distribution(0.5);

    LUT4Layer newLayer(numNodes);
    //make connections
    uint topLayerNodeCount = layers.back().size();
    uint stride = ceil(double(topLayerNodeCount) / double(numNodes));
    // std::cout << "stride: " << stride << std::endl;
// #ifdef PYTHON_BUILD
// py::print("stride", stride);
// #endif
    for(uint i=0; i < numNodes; i++) {
      //make connections
      for(uint i_input =0; i_input < 4; i_input++) {
        uint inputNodeIndex = ((i * stride) + i_input) % topLayerNodeCount;
        newLayer[i].inputs[i_input] = &layers.back().at(inputNodeIndex);
      }
      //init ttable
      for(uint i_t=0; i_t < 16; i_t++) {
        newLayer[i].ttable[i_t] =  randomiseTtables ? bd(mersenne) : 0;
      }
      //record node idx in layer
      newLayer[i].idx = i;
    }
    layers.push_back(newLayer);
  }

  void setTtable(uint layer, uint nodeNumber, uint ttable[16]) {
    for(uint i=0; i < 16; i++) {
        setTtableElement(layer, nodeNumber, i, ttable[i]);
    }
  }

  inline uint calcNodeLUTAddress(uint layer, uint nodeNumber) {
    uint addr = 0;
    for(uint i_input=0; i_input < 4; i_input++) {
      addr += (layers[layer][nodeNumber].inputs[3-i_input]->output << i_input);
    }
    return addr;
  }

  void calc(std::vector<unsigned int> &input) {
    if (input.size() != layers[0].size()) throw std::out_of_range("input size OOR");
    for(uint i=0; i < input.size(); i++) {
      layers[0][i].output = input[i];
    }
    auto calcLayerBlock = [this](uint layer, uint start, uint blockSize) {
      for(uint i_node=start; i_node < start + blockSize; i_node++) {
        uint addr = calcNodeLUTAddress(layer, i_node);
        layers[layer][i_node].output = layers[layer][i_node].ttable[addr];
      }
    };
    for(uint i=1; i < layers.size(); i++) {
      //at this stage, threading just slows it down
      // if (layers[i].size()   > 80000000) {
      //   uint numThreads = 16;
      //   uint blockSize = ceil(double(layers[i].size()) / double(numThreads));
      //   uint lastBlockSize = layers[i].size() - ((numThreads-1) * blockSize);
      //   std::vector<std::future<void> > future_threads;
      //   future_threads.reserve(numThreads);
      //   for(uint i_thread = 0; i_thread < numThreads - 1; i_thread++) {
      //     future_threads.emplace_back(std::async(std::launch::async, [&calcLayerBlock, i_thread, blockSize, i]() {
      //       calcLayerBlock(i, i_thread * blockSize, blockSize);
      //     }));
      //   }
      //   // auto last_thread_future = std::async(std::launch::async, calcLayerBlock, (numThreads - 1) * blockSize, lastBlockSize);
      //   future_threads.emplace_back(std::async(std::launch::async, [&calcLayerBlock, numThreads, blockSize, i, lastBlockSize]() {
      //     calcLayerBlock(i, (numThreads - 1) * blockSize, lastBlockSize);
      //   }));
      //   // future_threads.emplace_back(last_thread_future);
      //   for(auto& fut : future_threads) {
      //     fut.get();
      //   }
      // }else{
      //   calcLayerBlock(i, 0, layers[i].size());
      // }
      calcLayerBlock(i, 0, layers[i].size());
    }
  }

  int getTtableElement(unsigned int layer, unsigned int node, unsigned int bit) {
    if (bit > 15) throw std::out_of_range("bit index over 15");
    return layers.at(layer).at(node).ttable[bit];
  }

  void setTtableElement(unsigned int layer, unsigned int node, unsigned int bit, unsigned int value) {
    if (bit > 15) throw std::out_of_range("bit index over 15");
    layers.at(layer).at(node).ttable[bit] = value;
  }

  uint getLayerSize(uint idx) {
    return layers.at(idx).size();
  }

  uint getLayerCount() {
    return layers.size();
  }

  uint getOutput(uint layer, uint node) {
    return layers.at(layer).at(node).output;
  }

  uint getTopLayerOutput(uint node) {
    return layers.back().at(node).output;
  }

  uint getInputIndex(uint layer, uint node, uint input) {
    if (input > 3) {throw std::out_of_range("input index over 3"); return 0;}
    if (layer ==0) {throw std::out_of_range("no input to layer 0"); return 0;}
    return layers.at(layer).at(node).inputs[input]->idx;
  }

  const std::vector<uint> serialiseLayer(uint layerNum) const {
    std::vector<uint> data(layers.at(layerNum).size());
    for (uint node=0; node < data.size(); node++) {
      uint packedTable=0;
      for(uint bit=0; bit < 16; bit++) {
        uint bitValue = layers[layerNum][node].ttable[bit];
        if (bitValue) {
          packedTable |= (1 << (15-bit));
        }
        // std::cout << bitValue << "/";

      }
      // std::cout << std::endl;
      // std::cout << packedTable << std::endl;
      data[node] = packedTable;
    }
    return data;
  }

  const std::vector<uint> serialiseStructure() const {
    std::vector<uint> structure;
    structure.push_back(layers.size());
    for(uint layer=0; layer < layers.size(); layer++) {
      structure.push_back(layers[layer].size());
      //dont actually need to save this because it's calculated when you add a layer
      // if (layer > 0) {
      //   for(uint node=0; node < layers[layer].size(); node++) {
      //     for(uint inidx=0; inidx < 4; inidx++) {
      //       structure.push_back(layers[layer][node].inputs[inidx] -> idx);
      //     }
      //   }
      // }
    }
    return structure;
  }

  const std::vector<uint> serialiseAllLayers() const {
    std::vector<uint> allLayers;
    //start from layer 1, layer 0 doesn't use inputs / ttables
    for(uint layer=1; layer < layers.size(); layer++) {
      auto layerData = serialiseLayer(layer);
      allLayers.insert(allLayers.end(), layerData.begin(), layerData.end());
    }
    return allLayers;
  }

  const std::vector<uint> serialiseModel() const {
    auto struc = serialiseStructure();
    auto ttables = serialiseAllLayers();
    struc.insert(struc.end(), ttables.begin(), ttables.end());
    return struc;
  }

  void unserialise(std::vector<uint> &bin) {
    layers.clear();
    uint layerCount=bin.at(0);
    addInputLayer(bin.at(1));
    for(uint layer=1; layer < layerCount; layer++) {
      addLayer(bin.at(1+layer), false);
    }
    uint pos = layerCount+1;
    for(uint layer=1; layer < layerCount; layer++) {
      for(uint node=0; node < layers[layer].size(); node++) {
        uint ttableValue = bin.at(pos);
        // std::cout << ttableValue << std::endl;
        pos++;
        for(uint bit=0; bit < 16; bit++) {
          uint bitValue = (ttableValue & (1 << (15-bit))) >> (15-bit) ;
          // std::cout << bitValue << "/";
          layers[layer][node].ttable[bit] = bitValue;
        }
        // std::cout << std::endl;
      }
    }
  }

private:

  std::vector<LUT4Layer > layers;

  void addInputLayer(uint numInputs) {
    LUT4Layer inputLayer(numInputs);
    for(uint i=0; i < numInputs; i++) {
      inputLayer[i].idx = i;
      for(uint inIdx=0; inIdx < 4; inIdx++) {
        inputLayer[i].inputs[inIdx] = 0;
      }
    }
    layers.push_back(inputLayer);
  }
};
