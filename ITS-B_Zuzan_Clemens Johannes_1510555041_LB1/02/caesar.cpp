
/*
AUTHOR: Mag. Clemens J. Zuzan
YEAR: 2018
*/

#include "caesar.h"
#include <algorithm>
#include <vector>
#include <iterator>
#include <iomanip>
#include <stdio.h>


using namespace std;

void Caesar::set_filename_in(string dateiname_in) {
	//filename setter function (input file)
    this->filename_in = dateiname_in;
}

void Caesar::set_filename_out(string dateiname_out) {
	//filename setter function (output file)
	this->filename_out = dateiname_out;
}

void Caesar::set_key(const char* thekey) {
	//key setter function
	int tmp;
	tmp = atoi(thekey);
	if (tmp>=0 && tmp<=25){
		this->key = tmp;}
	else {
		cerr<<"Please choose a key that is within the range of [0;26["<<std::endl;
		exit(1);
	}

}

void Caesar::open_file_in(void) {
	//open file for input stream
	//Attention: Make sure to use the filestream .open function witch .c_str()
    this->ifs.open((this->filename_in).c_str());
}

void Caesar::open_file_out(void) {
	ifstream ifile;
	ifile.open((this->filename_out).c_str());
	if(!ifile){
		//open file for output stream
		this->ofs.open((this->filename_out).c_str());
	}
	else{
		cerr<<"File for output exists. (Overwrite error). Please choose a different filename."<<std::endl;
		exit(1);
	}
}

void Caesar::close_file_in(void) {
	//close file stream for input
    if (ifs.is_open()) {
        ifs.close();
    }
}

void Caesar::close_file_out(void) {
	//close file stream for output
	if (ofs.is_open()) {
		ofs.close();
	}
}

void Caesar::evaluate(void) {
	//Determine character statistics.
	//'A' to 'Z' (and 'a' to 'z') ACII range (otherwise do not count).
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
	//Function displays statistics counted by "evaluate" function.
	//(Call "evaluate" before please!)

	//Map's can't be sorted. They are ordered by key per default.
	std::vector<std::pair<char,double> >mapVec;
	for(map<char,double>::iterator iter=htab.begin();iter!=htab.end();iter++){
		mapVec.push_back(*iter);
	}
	//Thus: use a vector to sort the result
    std::sort(mapVec.begin(),mapVec.end(),sort_func);
    for(std::vector<std::pair<char,double> >::iterator vecIt=mapVec.begin();vecIt!=mapVec.end(); vecIt++) {
        cout<<vecIt->first<<setprecision(1)<<"  "<<std::fixed<<(((double) vecIt->second)/((double) this->total_number))*100<<" %"<<std::endl;
    }
}

void Caesar::encrypt(void) {
	char currentc;
    std::map<char,double>::iterator icc;
    this->total_number=0;
    this->htab.clear();

    if(ifs.bad()) {
        cerr<<"Error: Input stream not available";
    }

    if(ofs.bad()) {
		cerr<<"Error: Output stream not available";
	}

	//encrypt one character at a time
	char current_cipher_c;
	//Attention: stream MUST always be located on the left hand side
    while(ifs>>std::noskipws>>currentc) {
		//cout<<currentc;

        if ((currentc>='A' && currentc<='Z') || (currentc>='a' && currentc<='z')) {
			//...in case of an uppercase letter
			//- reduce code counting to range mod26 (from ASCII)
			//- compute encoded letter (using mod26) and 're inflate' to ASCII
			//- Do modulo 26 on result to obtain a valid letter again.
			currentc = toupper(currentc);

			current_cipher_c = (char) ('A'+((currentc-'A')+key)%26);
			ofs<<current_cipher_c;
		}
		else{
			//Do not modify character if current character not in ASCII range 'A' to 'Z':
			//Write straight to output stream
			ofs<<currentc;
		}
    }
}

void Caesar::decrypt(void) {
	char currentc;
    std::map<char,double>::iterator icc;
    this->total_number=0;
    this->htab.clear();

    if(ifs.bad()) {
        cerr<<"Error: Input stream not available";
    }

    if(ofs.bad()) {
		cerr<<"Error: Output stream not available";
	}

	//decrypt one character at a time
	char current_clear_c;
	//Attention: stream MUST always be located on the left hand side
    while(ifs>>std::noskipws>>currentc) {
		//cout<<currentc;

        if ((currentc>='A' && currentc<='Z') || (currentc>='a' && currentc<='z')) {
			//...in case of a lowercase letter -> uppercase letter
			//reduce code to range mod26
			currentc = toupper(currentc);

			//decryption - in case modulus result is negative subtract from modulus (26):
			if ( (((currentc-'A')-key)%26) >= 0) {
				current_clear_c = (char) ('A'+((currentc-'A')-key)%26);
				ofs<<current_clear_c;
			}
			else {
				//in case the result of the modulo division is negative...
				//subtract the result from the modulus (add negative result to 26 (modulus number))
				current_clear_c = (char) ('A'+(26+(((currentc-'A')-key))%26));
				ofs<<current_clear_c;
			}
		}
		else{
			//Do not modify if current character not in ASCII range 'A' to 'Z'
			ofs<<currentc;
		}
    }
}
