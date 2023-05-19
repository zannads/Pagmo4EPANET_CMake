#include "NET1.h"

//using namespace pagmo;

void modifyNetwork(EN_Project ph, std::vector<double>& dv) {
	int err;

	std::vector< const char*> id = { "10", "11", "12", "21", "22", "31", "110", "111", "112", "113", "121", "122"};
	std::vector<double> diams = { 6, 8, 10, 12, 14, 18 };
	int idx; 

	for (int i = 0; i < dv.size(); ++i) {
		err = EN_getlinkindex(ph, id[i], &idx);
		err = EN_setlinkvalue(ph, idx, EN_DIAMETER, diams[dv[i]-1]);

		if (err >= 100)
			throw std::runtime_error("not modified");
	}
	
}

std::vector<std::vector<double>> runHydraulics(EN_Project ph) {
    int err;
    err = EN_openH(ph);
    if (err >= 100)
        throw std::runtime_error("hydraulics not opened");

    err = EN_initH(ph, 10);
    if (err >= 100)
        throw std::runtime_error("hydraulics not init");

    bool scheduled;
    int currentTime = 0;
    int timeIndex = 0;
    long t = 0;
    long tStep = 0;

    long hStep;
    err = EN_gettimeparam(ph, EN_REPORTSTEP, &hStep);
    err = EN_gettimeparam(ph, EN_HYDSTEP, &hStep);
    err = EN_settimeparam(ph, EN_REPORTSTEP, hStep);
    //std::cout << "Hydraulic time step is " << hStep << " " << std::endl;

    long duration;
    err = EN_gettimeparam(ph, EN_DURATION, &duration);
    //std::cout << "Duration of the simulation is " << duration <<" " << std::endl;
    
    long lTimestepCount = (duration / hStep) + 1; // the number of expected timesteps we will store.
    int nNodes;
    err = EN_getcount(ph, EN_NODECOUNT, &nNodes);

    std::vector<std::vector<double>> pressures;

    // keeps track of whether any of the timesteps have failed
    bool solutionFailure = false;

    // temp plot patterns
   /* int nPatt;
    double val;
    errorcode = EN_getcount(ph, EN_PATCOUNT, &nPatt);
    for (int pattIdx = 1; pattIdx <= nPatt; ++pattIdx) {
        int nPeriods;
        std::cout << "Pattern n " << pattIdx << std::endl;
        errorcode = EN_getpatternlen(ph, pattIdx, &nPeriods);
        for (int tIdx = 1; tIdx <= nPeriods; ++tIdx) {
            errorcode = EN_getpatternvalue(ph, pattIdx, tIdx, &val);
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }*/

    do {
        currentTime += tStep;
        /* if (currentTime > timeIndex * hStep) {
             std::cout << "this shouldn't happen" << "ct: " << currentTime << "t: " <<t << "ideal: " << timeIndex * hStep << std::endl;
         }*/
         // check if this is a timestep that we are expecting or which has been inserted due to e.g. tank status changing
        scheduled = (currentTime != 0) && (currentTime % hStep == 0);

        // run this timestep
        err = EN_runH(ph, &t);
        _ASSERT(err < 100);

        if (err)
            if (err >= 100) {
                solutionFailure = true;
                throw std::runtime_error("hydraulics not working");
            }
                

        // move the simulator onto the next timestep
        err = EN_nextH(ph, &tStep);
        _ASSERT(err < 100);


        // check to see if this was a scheduled timestep - 
        //  if not then we shouldn't be saving the results and this block will be skipped, unless I have energy for pumps or this type of data to pull out 
        if (scheduled || (currentTime == 0 && tStep == hStep)) {
            // @Dennis - this is where you store your results.
            // use EN_getnodevalue/EN_getlinkvalue to pull the data out of the model
            // the variable "timeIndex" can be used to identify which timestep this is.
            std::vector<double> tmp_pressures(nNodes);
            for (int nodeIdx = nNodes; nodeIdx; --nodeIdx) {
                err = EN_getnodevalue(ph, nodeIdx, EN_PRESSURE, &tmp_pressures[nodeIdx - 1]);
                _ASSERT(err < 100);
            }

            pressures.push_back(tmp_pressures);

            // e.g.
            // error= ENgetnodevalue(nodeIndex, EN_PRESSURE, &pressure[timeIndex]);
            ++timeIndex;
        }


    } while (tStep);

    err = EN_closeH(ph);
    
    return pressures;
}

std::vector<double> NET1sim(std::vector<double>& dv) {

	// create the network
	int err; 

	// load the network 
	EN_Project ph; 
	EN_createproject(&ph);
	err = EN_open(ph, "D:/repos/EPANET/example-networks/Net1.inp", "", "");

	if(err >= 100)
		throw std::runtime_error("not opened");

// change the parameters based on the population
	modifyNetwork(ph, dv);

	/* std::vector< const char*> id = {"10", "11", "12", "21", "22", "31", "110", "111", "112", "113", "121", "122"};
	int idx;
	double value;
	for (int i = 0; i < dv.size(); ++i) {
		err = EN_getlinkindex(ph, id[i], &idx);
		err = EN_getlinkvalue(ph, idx, EN_DIAMETER, &value);

		if (err >= 100)
			throw std::runtime_error("not modified");
		else
			std::cout << value << " ";
	}

	std::cout << std::endl; 
	*/
// run hydraulics 
    std::vector<std::vector<double>> pressures = runHydraulics(ph); 

    // OF min pipe dimension & max avp (min over the day)
// cnt min pressure
    std::vector< const char*> pipeIds2check = { "10", "11", "12", "21", "22", "31", "110", "111", "112", "113", "121", "122" };
    double cumDiam = 0;
    int idx;
    double pipeDiam;
    for (int i = 0; i < pipeIds2check.size(); ++i) {
        err = EN_getlinkindex(ph, pipeIds2check[i], &idx);
        _ASSERT(err < 100);
        err = EN_getlinkvalue(ph, idx, EN_DIAMETER, &pipeDiam);
        _ASSERT(err < 100);

        cumDiam += pipeDiam;
    }

    std::vector< const char*> nodeIds2check = { "10", "11", "12", "13", "21", "22", "23", "31", "32" };
    double minPNode = 0; //doesn't matter the first one will overwrite it 
    double minAvgP = 0; // min Average Zone Pressure over the day
    double aveP; // AZP between every time step
    double P;


    for (int i = 0; i < pressures.size(); ++i) { 
        // first dim time
        aveP = 0;
        for (int j = 0; j < nodeIds2check.size(); ++j) {
            // sec dim nodes
            err = EN_getlinkindex(ph, nodeIds2check[j], &idx); // get the idx which is also the pos in the array

            if ((i == 0 && j == 0) || minPNode > pressures[i][idx]) {
                minPNode = pressures[i][idx];
            }
            aveP += pressures[i][idx] / nodeIds2check.size(); // add weighted pressure to average
        }

        if (i == 0 || minAvgP > aveP) {
            minAvgP = aveP;
        }
    }
   
    std::vector<double> fit;

    fit.push_back( cumDiam ); // to min
    fit.push_back( -minAvgP ); // to max
    fit.push_back( minPNode >= 0 ? 0 : -minPNode); // const, if greater than 0 it is 0, so it will not be added to the objfun, if negative I change the sign so it increases the obj function 
        
    EN_close(ph); 

	EN_deleteproject(ph); 

	return fit;
}