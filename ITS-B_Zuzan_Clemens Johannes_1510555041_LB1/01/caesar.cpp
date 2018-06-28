#include "caesar.h"
#include <algorithm>
#include <vector>
#include <iterator>
#include <iomanip>
#include <stdio.h>


using namespace std;

void Caesar::set_filename(string dateiname) {
	//filename setter function
    this->filename = dateiname;
}

void Caesar::open_file(void) {
	//open file stream
	//Attention: Make sure to use the filestream .open function witch .c_str()
    this->ifs.open((this->filename).c_str());
}

void Caesar::close_file(void) {
	//close file stream
    if (ifs.is_open()) {
        ifs.close();
    }
}

void Caesar::evaluate(void) {
	//Read characterwise from previously opened file stream,
	//make every character uppercase and check every character to belong
	//to 'A' to 'Z' ACII range (otherwise do not count).
    char currentc;
    std::map<char,double>::iterator icc;
    this->total_number=0;
    this->htab.clear();

    if(ifs.bad()) {
        cerr<<"Error: Input stream not available";
    }

	//Attention: stream MUST always be located on the left hand side!
    while(ifs>>currentc) {
        currentc = toupper(currentc);
        if (currentc>'A' && currentc<'Z') {
            if((icc = (htab.find(currentc))) != htab.end() ) {
                icc->second++;
                this->total_number++;
            }
            else {
                htab.insert(std::pair<char,double> (currentc,1) );
                this->total_number++;
            }
        }
    }
}

bool Caesar::sort_func(pair<char,double>& item1, pair<char,double>& item2) {
	if (item1.second>item2.second)
		return (true);
	else return(false);
}

void Caesar::display_frequencies(void) {
	//Map's can't be sorted. They are ordered by key per default.
	std::vector<std::pair<char,double> >mapVec;
	for(map<char,double>::iterator iter=htab.begin();iter!=htab.end();iter++){
		mapVec.push_back(*iter);
	}
	//Thus: use a vector to sort the result
    std::sort(mapVec.begin(),mapVec.end(),sort_func);
    for(std::vector<std::pair<char,double> >::iterator vecIt=mapVec.begin();vecIt!=mapVec.end(); vecIt++) {
        cout<<vecIt->first<<setprecision(0)<<"  "<<std::fixed<<(((double) vecIt->second)/((double) this->total_number))*100<<" %"<<std::endl;
    }
}

