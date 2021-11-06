#pragma once

#include "LUTNet.hpp"
#include <vector>

#ifdef PYTHON_BUILD

#include <pybind11/pybind11.h>
namespace py = pybind11;

#endif


class SLO {
public:

  SLO() {}

/*
def findLayerClusters(lutnet, layer, nodes):
    clusterList = []
    clusterTemplate = {'outputNodes':[], 'inputNodes':[]}
    def makeNewCluster(topNode):
        newCluster = copy.deepcopy(clusterTemplate)
        newCluster['outputNodes'] = [topNode]
        for i_input in range(4):
            newCluster['inputNodes'].append(lutnet.getInputIndex(layer, topNode,i_input))
        return newCluster
        
    for i_toplayer in nodes:
        if len(clusterList) == 0:
            clusterList = [makeNewCluster(i_toplayer)]
        else:
            foundCluster = None
            for i_input in range(4):
                inNodeIdx = lutnet.getInputIndex(layer, i_toplayer, i_input)
                for cluster in clusterList:
                    if inNodeIdx in cluster['inputNodes']:
                        foundCluster = cluster
                        break
                if foundCluster != None:
                    break
            if foundCluster != None:
                #add to current cluster
                cluster['outputNodes'].append(i_toplayer)
                for i_input in range(4):
                    inputNodeIdx = lutnet.getInputIndex(layer, i_toplayer,i_input)
                    if inputNodeIdx not in cluster['inputNodes']:
                        cluster['inputNodes'].append(inputNodeIdx)
            else:
                #make a new cluster
                clusterList.append(makeNewCluster(i_toplayer))
#         print('========', i_toplayer)
#         print(clusterList)
            

    return clusterList
    */


  struct cluster {
    std::vector<uint> inputNodes;
    std::vector<uint> outputNodes;
  };

  std::vector<cluster> findLayerClusters(FFLUT4Net &net, size_t layer, std::vector<uint> nodes) {
    std::vector<cluster> clusterList;
    auto makeNewCluster = [&](uint topNode) {
      cluster c;
      c.outputNodes.push_back(topNode);
      for(size_t i=0; i < 4; i++) {
        c.inputNodes.push_back(net.getInputIndex(layer, topNode,i));
      }
      return c;
    };
    for(auto &i_topLayer : nodes) {
      if (clusterList.size()==0) {
        clusterList.push_back(makeNewCluster(i_topLayer));
      }else{
        bool clusterWasFound = false;
        cluster foundCluster;
        for(uint i_input=0; i_input < 4; i_input++) {
            uint inNodeIdx = net.getInputIndex(layer, i_topLayer, i_input);
            for (auto &cl : clusterList) {
                auto findResult = std::find(cl.inputNodes.begin(), cl.inputNodes.end(), inNodeIdx);
                if (findResult != cl.inputNodes.end()) {
                    foundCluster = cl;
                    clusterWasFound = true;
                    break;
                }
            }
            if (clusterWasFound) {
                break;
            }

        }

        if (clusterWasFound) {
          //add to current cluster
          foundCluster.outputNodes.push_back(i_topLayer);
          for(uint i_input=0; i_input < 4; i_input++) {
              uint inputNodeIdx = net.getInputIndex(layer, i_topLayer, i_input);
              auto findResult = std::find(foundCluster.inputNodes.begin(), foundCluster.inputNodes.end(), inputNodeIdx);
              if (findResult == foundCluster.inputNodes.end()) {
                foundCluster.inputNodes.push_back(inputNodeIdx);
              }
          }
        }else{
          //make a new cluster
          clusterList.push_back(makeNewCluster(i_topLayer));
        }

      }

    }
    // cluster c = makeNewCluster(0);
    // clusterList.push_back(c);
    return clusterList;
  }
};