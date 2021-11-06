
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "LUTNet.hpp"
#include "SLO.hpp"
#include <vector>


namespace py = pybind11;

using namespace std;

char const* greet()
{
   return "greetings, would you like a refridgerators?";
}

PYBIND11_MODULE(lutnet, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring
    m.def("greet", &greet, "Hello");
    
    py::class_<SLO>(m, "SLO")
    .def(py::init<>())
    .def("findLayerClusters", &SLO::findLayerClusters);

    py::class_<SLO::cluster>(m, "SLOcluster")
    // .def(py::init<>())
    .def_readwrite("inputNodes", &SLO::cluster::inputNodes)
    .def_readwrite("outputNodes", &SLO::cluster::outputNodes)
    ;

    py::class_<FFLUT4Net>(m, "FFLUT4Net")
    .def(py::init<uint>())
    .def("addLayer",  &FFLUT4Net::addLayer, py::arg("numNodes") = 4, py::arg("randomiseTtables") = true)
    .def("setTtable", &FFLUT4Net::setTtable)
    .def("calcNodeLUTAddress", &FFLUT4Net::calcNodeLUTAddress)
    .def("calc", &FFLUT4Net::calc)
    .def("getTtableElement", &FFLUT4Net::getTtableElement)
    .def("setTtableElement", &FFLUT4Net::setTtableElement)
    .def("getLayerSize", &FFLUT4Net::getLayerSize)
    .def("getOutput", &FFLUT4Net::getOutput)
    .def("getLayerSize", &FFLUT4Net::getLayerSize)
    .def("getLayerCount", &FFLUT4Net::getLayerCount)
    .def("getInputIndex", &FFLUT4Net::getInputIndex)
    .def("getTopLayerOutput", &FFLUT4Net::getTopLayerOutput)

    .def("serialiseAllLayers", &FFLUT4Net::serialiseAllLayers)
    .def("serialiseModel", &FFLUT4Net::serialiseModel)
    .def("unserialise", &FFLUT4Net::unserialise)

    .def(py::pickle(
            [](const FFLUT4Net &p) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                auto data = p.serialiseModel();
                py::tuple tp(data.size());
                for(uint i=0; i < data.size(); i++) {
                  tp[i] = data[i];
                }
                return tp;
            },
            [](py::tuple t) { // __setstate__
                // if (t.size() != 1)
                //     throw std::runtime_error("Invalid state!");

                /* Create a new C++ instance */
                cout << "unpickling" << endl;
                FFLUT4Net p(4);
                std::vector<uint> data(t.size());
                for(uint i=0; i < t.size(); i++) {
                  data[i] = t[i].cast<uint>();
                }
                cout << "unserialising" << endl;
                p.unserialise(data);

                return p;
            }
        ))
    ;
}
