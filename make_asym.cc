#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>

namespace py = pybind11;
using std::pow;

static double make_asym_func(
    py::array_t<double, py::array::c_style | py::array::forcecast> dataIn,
    py::array_t<double, py::array::c_style | py::array::forcecast> disIn,
    py::array_t<double, py::array::c_style | py::array::forcecast> weightsIn,
    int w_truth
    ){
    auto data = dataIn.request(),
         dis = disIn.request(),
         weights = weightsIn.request();

    if (data.ndim != 2 || dis.ndim !=2 || weights.ndim !=2)
        throw std::runtime_error("Input arrays must be two dimensional");

    //std::cout << "shapes\n" << data.shape[0] << "," << data.shape[1] << "\n";
    //std::cout << "shapes\n" << dis.shape[0] << "," << dis.shape[1] << "\n";
    //std::cout << "shapes\n" << weights.shape[0] << "," << weights.shape[1] << "\n";
    if (data.shape[0] != dis.shape[0] || data.shape[1] != dis.shape[1] ||
        data.shape[0] != weights.shape[0] || data.shape[1] != weights.shape[1])
        throw std::runtime_error("Shape of data and distance arrays unequal");

    double r, weight_sum, sum_weight_mean, mu, core, asym, var, quad, lin;
    double *dataPtr = (double *) data.ptr,
           *disPtr = (double *) dis.ptr,
           *weightsPtr = (double *) weights.ptr;
    asym = 0;

    for (int dis_i = 0; dis_i < dis.shape[0]; dis_i++){
        for(int dis_j = 0; dis_j < dis.shape[1]; dis_j++){
            r = disPtr[dis_i*dis.shape[0]+dis_j];
            weight_sum = 0;
            sum_weight_mean = 0;
            quad = 0;
            lin = 0;
            core = 0;
            mu = 0;
            var = 0;

            for(int dis_ii = 0; dis_ii < dis.shape[0]; dis_ii++){
                for(int dis_jj = 0; dis_jj < dis.shape[1]; dis_jj++){
                    if (disPtr[dis_ii*dis.shape[0]+dis_jj] == r){
                        weight_sum += weightsPtr[dis_ii*dis.shape[0]+dis_jj];
                        sum_weight_mean += weightsPtr[dis_ii*dis.shape[0]+
                                           dis_jj] *
                                           dataPtr[dis_ii*dis.shape[0]+
                                           dis_jj];
                        quad += weightsPtr[dis_ii*dis.shape[0]+dis_jj] *
                                pow(dataPtr[dis_ii*dis.shape[0]+dis_jj], 2);
                        lin += weightsPtr[dis_ii*dis.shape[0]+dis_jj] *
                                dataPtr[dis_ii*dis.shape[0]+dis_jj];
                    }
                }
            }
            mu = sum_weight_mean / weight_sum;
            core = quad - 2*mu * lin + pow(mu, 2) * weight_sum;
            var = core/weight_sum;
            if (w_truth == 1){
                var = pow(var, 2);
            }
            asym += var;
        }
    }
    return asym;
}

PYBIND11_PLUGIN(make_asym) {
    py::module m("make_asym", "C++ python module for asymmetry calculation");
    m.def("make_asym_func", &make_asym_func, "Cacluate asym value");
    return m.ptr();
}
