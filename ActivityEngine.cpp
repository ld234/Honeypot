#include <iostream>
#include <random>
#include <unistd.h>
#include <map>
#include <cmath> 
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <new>
#include "EventType.h"
#include "ActivityEngine.h"

using namespace std;

// Destructor
ActivityEngine::~ActivityEngine(){
	clearDayStats();
	genEventList.clear();
}

void ActivityEngine::clearDayStats(){
	delete [] dayStats;
}

void ActivityEngine::generate(int days, int eventNum, map<int,string>* eventNoMap,map<string, pair<EventType,bool> >*eventTypeMap, bool writeOut){
	generator.seed(time(0));
	dayStats = new DayStats[days*eventNum];
	
	// For each event type
	for (int i = 0; i < eventNum; i++){
		EventType * E = &((eventTypeMap->find(eventNoMap->find(i)->second))->second.first);
		
		// If event is of type C
		if ( E->getType() == 'C'){
			double number;
			
			// Provide mean and stdDev
			distribution.param(normal_distribution<double>::param_type (E->getMean(), E->getStdDev()));
			
			// Set day number, type, and value
			for (int j = 0; j < days; j++){
				dayStats[eventNum*j+i].dayNo = j+1;
				dayStats[eventNum*j+i].type = DayStats::C;
				number = distribution(generator);
				if ((E->hasMaximum() && E->getMaxC() < number)|| (E->hasMinimum() && number < E->getMinC()))
					j--;
				else
					dayStats[eventNum*j+i].value.valueC = number;
			}
		}
		
		// If event is of type D
		else if (E->getType() == 'D'){
			double number;
			long total = 0;
			
			// Provide mean and stdDev
			distribution.param(normal_distribution<double>::param_type (E->getMean(), E->getStdDev()));
			
			// Set day number, type, and value
			for (int j = 0; j < days; j++){
				dayStats[eventNum*j+i].dayNo = j+1;
				dayStats[eventNum*j+i].type = DayStats::D;
				number = distribution(generator);
				if ((E->hasMaximum() && E->getMaxDE('D') < lround(number))|| (E->hasMinimum() && lround(number) < E->getMinDE('D'))){
					j--;
				}
				else{
					dayStats[eventNum*j+i].value.valueD = lround(number);
					total+=lround(number);
				}
			}
			
		}
		// If event is of type E
		else if (E->getType() == 'E'){
			double number;
			// Provide mean and stdDev
			distribution.param(normal_distribution<double>::param_type (E->getMean(), E->getStdDev()));
			
			// Set day number, type, and value
			for (int j = 0; j < days; j++){
				dayStats[eventNum*j+i].dayNo = j+1;
				dayStats[eventNum*j+i].type = DayStats::E;
				number = distribution(generator);
				if ((E->hasMaximum() && E->getMaxDE('E') < number)|| (E->hasMinimum() && number < E->getMinDE('E')))
					j--;
				else
					dayStats[eventNum*j+i].value.valueE = lround(number);
			}
		}
	}
	// If generating baseline stats
	if (writeOut){
		generateEventInstances(days, eventNum,eventNoMap,eventTypeMap);
		sortEventList();
		char logFilename[] = "Baseline.log";	
		writeToFile(logFilename,eventNoMap,eventTypeMap);
	}
}

