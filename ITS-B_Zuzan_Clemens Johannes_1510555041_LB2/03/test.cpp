
/*
AUTHOR: Mag. Clemens J. Zuzan
YEAR: 2018
*/

#include <iostream>
#include <gmpxx.h>

using namespace std;

//source: http://www.oxfordmathcenter.com/drupal7/node/58
void my_invert(mpz_ptr d, mpz_srcptr e, mpz_srcptr phi_N){
	//was ist was bei den Linearfaktoren? lt. Skript:
	mpz_class a(e),b(phi_N);

	mpz_class q(0),x(0),y(1);
	mpz_class b_tmp,x_tmp,y_tmp;
	mpz_class xlast(1),ylast(0);

	while(b!=0) {
		/*
		cout<< endl << "a:" << a << endl;
		cout<< endl << "b:" << b << endl;
		cout<< endl << "q:" << q << endl;
		cout<< endl << "xlast:" << xlast << endl;
		cout<< endl << "ylast:" << ylast << endl;
		*/

		// a div b
		q = a/b;

		//(a,b)←(b,a mod b)
		b_tmp = b;
		b = a % b;
		a = b_tmp;

		//(x,xlast)←(xlast−q⋅x,x)
		x_tmp = x;
		x = xlast-q*x;
		xlast = x_tmp;

		//(y,ylast)←(ylast−q⋅y,y)
		y_tmp = y;
		y = ylast-q*y;
		ylast = y_tmp;
	}

	// mit Bezout's identitaet (gcd(a*xlast + b*ylast) = 1)
	// ist a⁻1 = xlast
	mpz_set(d,xlast.get_mpz_t());
}

mpz_class get_prime(gmp_randstate_t &state){
	mpz_class prime;

	mpz_class random_tmp;
	mp_bitcnt_t bits=1024;
	mpz_urandomb(random_tmp.get_mpz_t(),state,bits);
	//Achtung: nie mpz_t selbst deklarieren! Immer mit get_mpz_t()
	mpz_nextprime(prime.get_mpz_t(),random_tmp.get_mpz_t());

	return prime;

}

void get_e(mpz_class &e, mpz_class &phi_N){
	mpz_class gcd;
	e=1;

	for(mpz_class i=2;i<phi_N;i++){
		mpz_gcd(gcd.get_mpz_t(),i.get_mpz_t(),phi_N.get_mpz_t());

		//pruefen auf Teilerfremdheit pruefen (erfuellt, wenn ggT==1);
		//und eine ungerade natuerliche Zahl finden...
		if(gcd==1 && i%2!=0) {
			//wenn Laufvariable und phi_N teilerfremd: Wert zurueckgeben
			e=i;
			return;
		}
	}
}

int main(const int argc, const char * const argv[])
{
  if (argc < 1)
  {
    cerr << "Usage: " << argv[0] << endl;
    return 1;
  }

	mpz_class q;
	mpz_class p;

	mpz_class e;
	mpz_class d;
	mpz_class N;

	mpz_class phi_N;

	//Zufallsgenerator 1x initialisieren (nicht in Funktion; nur einmal aufrufen!)
	gmp_randstate_t state;
	gmp_randinit_default(state);

	p = get_prime(state);
	q = p;
	//Sicherstellen, dass p != q
	while(q==p){
		q = get_prime(state);
	}

	N = p*q;

	phi_N = (p-1)*(q-1);
	get_e(e,phi_N);
	if (e==1) {
		cerr<<"Suche nach e fehlgeschlagen."<<std::endl;
		exit(1);
	}

	//phi_N = 168;e = 1239;  // zum Testen
	//mpz_invert(d.get_mpz_t(),e.get_mpz_t(),phi_N.get_mpz_t());
	my_invert(d.get_mpz_t(),e.get_mpz_t(),phi_N.get_mpz_t());


  //cout << endl << "e: " << e << endl << endl << "N: "<< N << endl << endl << endl << "d: " << d << endl <<endl << "N: " << N << endl;
  cout<<endl<<"Public key: ("<<e<<", "<<N<< ")"<<endl<<"Private key: ("<<d<<", "<<N<<")"<<endl;
  return 0;

}
