#include "IDS.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath> //huge_val
#include <sstream> // istringstream
#include <map>
#include <utility> //pair
#include <fstream> 
#include <stdlib.h> // atoi
#include "EventType.h"
#include "ActivityEngine.h"
#include "AnalysisEngine.h"
//Reads events file
bool IDS::readEventFile(char* filename){
	ifstream readFile(filename);
	string line;
	int count=0;
	
	if(!readFile.good()){
		cerr<<"Error:File "<<filename<<" not found"<<endl;
		return false;
	}
	
	//Checks that the First line of Events.txt file is an int and not a negative value
	getline(readFile,line);
	if (!is_int(line) || atoi(line.c_str()) <= 0){
		cerr << "Error: First line is not a number" << endl;
		return false;
	}

	eventsNo = atoi(line.c_str());
	while(!readFile.eof())	{
		
		getline(readFile,line);
		
		//checks if it is a valid input
		if(readFile.fail()){
			break;
		}
		
		//Seperates the string line by ':'
		vector<string>fields =getFields(line);
		
		//Checks for the correct number of words or empty
		if(fields.size()!= 6 && fields.size()!=0){
			cerr<<"Error: Wrong event input detail"<<endl;
			return false;
		}
		//if file has empty lines, it skips them
		if(fields.size()==0)
		{
			cout<<"empty line"<<endl;
			continue;
		}
		//If a event name is repeated, the program exits
		itM =eventTypeMap.find(fields[0]);
		
		if(itM != eventTypeMap.end()){
			cerr<<"Error:"<<fields[0]<<" event read twice in file"<<endl;
			return false;
		}
		
		if(atof(fields[5].c_str())< 0){
			cerr<<"Error: negative weight detected!"<<endl;
			return false;
		}
		
		//Inserts into the map depending on the event type
		if(fields[1] == "C" ){
		
			if((fields[2] == "" || is_double(fields[2])) && (fields[3] == "" || is_double(fields[3])) &&  is_double(fields[5]))	{
				EventType eventC;
				eventC.setType('C');
				eventC.setEventname(fields[0]);
				eventC.setWeight(atoi(fields[5].c_str()));
				eventC.setUnit(fields[4]);
				
				//If there is no minimum or maximum, a default value is set.
				if(fields[2] == ""){
					eventC.sethasMin(false);	
					eventC.setMin(-10000000);	
				}
				else{
					eventC.sethasMin(true);
					eventC.setMin(atof(fields[2].c_str()));
				}
				if(fields[3]==""){
					eventC.sethasMax(false);
					eventC.setMax(10000000);				
				}
				else
				{
					eventC.sethasMax(true);
					eventC.setMax(atof(fields[3].c_str()));
				}
				eventTypeMap.insert(make_pair(fields[0], make_pair(eventC, false)));
				eventNumMap.insert(make_pair(count,fields[0]));
			}
			else{
				cerr<<"Error:Invalid min, max or weight"<<endl;
				return false;
			}
		}
		else if(fields[1] == "D" )	{
			if((fields[2] == "" || is_int(fields[2])) && (fields[3] == "" || is_int(fields[3])) && is_int(fields[5]))	{
				EventType eventD;
				eventD.setType('D');
				eventD.setEventname(fields[0]);
				eventD.setWeight(atoi(fields[5].c_str()));
				//If there is no minimum or maximum, a default value is set.
				if(fields[2] == "")	{
					eventD.sethasMin(false);
					eventD.setMin(-10000000,'D');				
				}
				else{
					eventD.sethasMin(true);
					eventD.setMin(atoi(fields[2].c_str()),'D');
				}
				if(fields[3]==""){
					eventD.sethasMax(false);
					eventD.setMax(10000000,'D');				
				}
				else{
					eventD.sethasMax(true);
					eventD.setMax(atoi(fields[2].c_str()),'D');
				}
				eventTypeMap.insert(make_pair(fields[0], make_pair(eventD, false)));
				eventNumMap.insert(make_pair(count,fields[0]));
			}
			else
			{
				cerr<<"Error:Invalid min, max or weight"<<endl;
				return false;
			}
			
		}
		else if(fields[1] == "E" ){
			if((fields[2] == "" || is_int(fields[2])) && (fields[3] == "" || is_int(fields[3])) && is_int(fields[5])) {
				EventType eventE;
				eventE.setType('E');
				eventE.setEventname(fields[0]);
				eventE.setWeight(atoi(fields[5].c_str()));
				eventE.setUnit(fields[4]);
				//If there is no minimum or maximum, a default value is set.
				if(fields[2] == "")
				{
					eventE.sethasMin(false);
					eventE.setMin(-10000000,'E');				
				}
				else
				{
					eventE.sethasMin(true);
					eventE.setMin(atof(fields[2].c_str()),'E');
				}
				if(fields[3]=="")
				{
					eventE.sethasMax(false);
					eventE.setMax(10000000,'E');				
				}
				else
				{
					eventE.sethasMax(true);
					eventE.setMax(atof(fields[2].c_str()),'E');
				}
				eventTypeMap.insert(make_pair(fields[0], make_pair(eventE, false)));
				eventNumMap.insert(make_pair(count,fields[0]));
			}
			else
			{
				cerr<<"Error:Invalid min, max or weight"<<endl;
				return false;
			}
			
		}
		else
		{
			cerr<<"Error:Invalid event type "<<fields[1]<<endl;
			return false;
		}
		
		count++;		
	}
	//If the number of events found and number of events stated in the first line is not the same, program exits
	if (count != eventsNo){
		cerr << "Error: The number of events is not correctly specified" << endl;
		return false;
	}
	
	readFile.close();	
	return true;
}

