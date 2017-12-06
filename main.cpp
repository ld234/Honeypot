#include <iostream>
#include <string>
#include <map>
#include <utility> //pair
#include <fstream> 
#include <stdlib.h> // atoi
#include <limits>
#include <cstring>
#include "EventType.h"
#include "IDS.h"
using namespace std;
// Compile: CC -std=c++11 ActivityEngine.cpp AnalysisEngine.cpp AlertEngine.cpp EventType.cpp IDS.cpp main.cpp -o ass3
// Run: ./ass3 Events.txt username Stats.txt Days Stats2.txt Days2
bool is_int(char * s){
	for (int i = 0; i < strlen(s) ; i++){
		if (!isdigit(s[i]))
			return false;
	}
	return true;
}

int main(int argc, char** argv) {
	
	//Checks for the correct number of user arguments
	if (argc<7){
		cerr<< "Error: Invalid command line arguments" << endl;
		exit(1);
	}
	int days, days2;
	//checks to make sure that the day inputs are positive integers
	if (is_int(argv[4]) && is_int(argv[6])){
		days = atoi(argv[4]);
		days2 = atoi(argv[6]);
	}
	else{
		cerr << "Error: Invalid number of days." << endl;
		exit(1);
	}
	
	IDS ids;
	string option;
	string stats;
	string stats2;
	
	if(!(ids.filesExist(argv[1])&&ids.filesExist(argv[3])&&ids.filesExist(argv[5])))
	{
		exit(1);
	}
	if(days <=0)
	{
		cout<<"Error: Invalid number of days"<<endl;
		exit(1);
	}
	//reads Event file
	if(ids.readEventFile(argv[1]))
	{
		cout<<"Read "<<argv[1]<<" complete"<<endl;
		if(ids.readStats(argv[3])){
			cout<<"Read "<<argv[3]<<" complete"<<endl;
			ids.generate(days,true);
			ids.analyse();
			ids.alert(argv[5],days2);
		}
		
		//Repeatedly prompt for new session 
		while(1){
			cout << "New session? (y/n) > ";
			cin >> option;
			//Repeatedly prompt for new stats file and checks for correct input
			while (option != "n" && option != "N" && option !="y" && option != "Y"){
				cin.clear();
				cin.ignore(1000, '\n');
				cerr << "Please type in y or n." << endl;
				cout << "New session? (y/n) > ";
				cin >> option;
			}
			//If user wants to enter new stats data, gets input
			if (option =="y"|| option == "Y"){
				cout << "Enter Stats.txt Days Stats2.txt Days2 > ";
				cin >> stats >> days >> stats2 >> days2;
				if (cin.fail() || days <= 0 || days2 < 0 ) {
					cerr << "Error: Invalid input. " << endl;
					cin.clear();
					cin.ignore(1000, '\n');
					
					continue;
				}
				if(!(ids.filesExist(const_cast<char*>(stats.c_str()))&&ids.filesExist(const_cast<char*>(stats2.c_str()))))
				{
					continue;
				}
				//Checking
				if(ids.readStats(const_cast<char*>(stats.c_str()))){
					cout<<"Read "<<const_cast<char*>(stats.c_str())<<" complete"<<endl;
					ids.generate(days,true);
					ids.analyse();
					ids.alert(const_cast<char*>(stats2.c_str()),days2);
				}
			}
			else if (option == "n" || option == "N"){
				cout << "Quit." << endl;
				exit(0);
			}
		}
	}
	else
	{
		exit(1);
	}
	return 0;
}



