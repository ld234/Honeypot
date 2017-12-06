#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream> // istringstream
#include <map>
#include <set>
#include <fstream> 
#include <stdlib.h> // atoi
#include "localmath.h"
#include "EventType.h"
#include "AnalysisEngine.h"
#include "ActivityEngine.h"

using namespace std;

AnalysisEngine::~AnalysisEngine(){

}
//Reset BstatMap for new input
void AnalysisEngine::clearStats(){
	for (itD = EventList.begin(); itD!=EventList.end(); itD++){
		if(itD->second.type == 0 ){
			itD->second.value.valueC = 0;
		}
		else if(itD->second.type==1){
			itD->second.value.valueD = 0;
		}
		else if(itD->second.type==2){
			itD->second.value.valueE = 0;
		}
	}
	EventList.clear();
}

//Reads the baseline.log file
bool AnalysisEngine::readBaseline()
{
	ifstream readFile(bFileName.c_str());
	string line;
	
	//Reset bstatsMap
	clearStats();
	if(!readFile.is_open())
	{
		cerr<<"Error: No File named "<<bFileName<<" found"<<endl;
		return false;
	}
	
	while(!readFile.eof())
	{
		DayStats dstat;
		getline(readFile,line);
		if (readFile.fail()){
			break;
		}
		vector<string>fields =getFields(line);
		int fieldSize = fields.size();
		
		// if the baseline log file does not conform to the correct formatting, the program exits
		if(fieldSize != 6)
		{
			cerr<<"Error: Wrong data format in "<<bFileName<<endl;
			return false;
		}
		//Checks that the day and event num are integer and depending on event type, double or int
		if((!is_int(fields[0]))||(!is_int(fields[1])))
		{
			cerr<<"Error: Baseline.log file is corrupted"<<endl;
			return false;
		}
		if(fields[2]=="C" && !is_double(fields[4]))
		{
			cerr<<"Error: Incorrect value for event type C in baseline.log"<<endl;
			return false;
		}
		if(fields[2]=="E" && !is_int(fields[4]))
		{
			cerr<<"Error: Incorrect value for event type E in baseline.log"<<endl;
			return false;
		}
		
		int dayNoField = atoi(fields[0].c_str());
		int eventNoField =atoi(fields[1].c_str());
		itD = EventList.find(make_pair(dayNoField,eventNoField));
		
		if(itD != EventList.end())
		{
			if(fields[2]=="C" ){
				itD->second.value.valueC += atof(fields[4].c_str());
			}
			else if(fields[2]=="D"){
				itD->second.value.valueD++;
			}
			else if(fields[2]=="E"){
				itD->second.value.valueE += atoi(fields[4].c_str());
			}
			else{
				return false;
			}
		}
		else
		{
			if(fields[2]=="C"){
				dstat.type = DayStats::C;
				dstat.value.valueC = atof(fields[4].c_str());
			}
			else if(fields[2]=="D"){
				dstat.type = DayStats::D;
				dstat.value.valueD = 1;
			}
			else if(fields[2]=="E"){
				dstat.type = DayStats::E;
				dstat.value.valueE = atoi(fields[4].c_str());
			}
			else{
				return false;
			}
			EventList[make_pair(dayNoField, eventNoField)] = dstat;
		}
		
		DayNo.insert(dayNoField);// adds dayNo if its not already in there
		EventNo.insert(eventNoField);// adds eventNo if its not already in there
	}
	readFile.close();
	return true;
}

bool AnalysisEngine::writeDaystat()
{
	ofstream fout (dsFileName.c_str());
	if (!fout.good()){
		cerr << "Error: Cannot write file "<<bFileName<< endl;
		return false;
	}
	//iterats through the day
	for(set<int>::iterator itDN= DayNo.begin(); itDN != DayNo.end(); itDN++)
	{
		int dno= *itDN;
		
		//iterats through the even num
		for(set<int>::iterator itEN= EventNo.begin(); itEN != EventNo.end(); itEN++)
		{
			int eno = *itEN;
			//checks if the event exist in that day and prints it if it exist
			itD = EventList.find(make_pair(dno,eno));
			if(itD != EventList.end())
			{
				if(itD->second.type == 0)
				{
					fout<<dno<<"\t"<<eno<<"\t"<<"C"<<"\t"<<fixed << setprecision(2) << itD->second.value.valueC<<endl;
				}
				else if(itD->second.type == 1)
				{
					fout<<dno<<"\t"<<eno<<"\t"<<"D"<<"\t"<<itD->second.value.valueD<<endl;
				}
				else if(itD->second.type == 2)
				{
					fout<<dno<<"\t"<<eno<<"\t"<<"E"<<"\t"<<itD->second.value.valueE<<endl;
				}
			}
		}
		
	}
	fout.close();
	return true;
}