bool IDS::readStats(char* filename){
	
	ifstream readFile(filename);
	string line;
	int count=0;
	if(!readFile.good())	{
		cerr<<"Error:File "<<filename<<" not found"<<endl;
		return false;
	}
	getline(readFile,line);
	
	//Checks that the First line of Stats.txt file is an int and not a negative value
	if (!is_int(line) || atoi(line.c_str()) <= 0){
		cerr << "Error: First line is not a number" << endl;
		return false;
	}
	//Checks that the event number on the first line is same as Events.txt
	int temp = atoi(line.c_str());
	if (temp != eventsNo ){
		cerr << "Error: First line does not match the number of events." << endl;
		return false;
	}
	//set found twice boolean to false intially before reading from the file
	for(itM = eventTypeMap.begin(); itM != eventTypeMap.end(); itM++)
	{
		itM->second.second = false;
	}
	
	while(!readFile.eof()){
		getline(readFile,line);
		//checks if it is a valid input
		if(readFile.fail()){
			break;
		}
		vector<string>fields =getFields(line);
		//Checks for correct number of value per line or empty 
		if(fields.size()!= 3 && fields.size()!=0){
			cerr<<"Error: Wrong event input detail"<<endl;
			return false;
		}
		//if file has empty lines, it skips them
		if(fields.size()==0)
		{
			continue;
		}
		
		//Checks event name is found in Events.txt and exits if not found
		itM =eventTypeMap.find(fields[0]);
		if(itM == eventTypeMap.end())
		{
			cerr<<"Error:"<<fields[0]<<" event not found"<<endl;
			return false;
		}
		
		//Checks that the Event name is not read twice in stats
		if(itM->second.second)
		{
			cerr<<"Error: event "<<fields[0]<<" found twice in stats"<<endl;
			return false;
		}
		
		//Checks that the value is double for both mean and standard deviation and add Standard Deviation and Mean value from stats.txt to eventTypeMap
		if((is_double(fields[1])) && (is_double(fields[2])))
		{
			if(itM->second.first.hasMinimum())
			{
				if(itM->second.first.getType() == 'C')
				{
					if( itM->second.first.getMinC() >= atof(fields[1].c_str()) - atof (fields[2].c_str()) )
					{ 
						cerr<<"Error:"<<fields[0]<<" min > mean - stdev"<<endl;
						return false;
					}
					
				}
				else if(itM->second.first.getType() == 'D' || itM->second.first.getType() == 'E')
				{
					//checks if min is greater than mean
					if( itM->second.first.getMinDE(itM->second.first.getType()) >= atof(fields[1].c_str() )- atof(fields[2].c_str()))
					{ 
						
						cerr<<"Error:"<<fields[0]<<" min > mean - stdev"<<endl;
						return false;
					}
				}
				else
				{
					cerr<<"Error:"<<fields[0]<<" wrong event type"<<endl;
					return false;
				}
			}
			if(itM->second.first.hasMaximum())
			{
				if(itM->second.first.getType() == 'C')
				{
					if( itM->second.first.getMaxC() <= atof(fields[1].c_str())+ atof(fields[2].c_str()))
					{ 
						cerr<<"Error:"<<fields[0]<<" max < mean + stdev"<<endl;
						return false;
					}
					
				}
				else if(itM->second.first.getType() == 'D' || itM->second.first.getType() == 'E')
				{
					//checks if max is less than mean
					if( itM->second.first.getMaxDE(itM->second.first.getType()) <= atof(fields[1].c_str()) + atof(fields[2].c_str())){ 
						cerr<<"Error:"<<fields[0]<<" max < mean + stdev" <<endl;
						return false;
					}
				}
				else
				{
					cerr<<"Error:"<<fields[0]<<" wrong event type"<<endl;
					return false;
				}
			}
			itM->second.second = true;
			itM->second.first.setMean(atof(fields[1].c_str()));
			itM->second.first.setStdDev(atof(fields[2].c_str()));
		}
		else{
			cerr<<"Error: Mean or standard deviation is not a number"<<endl;
			return false;
		}
		count ++;
	}
	//If the number of events found and number of events stated in the first line is not the same, program exits
	if (count != eventsNo){
		cerr << "Error: Input is not equal to event number" << endl;
		return false;
	}
	return true;
};

