#include <string>
#include <iostream>
#include "EventType.h"

using namespace std;

EventType::EventType(){
	
}

void EventType::setEventname(string name){
	eventName = name;
}

void EventType::setWeight(int w){
	weight = w;
}

void EventType::setMean(double m){
	mean = m;
}

void EventType::setStdDev(double std){
	stdDev = std;
}

void EventType::sethasMin(bool minbl){
	hasMin = minbl;
}

void EventType::sethasMax(bool maxbl){
	hasMax = maxbl;
}

void EventType::setType(char CDEtype){
	if(CDEtype == 'C')	{
		type = C;
	}
	else if(CDEtype == 'D')	{
		type = D;
	}
	else{
		type = E;
	}
}
void EventType::setMin(int minParam, char CDEtype){
	if(CDEtype == 'D')	{
		min.minD = minParam;
	}
	else if(CDEtype == 'E')	{
		min.minE = minParam;
	}
	else{
		cout<<"Error: invalid character type for setMin fucntion"<<endl;
	}
}

void EventType::setMin(double minParam){
	min.minC = minParam;
}

void EventType::setMax(int maxParam, char CDEtype){
	if(CDEtype == 'D')	{
		max.maxC = maxParam;
	}
	else if(CDEtype == 'E')	{
		max.maxE = maxParam;
	}
	else{
		cout<<"Error: invalid character type for setMax fucntion"<<endl;
	}
}

void EventType::setMax(double maxParam){
	max.maxD = maxParam;
}

void EventType::setUnit(string units){
	unit =units;
}

void EventType::setDayNo(int num){
	dayNo = num;
}

bool EventType::hasMinimum(){
	return hasMin;
}

bool EventType::hasMaximum(){
	return hasMax;
}

char EventType::getType(){
	char returnVal;
	if(type == C)
	{
		returnVal = 'C';
	}
	else if( type == D)
	{
		returnVal ='D';
	}
	else
	{
		returnVal = 'E';
	}
	return returnVal;
}

double EventType::getMinC(){
	return min.minC;
}

int EventType::getMinDE(char CDEtype){
	int returnVal;
	if(CDEtype != 'D' && CDEtype != 'E')
	{
		returnVal = 0;
	}
	else if (CDEtype =='D')
	{
		returnVal = min.minD;	
	}
	else 
	{
		returnVal = min.minE;
	}
	return returnVal;
}

double EventType::getMaxC(){
	return max.maxD;
}

int EventType::getMaxDE(char CDEtype){
	int returnVal;
	if(CDEtype != 'D' && CDEtype != 'E'){
		returnVal = 0;
	}
	else if (CDEtype =='D')	{
		returnVal = max.maxD;	
	}
	else{
		returnVal = max.maxE;
	}
	return returnVal;
}

int EventType::getDayNo(){
	return dayNo;
}

string EventType::getEventName(){
	return eventName;
}

int EventType::getWeight(){
	return weight;
}

double EventType::getMean(){
	return mean;
}

double EventType::getStdDev(){
	return stdDev;
}

string EventType::getUnit(){
	return unit;
}
