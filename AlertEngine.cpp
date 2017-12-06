#include "AlertEngine.h"
#include <vector>
#include <map>
#include <iomanip>

AlertEngine::AlertEngine(){
	anomalyCounter = 0;
	threshold = 0;
}

// Set the threshold
void AlertEngine::setThreshold(int thresholdParam){
	std::cout<<"Threshold:"<<thresholdParam<<endl;
	threshold = thresholdParam;
}


// Calculate the anomaly counter
void AlertEngine::calcAnomalyCounter(map<int,double>* v, std::map<int,string>* eventNoMap, std::map<string,pair<EventType,bool> > *eventTypeMap, map<int, pair <double, double> > * bstat){
	anomalyCounter = 0;
	for (int i= 0; i < eventNoMap->size(); i++){
		EventType * E = &((eventTypeMap->find(eventNoMap->find(i)->second))->second.first);
		double diff = v->find(i)->second > bstat->find(i)->second.first ? v->find(i)->second - bstat->find(i)->second.first :  bstat->find(i)->second.first - v->find(i)->second;
		if (bstat->find(i) != bstat->end())
			anomalyCounter += ((diff)/bstat->find(i)->second.second * E->getWeight());
	}
}

// Check if there are anomalies
bool AlertEngine::hasAnomalies(){
	std::cout << " Anomaly counter: " << fixed << setprecision(2)<< anomalyCounter << std::endl;
	if (anomalyCounter >= threshold)
		return true;
	return false;
}

