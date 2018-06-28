/*
AUTHOR: Clemens J. Zuzan
YEAR: 2018
PURPOSE: Sign a message/document using mpz calculus. (Without libsodium functions; demonstration - purpose)
USECASE: Use following d,e,N from the Krypto-Lab's RSA keygen (LB2/HA03) to test this software:
Public key: (11, 222717054279107723348776815750191412393601953150233277318727050305533
			77327979866044243199497323528404368833049911941802832106863948559802495321
			28288300818435899233572809024128589065033628519755544267946002118257867576
			08878287774560975370517966853389142702535325065938692832137902718541776682
			95279903868385435684092618306195602938458695163944131069634975226554436302
			84033811308247866408892760647824350116195544105226660061642046609938201365
			18074807741129373760177345863714382780132414388470125723101862953644662037
			80870693111359678214315882481986344724840584694166335743896271129684073568
			128960223634580483449510308341)
Private key: (607410148033930154587573133864158397437096235864272574505619228106001199
			85399634666117816810882350193733181045214386734996655083496072188623603498
			77184050279725182471297338532515631909895962969666185307278504339638843878
			49875748802660101412636872879480097823613816196434996739734686932118226234
			90646913778460141813266339939426780840400897244462256598154751545528051448
			80743566884573146366525018983781782635843907676864425982028981991763081948
			36819600798408121010261454026219813757816866648415888161392644947306730349
			72259775758880616374226831624422546114283258551032806560238565227541471087
			51268735457864401828869091, 2227170542791077233487768157501914123936019531
			50233277318727050305533773279798660442431994973235284043688330499119418028
			32106863948559802495321282883008184358992335728090241285890650336285197555
			44267946002118257867576088782877745609753705179668533891427025353250659386
			92832137902718541776682952799038683854356840926183061956029384586951639441
			31069634975226554436302840338113082478664088927606478243501161955441052266
			60061642046609938201365180748077411293737601773458637143827801324143884701
			25723101862953644662037808706931113596782143158824819863447248405846941663
			35743896271129684073568128960223634580483449510308341)
*/


#include <iostream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <sstream>


#include <iostream>
#include <gmp.h>
#include <gmpxx.h>

using namespace std;

void libsodium_to_GMP(const unsigned char (&libsodium_value) [crypto_hash_sha512_BYTES], mpz_class &GMP_value)
{
stringstream s;
s << hex;
for (size_t i = 0; i < sizeof libsodium_value; i++)
s << setw(2) << setfill('0') << (int)libsodium_value[i]; const char * const string_as_hex = s.str().c_str(); mpz_set_str(GMP_value.get_mpz_t(), string_as_hex , 16);
}


void sign(const string &message,const mpz_class &d,const mpz_class &N){
	unsigned char hash[crypto_hash_sha512_BYTES];

	const size_t message_length = message.length();
	crypto_hash_sha512(hash, (const unsigned char*)message.c_str(), message_length);

	mpz_class hash_mpz;

	libsodium_to_GMP(hash,hash_mpz);

	mpz_class signature;
	mpz_powm(signature.get_mpz_t(),hash_mpz.get_mpz_t(),d.get_mpz_t(),N.get_mpz_t());

	//Nachricht und Signatur ausgeben
	//cout<<signature<<" ["<<message<<"]"<<endl;
	cout<<signature<<endl;
}

void verify(const string &message,const mpz_class &signature_encrypted,const mpz_class &e,const mpz_class &N){
	//Hash berechnen...
	const size_t message_length = message.length();
	unsigned char hash[crypto_hash_sha512_BYTES];
	crypto_hash_sha512(hash, (const unsigned char*)message.c_str(), message_length);

	mpz_class hash_mpz;
	libsodium_to_GMP(hash,hash_mpz);

	//Signatur entschluesseln
	mpz_class signature;
	mpz_powm(signature.get_mpz_t(),signature_encrypted.get_mpz_t(),e.get_mpz_t(),N.get_mpz_t());

    if (signature==hash_mpz) {
		cout<<"Signature valid."<<endl;
    }
    else {
		cout<<"Signature invalid."<<endl;
    }

}

int main(const int argc, const char * const argv[])
{
  if (sodium_init() == -1)
	return 1;



  if(argc<2) {
		cerr<<"Usage: ./test [<Sign> <Verify>] ...\n For details on Sign and Verify parameters see ./test Sign or ./test Verify"<<endl;
		return 1;
  }

  string mode=argv[1];

  if (mode=="Sign" && argc != 5) {
    cerr << "Usage: ./test Sign <message> <d> <N>" << endl;
    return 2;
  }

  if (mode=="Verify" && argc != 6){
    cerr << "Usage: ./test Verify <message> <signature> <e> <N>" << endl;
    return 2;
  }

  if (mode=="Verify"){
		string message = argv[2];
		mpz_class signature(argv[3]);
		mpz_class e(argv[4]);
		mpz_class N(argv[5]);
		verify(message,signature,e,N);
		return(0);
		}

  else if (mode=="Sign"){
		string message = argv[2];
		mpz_class d(argv[3]);
		mpz_class N(argv[4]);
		sign(message,d,N);
		return(0);
  }
  else {
		cerr<<"Mode not valid. Please give Sign or Verify as mode."<<endl;
  }

  return 0;
}