// Returns a map < eventNo, pair <mean, stdDev> >
map<int, pair<double,double> >* AnalysisEngine::calcMeanSTDev(map<int, string> &eventNumMap, map<string,pair<EventType,bool> >&eventTypeMap)
{
	if(EventList.empty())
	{
		cerr<<"Error: readBaseline function not called"<<endl;
	}
	else
	{
		
		ofstream fout ("BaselineStats.txt");
		if (!fout.good()){
			cerr << "Error: Cannot write file "<<bFileName<< endl;
			exit(1);
		}
		fout<<EventNo.size()<<endl;
		//loops through the event no
		for(set<int>::iterator itEN= EventNo.begin(); itEN != EventNo.end(); itEN++)
		{
			double mean = 0, stDev = 0, sum = 0, count = 0;
			int eno = *itEN;
			//loops through the day and finds the sum of that event no
			for(set<int>::iterator itDN= DayNo.begin(); itDN != DayNo.end(); itDN++)
			{
				int dno= *itDN;
				itD = EventList.find(make_pair(dno,eno));
				if(itD != EventList.end())
				{
					if(itD->second.type == 0){
						sum += itD->second.value.valueC;
						count++;
					}
					else if(itD->second.type == 1){
						sum += itD->second.value.valueD;
						count++;
					}
					else if(itD->second.type == 2){
						sum += itD->second.value.valueE;
						count++;
					} 
				}
			}
			
			//caculate mean
			mean = sum/count;
			
			//loops through the day to find standard deviation 
			for(set<int>::iterator itDN= DayNo.begin(); itDN != DayNo.end(); itDN++)
			{
				int dno= *itDN;
				itD = EventList.find(make_pair(dno,eno));
				if(itD != EventList.end())
				{
					if(itD->second.type == 0){
						stDev += pow(itD->second.value.valueC - mean, 2);
					}
					else if(itD->second.type == 1){
						stDev += pow(itD->second.value.valueD - mean, 2);
					}
					else if(itD->second.type == 2){
						stDev += pow(itD->second.value.valueE - mean, 2);
					} 
				}
			}
			
			//Calculate standard deviation
			stDev = sqrt(stDev / count);
			EventType *E  = &(eventTypeMap.find(eventNumMap.at(eno))->second.first);
			if(E->getType() == 'D' || E->getType() == 'E'){
				int m = lround(mean);
				fout<<eventNumMap.at(eno)<<":"<<m<<":"<< fixed << setprecision(2)<< stDev<<":"<<endl;
				bstatMap[eno].first = round(mean);
				bstatMap[eno].second = stDev;
			}
			else{
				fout<<eventNumMap.at(eno)<<":"<<fixed << setprecision(2)<<mean<<":"<< fixed << setprecision(2)<< stDev<<":"<<endl;
				bstatMap[eno].first = mean;
				bstatMap[eno].second = stDev;
			}
		}
		fout.close();
	}
	
	return &bstatMap;
}

//Seperate the string by '\t'
vector<string> AnalysisEngine::getFields(string line)
{
	vector<string> fields;
	string word;
	istringstream iss(line);
	while(getline(iss, word, '\t')){
		fields.push_back(word);
	}
	return fields;
}

//calculates the daily total of the generated events
map <int, double>* AnalysisEngine::dailyTotal (vector<GeneratedEvent> *v, int size){
	daily.clear();
	for (int i = 0; i < size; i++){
		daily[i] = 0;
	}
	for (vector<GeneratedEvent>:: iterator it = v->begin();it != v->end(); it++){
		if (it->type == 0){
			daily[it->eventTypeNum] += it->value.valueC;
		}
		else if (it->type == 1){
			daily[it->eventTypeNum]++;
		}
		else if (it->type == 2){
			daily[it->eventTypeNum]+= it->value.valueE;
		}
	}
	return &daily;
}

//Checks if string is integer
bool AnalysisEngine::is_int(const string& s){
	
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-'))) return false ;
	char * end ;
	strtol(s.c_str(), &end, 10) ;
	return (*end == 0) ;
}

//Chekcks if string is double
bool AnalysisEngine::is_double(const string& s){
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-'))) return false ;
    char* end ;
    double val = strtod(s.c_str(), &end);
    return (*end == 0);
}
