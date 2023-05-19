#pragma once

#include <vector> 
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <utility>
#include <stdexcept>

#include "epanet2_2.h"

//#include <pagmo/types.hpp>

//using namespace pagmo;

void modifyNetwork(EN_Project ph, std::vector<double>& dv);

std::vector<std::vector<double>> runHydraulics(EN_Project ph);

std::vector<double> NET1sim(std::vector<double>& dv);