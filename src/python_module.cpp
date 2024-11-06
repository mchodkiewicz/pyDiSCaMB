#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/complex.h"

#include "discamb/CrystalStructure/Crystal.h"
#include "discamb/CrystalStructure/UnitCell.h"
#include "discamb/CrystalStructure/SpaceGroup.h"
#include "discamb/CrystalStructure/SpaceGroupOperation.h"

#include "discamb/BasicUtilities/discamb_version.h"

#include "DiscambWrapper.hpp"
#include "DiscambWrapperTests.hpp"
#include "scattering_table.hpp"


namespace py = pybind11;

using namespace std;
using namespace discamb;



vector<complex<double>> calculate_structure_factors_TAAM(py::object &structure, double d){
    DiscambWrapper w {structure, FCalcMethod::TAAM};
    vector< complex<double> > structureFactors = w.f_calc(d);
    return structureFactors;
}

vector<complex<double>> calculate_structure_factors_IAM(py::object &structure, double d){
    DiscambWrapper w {structure, FCalcMethod::IAM};
    vector< complex<double> > structureFactors = w.f_calc(d);
    return structureFactors;
}


PYBIND11_MODULE(_pydiscamb, m) {
    m.doc() = R"pbdoc(
        DiSCaMB wrapper using pybind11
        -----------------------

        .. currentmodule:: pydiscamb

        .. autosummary::
           :toctree: _generate

    )pbdoc";

    m.def(
        "get_discamb_version", 
        &discamb_version::version, 
        R"pbdoc(Get the version string for DiSCaMB)pbdoc"
    );

    m.def(
        "calculate_structure_factors_TAAM", 
        &calculate_structure_factors_TAAM, 
        R"pbdoc(Calculate structure factors for a given structure up to a given d-spacing, using the Transferable Aspherical Atom Model)pbdoc",
        py::arg("structure"),
        py::arg("d_min")
    );
    m.def(
        "calculate_structure_factors_IAM", 
        &calculate_structure_factors_IAM, 
        R"pbdoc(Calculate structure factors for a given structure up to a given d-spacing, using the Independent Atom Model)pbdoc",
        py::arg("structure"),
        py::arg("d_min")
    );

    py::enum_<FCalcMethod>(m, 
            "FCalcMethod", 
            R"pbdoc(Enum for specifying the model for atomic form factor calculations)pbdoc"
        )
        .value("IAM", FCalcMethod::IAM, R"pbdoc(Independent Atom Model)pbdoc")
        .value("TAAM", FCalcMethod::TAAM, R"pbdoc(Transferable Aspherical Atom Model)pbdoc")
        .export_values();

    py::class_<DiscambWrapper>(m, 
            "DiscambWrapper", 
            R"pbdoc(Calculate structure factors using DiSCaMB)pbdoc"
        )
        .def(py::init<py::object, FCalcMethod>(), py::arg("structure"), py::arg("method") = FCalcMethod::IAM)
        .def(
            "f_calc", 
            py::overload_cast<double>(&DiscambWrapper::f_calc), 
            R"pbdoc(Calculate the structure factors up to a given d-spacing)pbdoc",
            py::arg("d_min")
        )
        .def(
            "f_calc", 
            py::overload_cast<>(&DiscambWrapper::f_calc), 
            R"pbdoc(Calculate the structure factors for previously set hkl)pbdoc"
        )
        .def(
            "set_indices",
            &DiscambWrapper::set_indices,
            R"pbdoc(Set indices for calculating f_calc. Input must be iterable of tuples with three ints)pbdoc",
            py::arg("indices")
        )
        .def(
            "set_d_min",
            &DiscambWrapper::set_d_min,
            R"pbdoc(Set minimum d-spacing for calculating f_calc)pbdoc",
            py::arg("d_min")
        )
    ;

    py::class_<DiscambWrapperTests, DiscambWrapper>(m, 
        "DiscambWrapperTests", 
        R"pbdoc(Class for testing the wrapper)pbdoc"
        )
        .def(py::init<py::object, FCalcMethod>(), py::arg("structure"), py::arg("method") = FCalcMethod::IAM)
        .def("test_get_crystal", &DiscambWrapperTests::test_get_crystal)
        .def("test_update_atoms", &DiscambWrapperTests::test_update_atoms)
        .def("test_f_calc", &DiscambWrapperTests::test_f_calc)
        .def("get_f_calc_runtime", &DiscambWrapperTests::get_f_calc_runtime)
        .def("get_f_calc_runtime_with_atom_updates", &DiscambWrapperTests::get_f_calc_runtime_with_atom_updates)
    ;

    py::class_<GaussianScatterer>(m, "GaussianScatterer")
        .def_readwrite("a", &GaussianScatterer::a)
        .def_readwrite("b", &GaussianScatterer::b)
        .def_readwrite("c", &GaussianScatterer::c)
        .def("__repr__", &GaussianScatterer::repr)
    ;

    m.def(
        "get_table",
        &get_table,
        R"pbdoc(Get a dict of all scatterers in a given table)pbdoc",
        py::arg("table")
    );
}
