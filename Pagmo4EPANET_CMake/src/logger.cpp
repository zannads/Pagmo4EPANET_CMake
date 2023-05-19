
#include "logger.h"

using namespace std;

void logOnFile(const char* fileName, pagmo::population pop) {
	std::vector<double> diams = { 6, 8, 10, 12, 14, 18 };

	ofstream outputFile(fileName);

	if (outputFile.is_open()){ 
		for (int i = 0; i < pop.size(); ++i) {
			outputFile << "id: " << pop.get_ID()[i] << endl;
			outputFile << "dv: ";
			for (int j = 0; j < pop.get_x().at(i).size(); ++j) {
				outputFile << diams[pop.get_x()[i][j]-1] << " ";
			}
			outputFile << endl; 
			outputFile << "fit: ";
			for (int j = 0; j < pop.get_f().at(i).size(); ++j) {
				outputFile << pop.get_f()[i][j] << " ";
			}
			outputFile << endl;
		}
	}

	outputFile.close();
	return;
}