//Generate for each day (Stats2.txt)
vector<GeneratedEvent> *ActivityEngine::generateEventInstancesPerDay(int dayNo,map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap){
	eventList.clear();
	int mapSize = eventNoMap->size();
	
	// For each event in a day
	for (int i = 0; i < mapSize; i++){
		
		// For C event
		if (dayStats[dayNo*mapSize + i].type == 0){
			
			int times = iRandom(1,20);//total number of event instances in a day
			EventType * E = &((eventTypeMap->find(eventNoMap->find(i)->second))->second.first);
			double localTotal = 0;
			double avg = dayStats[dayNo*mapSize + i].value.valueC/double(times); // Average
			double value = dayStats[dayNo*mapSize + i].value.valueC;
			// Range of the next instance to be generated
			double localMin = max (E->getMinC(), avg- (times - 1)*(E->getMaxC() - avg));
			double localMax = avg;
			
			// Get the right number of event instances in d day so that the average is not out of the valid range
			while(avg > E->getMaxC() || avg < E->getMinC()){
				if (avg > E->getMaxC() && E->getMaxC() > 0 || avg < E->getMinC() && E->getMinC() < 0) {
					times = iRandom(times+1,20);
				}
				else if (avg < E->getMinC() || avg > E->getMaxC() && E->getMaxC() < 0){
					times = iRandom(1,times-1);
				}
				avg = dayStats[dayNo*mapSize + i].value.valueC/double(times);
				double localMin = max (E->getMinC(),avg- (times - 1)*(E->getMaxC() - avg));
				double localMax = avg;
			}
			
			// Generate the value of each instance
			for (int j = 1; j <= times; j++){
				
				// Set the type, event ID, time
				GeneratedEvent ge;
				ge.type = GeneratedEvent::C;
				ge.eventTypeNum = i;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				double oldLocalMax = localMax;
				double addedValue = 0;
				
				// Break up the daily total
				if (j == 1 && times > 1){
					ge.value.valueC = fRandom(localMin, localMax);
					addedValue = (avg-ge.value.valueC)/(times-j);
					localMin = max (ge.value.valueC, avg - (times - j)*(E->getMaxC() - avg -addedValue));
					localMax = avg;
				}
				else if (j != times){
					ge.value.valueC = fRandom(localMin, localMax);
					oldLocalMax = localMax;
					addedValue = (avg-ge.value.valueC)/(times-j);
					avg += addedValue;
					
					// Set range for next event instance
					localMin = max (ge.value.valueC,  avg - (times - j)*(E->getMaxC() - avg- addedValue));
					localMax = avg;
				}
				else{
					ge.value.valueC = value - localTotal;
					if (ge.value.valueC > E->getMaxC()){
						ge.value.valueC = E->getMaxC();
					}
					if (ge.value.valueC < E->getMinC()){
						ge.value.valueC = E->getMinC();
					}
				}
				localTotal += ge.value.valueC;
				eventList.push_back(ge);  //Push the generated event into the vector
			}
		}
		
		// For D event
		else if(dayStats[dayNo*mapSize + i].type == 1){
			int times = dayStats[dayNo*mapSize + i].value.valueD;
			// Set the type, event ID, time
			for (int j = 0; j < times; j++){
				GeneratedEvent ge;
				ge.type = GeneratedEvent::D;
				ge.eventTypeNum = i;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				eventList.push_back(ge);
			}
		}
		
		// For E event
		else{
			int times = iRandom(1,20);//total number of event instances in a day
			EventType * E = &((eventTypeMap->find(eventNoMap->find(i)->second))->second.first);
			int localTotal = 0;
			int avg = lround(dayStats[dayNo*mapSize + i].value.valueE/times);  // Average
			int localMin = max (E->getMinDE('E'), avg- (times - 1)*(E->getMaxDE('E') ));
			int localMax = avg;
			int value = dayStats[dayNo*mapSize + i].value.valueE;
			
			// Get the right number of event instances in d day so that the average is not out of the valid range
			while(avg > E->getMaxDE('E') || avg < E->getMinDE('E') ){
				if (avg > E->getMaxDE('E') && E->getMaxDE('E') > 0 || avg < E->getMinDE('E') && E->getMinDE('E') < 0){
					times = iRandom(times+1,20);
				}
				else if (avg < E->getMinDE('E') && E->getMinDE('E') > 0 || avg > E->getMaxDE('E') && E->getMaxDE('E') < 0){
					times = iRandom(1,times-1);
				}
				avg = dayStats[dayNo*mapSize + i].value.valueE/times;
				// Set range for next event instance
				int localMin = max (E->getMinDE('E'), avg- (times - 1)*(E->getMaxDE('E') - avg));
				int localMax = avg;
			}
			
			// Generate value of each instance
			for (int j = 1; j <= times; j++){
				
				// Set the type, event ID, time
				GeneratedEvent ge;
				ge.type = GeneratedEvent::E;
				ge.eventTypeNum = i;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				int addedValue = 0;
				
				// Break up the daily total
				if (j == 1 && times > 1){
					ge.value.valueE = iRandom(localMin, localMax);
					addedValue = lround((avg-ge.value.valueE)/(times-j));
					avg += addedValue;
					// Set range for next event instance
					localMin = max (ge.value.valueE, avg- (times - j)*(E->getMaxDE('E') - avg));
					localMax = avg;
				}
				else if (j != times){
					
					ge.value.valueE = iRandom(localMin, localMax);
					addedValue = lround((avg-ge.value.valueE)/(times-j));
					avg += addedValue;
					// Set range for next event instance
					localMin = max (ge.value.valueE,  avg- (times - j)*(E->getMaxDE('E') - avg));
					localMax = avg;
				}
				else{
					ge.value.valueE = value - localTotal;
				}
				localTotal += ge.value.valueE;
				eventList.push_back(ge);
			}
		}
	}

	return &eventList;
}

