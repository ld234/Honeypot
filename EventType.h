#include <string>
#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

using namespace std;

class EventType{
	public:
		EventType();
		void setEventname(string name);
		void setWeight(int w);
		void setMean(double m);
		void setStdDev(double std);
		void sethasMin(bool minbl);
		void sethasMax(bool maxbl);
		void setType(char CDEtype);
		void setMin(int minParam, char CDEtype);
		void setMin(double minParam);
		void setMax(int maxParam, char CDEtype);
		void setMax(double maxParam);
		void setDayNo(int num);
		void setUnit(string units);
		bool hasMinimum();
		bool hasMaximum();
		char getType();
		int getMinDE(char CDEtype);
		double getMinC();
		int getMaxDE(char CDEtype);
		double getMaxC();
		int getDayNo();
		string getEventName();
		string getUnit();
		int getWeight();
		double getMean();
		double getStdDev();
		
	protected:
		string eventName;
		int weight;
		double mean;
		double stdDev;
		bool hasMin;
		bool hasMax;
		string unit;
		enum {C,D,E} type;
		int dayNo;
		union{
			double minC;
			int minD;
			int minE;
		} min;
		union{
			double maxC;
			int maxD;
			int maxE;
		} max;
};



#endif
