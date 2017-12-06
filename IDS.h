#include <iostream>
#include <string>
#include <vector>
#include "localmath.h"  //huge_val
#include <sstream> // istringstream
#include <map>
#include <utility> //pair
#include <fstream> 
#include <stdlib.h> // atoi
#include "EventType.h"
#include "ActivityEngine.h"
#include "AnalysisEngine.h"
#include "AlertEngine.h"
#ifndef IDS_H
#define IDS_H
using namespace std;


class IDS{
	public:
		bool readEventFile(char* filename);
		bool readStats(char* filename);
		void print();
		void generate(int days,bool);
		void analyse();
		void alert(char* filename,int days);
		bool filesExist(char* filename);
		
	private: 
		//const int defaultMin = -10000000;
		//const int defaultMax = 10000000;
		int eventsNo;
		ActivityEngine ae;
		AnalysisEngine anE;
		AlertEngine alE;
		map<int, string> eventNumMap;
		map<string, pair<EventType,bool> >eventTypeMap;
		map<string, pair<EventType,bool> >::iterator itM;
		map<int, pair <double, double> > * bstat;
		
		// Private functions
		vector<string> getFields(string);
		bool is_double(const string& s);
		bool is_int(const string& s);
		int calcThreshold()	;
};

#endif