// Generate for a number of days and write to log file
void ActivityEngine::generateEventInstances(int days, int eventNum,map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap){
	srand(time(NULL));
	genEventList.clear();
	for (int i = 0; i < days*eventNum; i++){
		if (i%eventNum == 0) {
			string printStr (to_string(i/eventNum+1)+ "...  ");
			cout << "Generating day " << setw(8) << left << printStr;
		}
		
		// Type C
		if (dayStats[i].type == 0){
			int times = iRandom(1,20);//total number of events in a day
			EventType * E = &((eventTypeMap->find(eventNoMap->find(i%eventNum)->second))->second.first);
			double localTotal = 0;
			double avg = dayStats[i].value.valueC/double(times);  // Average
			double localMin = max (E->getMinC(), avg- (times - 1)*(E->getMaxC() - avg));
			double localMax = avg;
			double value = dayStats[i].value.valueC;
			
			// Get the right number of instances per day
			while(avg > E->getMaxC() || avg < E->getMinC()){
				
				if (avg > E->getMaxC() && E->getMaxC() > 0 || avg < E->getMinC() && E->getMinC() < 0) {
					times = iRandom(times+1,20);
				}
				else if (avg < E->getMinC() || avg > E->getMaxC() && E->getMaxC() < 0){
					times = iRandom(1,times-1);
				}
				avg = dayStats[i].value.valueC/double(times);
				
				// Set range for next instance
				double localMin = max (E->getMinC(), avg- (times - 1)*(E->getMaxC() - avg));
				double localMax = avg;
			}
			
			// Generate value for each event instance
			for (int j = 1; j <= times; j++){
				
				// Set the type, event ID, time
				GeneratedEvent ge;
				ge.type = GeneratedEvent::C;
				ge.eventTypeNum = i%eventNum;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				
				double value = dayStats[i].value.valueC;
				double addedValue = 0;
				
				// Break up the daily total;
				if (j == 1 && times > 1){
					ge.value.valueC = fRandom(localMin, localMax);
					addedValue = (avg-ge.value.valueC)/(times-j);
					
					// Set range for next event instance
					localMin = max (ge.value.valueC, avg - (times - j)*(E->getMaxC() - avg -addedValue));
					localMax = avg;
				}
				else if (j != times){
					ge.value.valueC = fRandom(localMin, localMax);
					addedValue = (avg-ge.value.valueC)/(times-j);
					avg+= addedValue;
					
					// Set range for next event instance
					localMin = max (ge.value.valueC,  avg - (times - j)*(E->getMaxC() - avg- addedValue));
					localMax = avg;
				}
				else{
					ge.value.valueC = value - localTotal;
					// Make sure value does not exceed max and min
					if (ge.value.valueC > E->getMaxC()){
						ge.value.valueC = E->getMaxC();
					}
					if (ge.value.valueC < E->getMinC()){
						ge.value.valueC = E->getMinC();
					}
				}
				
				localTotal += ge.value.valueC;
				genEventList[dayStats[i].dayNo].push_back(ge); // Add to generated event list
				
			}
		}
		// Type D
		else if(dayStats[i].type == 1){
			int times = dayStats[i].value.valueD;
			for (int j = 0; j < times; j++){
				GeneratedEvent ge;
				ge.type = GeneratedEvent::D;
				ge.eventTypeNum = i%eventNum;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				genEventList[dayStats[i].dayNo].push_back(ge); // Add to generated event list
			}
		}
		
		// Type E
		else{
			int times = iRandom(1,20);//total number of events in a day
			EventType * E = &((eventTypeMap->find(eventNoMap->find(i%eventNum)->second))->second.first);
			int localTotal = 0;
			int avg = lround(dayStats[i].value.valueE/times); // Average
			int localMin = max (E->getMinDE('E'), avg- (times - 1)*(E->getMaxDE('E') ));
			int localMax = avg;
			int value = dayStats[i].value.valueE;
			
			// Get the right number of instances per day
			while(avg > E->getMaxDE('E') || avg < E->getMinDE('E') ){
				if (avg > E->getMaxDE('E') && E->getMaxDE('E') > 0 || avg < E->getMinDE('E') && E->getMinDE('E') < 0){
					times = iRandom(times+1,20);
				}
				else if (avg < E->getMinDE('E') && E->getMinDE('E') > 0 || avg > E->getMaxDE('E') && E->getMaxDE('E') < 0){
					times = iRandom(1,times-1);
				}
				avg = dayStats[i].value.valueE/times;
				
				// Set range for next instance
				int localMin = max (E->getMinDE('E'), avg- (times - 1)*(E->getMaxDE('E') - avg));
				int localMax = avg;
			}
			
			// Break up the daily total
			for (int j = 1; j <= times; j++){
				GeneratedEvent ge;
				ge.type = GeneratedEvent::E;
				ge.eventTypeNum = i%eventNum;
				ge.time = iRandom(0,23)*100 + iRandom(0,59);
				
				int addedValue = 0;
				if (j == 1 && times > 1){
					ge.value.valueE = iRandom(localMin, localMax);
					addedValue = lround((avg-ge.value.valueE)/(times-j));
					avg += addedValue;
					
					// Set range for next event instance
					localMin = max (ge.value.valueE, avg- (times - j)*(E->getMaxDE('E') - avg));
					localMax = avg;
				}
				else if (j != times){
					
					ge.value.valueE = iRandom(localMin, localMax);
					addedValue = lround((avg-ge.value.valueE)/(times-j));
					avg += addedValue;
					
					// Set range for next event instance
					localMin = max (ge.value.valueE,  avg- (times - j)*(E->getMaxDE('E') - avg));
					localMax = avg;
				}
				else{
					ge.value.valueE = value - localTotal;
				}
				localTotal += ge.value.valueE;
				genEventList[dayStats[i].dayNo].push_back(ge);  // Add to the generated event list
			}
		}
		if (i%eventNum == eventNum-1) {
			cout << "Done." << endl;;
		}
	}
}

