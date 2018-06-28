#include <iostream>
#include <gmpxx.h>

using namespace std;

mpz_class get_prime(gmp_randstate_t &state){
	mpz_class prime;

	mpz_class random_tmp;
	mp_bitcnt_t bits=1024;
	//mp_bitcnt_t bits=6;

	mpz_urandomb(random_tmp.get_mpz_t(),state,bits);
	//Achtung: nie mpz_t selbst deklarieren! Immer mit get_mpz_t()
	mpz_nextprime(prime.get_mpz_t(),random_tmp.get_mpz_t());

	return prime;

}

void get_e(mpz_class &e, mpz_class &phi_N){
	mpz_class gcd;
	e=1;

	//for(mpz_class i=(phi_N-1);i>1;i--){
	for(mpz_class i=2;i<phi_N;i++){
		mpz_gcd(gcd.get_mpz_t(),i.get_mpz_t(),phi_N.get_mpz_t());
		//cout<<"gcd: "<<gcd<<" i: "<<i<<" phi_N: "<<phi_N<<endl;


		//pruefen auf Teilerfremdheit (erfuellt, wenn ggT==1)
		//und eine ungerade natuerliche Zahl finden...
		if(gcd==1 && i%2!=0) {
			//cout<<"i, phi_N:"<<i<<" "<<phi_N<<endl;
			//wenn Laufvariable und phi_N teilerfremd, wert zurueckgeben
			e=i;
			return;
		}
	}

	//nichts gefunden
	cerr<<"Fehler: e konnte nicht korrekt ermittelt werden."<<endl;
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
	//sicherstellen, dass p != q
	while(q==p){
		q = get_prime(state);
	}

	//zum Testen und nur zum Testen...
	//p=17; q=23;

	N = p*q;

	phi_N = (p-1)*(q-1);
	get_e(e,phi_N);

	//cout<<"p: "<<p<<endl<<endl<<"q: "<<q<<endl<<endl<<"phi_N: "<<phi_N<<endl<<endl;

	if (e==1) {
		cerr<<"Suche nach e fehlgeschlagen."<<std::endl;
		exit(1);
	}

	int flag=0;
	flag=mpz_invert(d.get_mpz_t(),e.get_mpz_t(),phi_N.get_mpz_t());
	if (flag==0){
		cerr<<"Kann mit gegebenem e und phi_N nicht zu d invertieren!"<<endl;
		exit(1);
	}


  //cout << "e: " << e << endl << endl << "N: "<< N << endl << endl << "d: " << d << endl << endl << "N: " << N << endl << endl;
  cout<<endl<<"Public key: ("<<e<<", "<<N<< ")"<<endl<<"Private key: ("<<d<<", "<<N<<")"<<endl;
  return 0;
}
