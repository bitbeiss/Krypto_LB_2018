#include <iostream>
#include <gmpxx.h>

using namespace std;

int main(const int argc, const char * const argv[])
{
  if (argc < 4)
  {
    cerr << "Description: Adds two numbers" << endl;
    cerr << "Usage: " << argv[0] << " <m> <e/d> <N>" << endl;
    return 1;
  }


  const mpz_class m(argv[1]);
  const mpz_class e(argv[2]);
  const mpz_class N(argv[3]);

  mpz_class ret;

  mpz_powm(ret.get_mpz_t(),m.get_mpz_t(),e.get_mpz_t(),N.get_mpz_t());

  cout << "m:"<< m << "->" << ret  << endl;
  return 0;
}