// Sort the event in chronological order
void ActivityEngine::sortEventList(){
	for (int i = 0; i < genEventList.size(); i++){
		sort (genEventList[i].begin(),genEventList[i].end());
	}
}

// Random int in range min to max
inline int ActivityEngine::iRandom(int min, int max){ 
	double f = double(rand()) /RAND_MAX;
   	return min + f* (max - min+1);
}

// Random double in range fMin to fMax
inline double ActivityEngine::fRandom(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

// Write baseline stats to file
void ActivityEngine::writeToFile(char filename[],map<int,string>* eventNoMap,map<string,pair<EventType,bool> > *eventTypeMap){
	ofstream fout (filename);
	if (!fout.good()){
		cerr << "Error: Cannot write file."<< endl;
	}
	EventType * E;
	for (map < int, vector<GeneratedEvent> >::iterator it = genEventList.begin(); it != genEventList.end(); it++){
		for (vector<GeneratedEvent>::iterator it2 = it->second.begin(); it2 < it->second.end() ; it2++){
			E = &(eventTypeMap->find(eventNoMap->find(it2->eventTypeNum)->second)->second.first);
			
			int minutes = it2->time%100;
			if (it2->type == GeneratedEvent::C){
				fout << it->first << "\t" << it2->eventTypeNum << "\t" << 'C' << "\t"<< setfill('0')<<setw(2)<< it2->time/100<<":"<< setfill('0')<<setw(2)<<minutes << "\t" << fixed <<setprecision(2) << it2->value.valueC << "\t";
				if (E->getUnit() == ""){
					fout << "N/A" << endl;
				}
				else{
					fout << E->getUnit() << endl;
				}
			}
			else if (it2->type == GeneratedEvent::D){
				fout << it->first << "\t" << it2->eventTypeNum << "\t" << 'D' << "\t"<< setfill('0')<<setw(2)<< it2->time/100<<":"<< setfill('0')<<setw(2)<<minutes  << "\t\t";
				if (E->getUnit() == ""){
					fout << "N/A" << endl;
				}
				else{
					fout << E->getUnit() << endl;
				}
			}
			else{
				fout << it->first << "\t" << it2->eventTypeNum << "\t" << 'E' << "\t"<< setfill('0')<<setw(2)<<it2->time/100<<":"<< setfill('0')<<setw(2)<<minutes << "\t" <<fixed <<setprecision(2) << it2->value.valueE << "\t";
				if (E->getUnit() == ""){
					fout << "N/A" << endl;
				}
				else{
					fout << E->getUnit() << endl;
				}
			}
		}
	}
	fout.close();
	genEventList.clear();
}
