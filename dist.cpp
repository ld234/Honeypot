#include <iostream>
#include <random>
#include <cmath>
 
using namespace std;

int main()
{
	const int nrolls=100;  // number of experiments
  	default_random_engine generator(time(0));
  	normal_distribution<double> distribution(4.0,1.5);

  	long p[100]={};

	for (int i=0; i<nrolls; ++i) {
		double number = distribution(generator);
		if (number>=0)
			p[i] = lround(number);
		else {
			i--;
		}
	}
	
	//cout << "normal_distribution (4.0,1.5):" << endl;
	
	for (int i=0; i<100; ++i) {
	    cout <<  p[i] << endl;
	}
	
	return 0;
}
