#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "EventType.h"

#ifndef ALERT_ENGINE_H
#define ALERT_ENGINE_H
using namespace std;

class AlertEngine{
	public:
		AlertEngine();
		void setThreshold(int threshold);
		void calcAnomalyCounter(map <int, double> * v, map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap, map<int, pair <double, double> > * bstat);
		bool hasAnomalies();
	private: 
		int threshold;
		double anomalyCounter;
};

#endif