//Seperates string by ":"
vector<string> IDS::getFields(string line){
	vector<string> fields;
	string word;
	istringstream iss(line);
	while(getline(iss, word, ':'))
	{
		fields.push_back(word);
	}
	return fields;
}
//Checks if string is double
bool IDS::is_double(const string& s){
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-'))) return false ;
    char* end ;
    double val = strtod(s.c_str(), &end);
    return (*end == 0);
}

//Checks if string is integer
bool IDS::is_int(const string& s){
	if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-'))) return false ;
	char * end ;
	strtol(s.c_str(), &end, 10) ;
	return (*end == 0) ;
}

//print eventTypeMap
void IDS::print() {
	cout << "Event number: " << eventsNo << endl;
	for(itM= eventTypeMap.begin(); itM != eventTypeMap.end(); itM++){		
		cout<<itM->second.first.getEventName()<<":"<<itM->second.first.getType()<<":";
		if(itM->second.first.hasMinimum())
		{
			if(itM->second.first.getType()!= 'C')
				cout<<itM->second.first.getMinDE(itM->second.first.getType());
			else
				cout<<itM->second.first.getMinC();
		}
		cout << ":";
		if(itM->second.first.hasMaximum())
		{
			if(itM->second.first.getType()!= 'C')
				cout<<itM->second.first.getMaxDE(itM->second.first.getType());
			else
				cout<<itM->second.first.getMaxC();
		}
		cout << ":";
		if(itM->second.first.getType()!= 'D')
			cout<<itM->second.first.getUnit();
		cout <<":"<<itM->second.first.getWeight()<<":"<<endl;		
	}
}

//Calls analyseEnging
void IDS::analyse()
{
	cout<<"Analysis begins..."<<endl;
	if(anE.readBaseline())
	{
		if(anE.writeDaystat()){
			bstat= anE.calcMeanSTDev(eventNumMap,eventTypeMap);
			cout<<"Analysis Complete"<<endl;
		}
		else{
			cerr<<"Error: Cannot write stat"<<endl;
		}
	}
	else
	{
		cerr<<"Error: Unable to read baseline"<<endl;
	}
}

//generate instances of events for the number of days
void IDS::generate(int days, bool writeOut){
	ae.generate(days,eventsNo,&eventNumMap, &eventTypeMap, writeOut);
	// when finished writing out the stats, clear the map for new stats input
	if (writeOut)
		ae.clearDayStats();
}

//Calls Alert engine
void IDS::alert(char* filename, int days){
	int threshold = calcThreshold();
	alE.setThreshold(threshold);
	if(readStats(filename)){
		generate(days, false);
		for (int i = 0; i  < days ; i++){
			cout << "Day " << i + 1;
			vector<GeneratedEvent> * v = ae.generateEventInstancesPerDay(i,&eventNumMap, &eventTypeMap);
			map<int, double> * Dtotal = anE.dailyTotal(v,eventsNo);
			alE.calcAnomalyCounter(Dtotal, &eventNumMap, &eventTypeMap,bstat);
			if (alE.hasAnomalies()){
				cout << "Intrusion detected for day " << i+1 << endl;
			}
			else{
				cout << "No intrusion detected for day " << i+1 << endl;
			}
			ae.clearDayStats();
		}
	}
	else{
		cerr << "Error: Fail to read "<<filename<< endl;
	}
}
//Calculates threshold
int IDS::calcThreshold(){
	int sum=0;
	int threshold = 0;
	for(itM = eventTypeMap.begin(); itM != eventTypeMap.end(); itM++){
		sum += itM->second.first.getWeight();
		
	}
	threshold = 2* sum;
	return threshold;
	
}

bool IDS::filesExist(char* filename){
	ifstream readFile(filename);
	bool returnVal = true;
	if(!readFile.good()){
		cerr<<"Error:File "<<filename<<" not found"<<endl;
		returnVal = false;
	}
	readFile.close();
	return returnVal;
}
