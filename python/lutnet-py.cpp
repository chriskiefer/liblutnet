
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "LUTNet.hpp"


namespace py = pybind11;

char const* greet()
{
   return "greetings, would you like a refridgerator?";
}

PYBIND11_MODULE(lutnet, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring
    m.def("greet", &greet, "Hello");
    py::class_<FFLUT4Net>(m, "FFLUT4Net")
    .def(py::init<uint>())
    .def("addLayer",  &FFLUT4Net::addLayer)
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
    ;
}
