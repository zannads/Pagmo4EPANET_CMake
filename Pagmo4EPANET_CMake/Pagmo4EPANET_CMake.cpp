// Pagmo4EPANET_CMake.cpp : Defines the entry point for the application.
//
#include <chrono>

#include "Pagmo4EPANET_CMake.h"

using namespace pagmo;

struct problem_NET1 {
    // num of obj func 
    vector_double::size_type get_nobj() const {
        return  2;
    }

    // num or eq constraint
    vector_double::size_type get_nec() const {
        return  0;
    }
    // number of inequality constraint
    vector_double::size_type get_nic() const {
        return 0;
    }
    // num of integer decision variables 
    vector_double::size_type get_nix() {
        return 12;
    }

    // Implementation of the objective function.
    vector_double fitness(const vector_double& dv) const
    {
        vector_double dvv = dv;
        vector_double obj_con = NET1sim(dvv);
        //minPNode is a non linear constraint and NSGA2 can't deal with it so I multiply it
        vector_double fit(this->get_nobj());

        int lambda = 10000000;
        fit[0] = obj_con[0];
        fit[1] = obj_con[1] + lambda * obj_con[2];

        return fit;
    }
    // Implementation of the box bounds.
    std::pair<vector_double, vector_double> get_bounds() const
    {
        // available diams = 6  = { 6, 8, 10, 12, 14, 18 }; ->from 1 to 6 (6 would not be included because each number is from 0 to 0.999 
        // pipe to change 12 pipeIds2check = { "10", "11", "12", "21", "22", "31", "110", "111", "112", "113", "121", "122" };
        return { {1., 1., 1., 1.,1., 1., 1., 1.,1., 1., 1., 1.}, {6.999, 6.999, 6.999, 6.999,6.999, 6.999, 6.999, 6.999,6.999, 6.999, 6.999, 6.999} };
    }
};

int main()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    
    try {
        //Construct a pagmo::problem for NET1
        problem p{ problem_NET1{} };
        unsigned int seed = 3;

        // Instantiate nsga2
        algorithm algo{ nsga2(1000) };
        algo.set_seed(seed);

        // Instantiate population 
        population pop{ p, 24, seed };
        logOnFile("inital.txt", pop);

        // evolve 
        pop = algo.evolve(pop);
        logOnFile("final.txt", pop);

        // print to screen 
        std::cout << pop << '\n';
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }

    auto t2 = high_resolution_clock::now();
    /* Getting number of milliseconds as an integer. */
       // auto ms_int = duration_cast<milliseconds>(t2 - t1);

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;

        //std::cout << ms_int.count() << "ms\n";
        std::cout << ms_double.count() << "ms\n";

    /*vector_double pop = {1., 2., 3.};
    try {
        pop = NET1sim(pop);
    }
    catch (const std::exception& e)
    {
        std::cout << "main() failed to open file: " << e.what();
    }*/

    return 0;
}
