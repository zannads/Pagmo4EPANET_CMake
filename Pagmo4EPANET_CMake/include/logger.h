#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <pagmo/population.hpp>


void logOnFile(const char* fileName, pagmo::population pop);