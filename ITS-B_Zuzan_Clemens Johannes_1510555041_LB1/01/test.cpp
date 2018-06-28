//Haeuffigkeitsanalyse
//LB-HA01


#include <iostream>
#include <map>
#include <string>
#include "caesar.h"

using namespace std;

int main(const int argc, const char * const argv[])
{
  if (argc < 2)
  { cerr<<"Usage:"<<argv[0]<<" + dateiname"<<std::endl;
    return 1;
  }

  Caesar csc;
  csc.set_filename(argv[1]);
  csc.open_file();
  csc.evaluate();
  csc.close_file();
  csc.display_frequencies();

  return 0;
}
