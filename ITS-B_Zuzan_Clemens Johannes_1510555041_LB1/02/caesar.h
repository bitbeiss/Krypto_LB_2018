
/*
AUTHOR: Mag. Clemens J. Zuzan
YEAR: 2018
*/

#ifndef CAESAR_H
#define CAESAR_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Caesar {
	public:
		Caesar() {
			text="";
			filename_in="";
			filename_out="";
			total_number=1;
			key=0;			//neutral key by default
			htab.clear();
		};

		~Caesar() {
			close_file_in();
			close_file_out();
		};


		void set_filename_in(string);
		void set_filename_out(string);
		void set_key(const char*);
		void open_file_in(void);
		void close_file_in(void);
		void open_file_out(void);
		void close_file_out(void);
		void evaluate(void);
		void display_frequencies(void);
		void encrypt(void);
		void decrypt(void);

	private:
		string text;
		string filename_in;
		string filename_out;
		int key;
		long long int total_number;
		ifstream ifs;
		ofstream ofs;
		map<char,double> htab;

		static bool sort_func(pair<char,double>&,pair<char,double>&);

};

#endif // CAESAR_H
