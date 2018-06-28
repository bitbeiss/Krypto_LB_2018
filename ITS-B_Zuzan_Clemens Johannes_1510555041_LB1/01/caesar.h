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
			filename="";
			total_number=0;
		};

		~Caesar() {
			close_file();
		};

		void set_filename(string);
		void open_file(void);
		void close_file(void);
		void evaluate(void);
		void display_frequencies(void);

	private:
		string text;
		string filename;
		long long int total_number;
		ifstream ifs;
		map<char,double> htab;

		static bool sort_func(pair<char,double>&,pair<char,double>&);

};

#endif // CAESAR_H
