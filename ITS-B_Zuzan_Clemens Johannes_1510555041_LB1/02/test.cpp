
/*
AUTHOR: Mag. Clemens J. Zuzan
YEAR: 2018
*/

//Haeuffigkeitsanalyse
//LB-HA01


#include <iostream>
#include <map>
#include <string>
#include "caesar.h"

using namespace std;

int main(const int argc, const char * const argv[])
{
  if (argc < 4)
  { cerr<<"Usage:"<<argv[0]<<" + dateiname_in<string>  + dateiname_out<string> + key<int [0;26[>"<<std::endl;
    return 1;
  }

  Caesar csc;

  csc.set_filename_in(argv[1]);
  csc.set_filename_out(argv[2]);

  csc.open_file_in();
  csc.open_file_out();

  //csc.evaluate();
  //csc.display_frequencies();

  csc.set_key(argv[3]);
  csc.encrypt();
  //csc.decrypt();

  csc.close_file_in();
  csc.close_file_out();

  return 0;
}
