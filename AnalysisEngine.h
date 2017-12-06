#include <map>
#include <set>
#include <vector>
#include <string>
#include "ActivityEngine.h"

#ifndef ANALYSIS_ENGINE_H

#define ANALYSIS_ENGINE_H

using namespace std;

class AnalysisEngine{
	public:
		~AnalysisEngine();
		bool readBaseline();
		bool writeDaystat();
		map<int, pair <double, double> > * calcMeanSTDev(map<int, string> &eventNumMap,map<string,pair<EventType,bool> >&eventTypeMap);
		map <int,double> *dailyTotal(vector<GeneratedEvent> *v,int size);
	private:
		map <int,double> daily;
		string bFileName = "Baseline.log";
		string dsFileName = "dayStat.txt";
		set<int> EventNo;
		set<int> DayNo;
		struct DayStats{
			enum {C,D,E} type;
			union{
				double valueC;
				int valueD;
				int valueE;
			} value;
		};
		map<int, pair <double, double> > bstatMap;
		map< pair< int,int>, DayStats > EventList;
		map< pair< int,int>, DayStats >::iterator itD;
		bool is_double(const string& s);
		bool is_int(const string& s);
		vector<string> getFields(string line);
		void clearStats();
	};

#endif
