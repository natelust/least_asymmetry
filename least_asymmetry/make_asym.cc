#include "include/pybind11/pybind11.h"
#include "include/pybind11/numpy.h"
#include <cmath>
#include <array>
#include <vector>

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

namespace py = pybind11;
using std::pow;

static double make_asym_func(
    py::array_t<double, py::array::c_style | py::array::forcecast> dataIn,
    py::array_t<double, py::array::c_style | py::array::forcecast> weightsIn,
    int w_truth
    ){
    // Retrieve the numpy buffer objects
    auto data = dataIn.request(),
         weights = weightsIn.request();

    // Check that the input arrays are conditioned appropriately
    if (data.ndim != 2 || weights.ndim !=2)
        throw std::runtime_error("Input arrays must be two dimensional");

    if (data.shape[0] != weights.shape[0] || data.shape[1] != weights.shape[1])
        throw std::runtime_error("Shape of data and distance arrays unequal");

    if (data.shape[0] != data.shape[1])
        throw std::runtime_error("Arrays must be square");

    if (data.shape[0] %2 == 0)
        throw std::runtime_error("Array dimensions must be odd");

    // Determine the radius of the array and calculate how many unique descrete
    // radii that corresponds to, and use that to construct a vector of arrays
    // to contain the results of calcuations for each radial bin
    int halfSize = (data.shape[0]-1)/2;
    int equationDim = (halfSize+1)*halfSize/2+halfSize+1;
    std::vector<std::array<double, 5> > EquationArray(equationDim);
    // Enumerate the indicies for the array corresponding to each radii for
    // more readable code
    enum EquationTerms {weight_sum, sum_weight_mean, quad, lin, count};
    // Declare variables used in actual calculations
    int index, x, y;
    double mu, core, asym, var;
    // Get pointers to the underlying c style array of the numpy objects
    double *dataPtr = (double *) data.ptr,
           *weightsPtr = (double *) weights.ptr;
    // Initialize the asymmetry value to zero
    asym = 0;

    for (int dis_i = 0; dis_i < data.shape[0]; dis_i++){
        for(int dis_j = 0; dis_j < data.shape[1]; dis_j++){
            /*
            Instead of looping over each radial bin in a distance array,
            we make note that we can calculate what position in the set of
            possible radii a given pixel at i,j corresponds to. using that
            information it is possible to simply refer to a vector that contains
            results of calculations for that particular radius, and append to
            those calculations. This cuts down on the number of loops nessisary.
            */
            if (abs(dis_j-halfSize) >= abs(dis_i-halfSize)){
                x = dis_j;
                y = dis_i;
            }
            else {
                x = dis_i;
                y = dis_j;
            }
            index = (abs(x-halfSize)+1)*abs(x-halfSize)/2+abs(y-halfSize);
            // In the determined radial bin, update the calculations for
            // the intermediate step in calculating the asymmetry value
            // corresponding to the current pixel.
            EquationArray[index][weight_sum] +=
                                         weightsPtr[dis_i*data.shape[0]+dis_j];
            EquationArray[index][sum_weight_mean] +=
                                         weightsPtr[dis_i*data.shape[0]+dis_j]*
                                         dataPtr[dis_i*data.shape[0]+dis_j];
            EquationArray[index][quad] += weightsPtr[dis_i*data.shape[0]+dis_j]*
                                         pow(dataPtr[dis_i*data.shape[0]+dis_j],
                                             2);
            EquationArray[index][lin] += weightsPtr[dis_i*data.shape[0]+dis_j]*
                                         dataPtr[dis_i*data.shape[0]+dis_j];
            EquationArray[index][count] += 1;
        }
    }
    for (std::array<double, 5> &subArray: EquationArray){
        // Mu is the sum of data times the weights normalized by the sum of the
        // weights for a given radius, without weighting, this would simply be
        // the average fo the data for a given radius
        mu = subArray[sum_weight_mean] / subArray[weight_sum];
        // This essensially calculates the formula weights*Variance(data-mean)
        // for each raidal bin. The cacluation has been algebraically expanded
        // and rearanged from the form W*(data-mean)*(data-mean). The sums over
        // pixels are computed in double for loop, and the products are comined
        // here.
        core = subArray[quad]-2*mu*subArray[lin]+pow(mu,2)*subArray[weight_sum];
        var = subArray[count]*core/subArray[weight_sum];
        if (w_truth == 1)
            var = pow(var, 2);
        asym += var;
    }
    return asym;
}

// Write the python bindings using pybind11
PYBIND11_PLUGIN(make_asym) {
    py::module m("make_asym", "C++ python module for asymmetry calculation");
    m.def("make_asym_func", &make_asym_func, "Cacluate asym value");
    return m.ptr();
}
