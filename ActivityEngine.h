#include <random>
#include <ctime>
#include <map>
#include <string>
#include "EventType.h"
#ifndef ACTIVITY_ENGINE_H
#define ACTIVITY_ENGINE_H
using namespace std;

struct GeneratedEvent {
	enum {C,D,E} type;
	int eventTypeNum;
	int time; //between 0 to 2399
	union{
		int valueE;
		double valueC;
	} value;
	bool operator<(const GeneratedEvent& a) const{
		return time < a.time;
	};
};
		
class ActivityEngine{
	public:
		//ActivityEngine(map<int,string> *,map<string, pair<EventType,bool> > *);
		~ActivityEngine();
		void generate(int days, int eventNum, map<int,string>*eventNoMap,map<string, pair<EventType,bool> >*eventTypeMap, bool writeOut);
		vector<GeneratedEvent> *generateEventInstancesPerDay(int dayNo,map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap);
		void clearDayStats();
	private:
		
		struct DayStats{
			enum {C,D,E} type;
			int dayNo;
			union{
				double valueC;
				int valueD;
				int valueE;
			} value;
		};
		default_random_engine generator;
		DayStats *dayStats;
  		normal_distribution<double> distribution;
  		map< int, vector<GeneratedEvent> > genEventList;
		vector<GeneratedEvent> eventList;
  		int iRandom(int min, int max);
  		double fRandom(double fMin, double fMax);
  		void sortEventList();
  		void generateEventInstances(int days, int eventNum,map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap);
  		void writeToFile(char filename[],map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap);
		
  		//map<int,string> *eventNoMap;
  		//map<string, pair<EventType,bool> > *eventTypeMap;
  		
};

#endif
