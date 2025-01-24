// make binding for filterEdges function

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "analyze_graph.hpp"

#ifndef PY_MODULE_NAME
#error "PY_MODULE_NAME must be defined"
#endif

PYBIND11_MODULE(PY_MODULE_NAME, m) {
    m.def("filterEdges", &filterEdges);
    m.def("make_graph", &make_graph);
    m.def("findMaxDistances", &findMaxDistances);
    m.def("accumulateBackwards", &accumulateBackwards);
}
