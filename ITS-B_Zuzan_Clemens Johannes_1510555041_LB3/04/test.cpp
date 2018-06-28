/*
AUTHOR: Clemens J. Zuzan
YEAR: 2018
PURPOSE: 	Implement Diffie-Hellman key-exchange and AES encryption using the
			session key exchanged.
*/

#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <sodium.h>

using namespace std;


//Funktion von Prof. Dr. A.Unterweger und Dr. F. Knirsch.
void HexStringToArray(const char* const text, unsigned char* const array, const size_t arr_size) {
    for(size_t i=0; i < arr_size; i++) {
        const std::string text_part(text + 2 * i, 2); //Process 2 chars at a time
        std::stringstream s(text_part);
        int value;
        s >> std::hex >> value;
        array[i]=value;
    }
}

//Funktion von Prof. Dr. A.Unterweger und Dr. F. Knirsch
//Liefert die letzten 256 bit des Schluessels für den Session key aus dem längeren Gesamtschluessel.
string ExtractKey(const mpz_class &g_xy) {
	mpz_class two_256;
	mpz_ui_pow_ui(two_256.get_mpz_t(), 2, 256);
	mpz_class g_xy_short;
	mpz_mod(g_xy_short.get_mpz_t(), g_xy.get_mpz_t(), two_256.get_mpz_t());
	stringstream s;
	s << hex << g_xy_short << endl; return s.str();
}

int main(const int argc, const char * const argv[])
{


  string OperationList (
		" ServerGeneratePartialKey\n \
ClientGeneratePartialKey\n \
ServerGenerateSessionKey\n \
ClientGenerateSessionKey\n \
ServerEncrypt\n \
ClientEncrypt\n");

  if(argc<=1) {
		//cerr<<"argc:"<<argc<<endl;
		cerr<<"Usage: ./test operation [<operand1> <operand2]"<<endl;
		cerr<<"Following operations are valid:"<<endl<<
		OperationList<<endl;
		cerr<<"For details on the operations individual operands try ./test <operation>"<<endl<<endl;
		return 1;
  }

  //Operations-Auswahl
  string func(argv[1]);

  //Generierung der vom Server an den Client bzw vom Client an den Server zu versendenden Teilschlüssel
  // X und Y sowie der geheim bleibenden (und bei Server bzw. Client) verbleibenden Teilschlüssel x und y.
  if (func=="ServerGeneratePartialKey" || func=="ClientGeneratePartialKey") {
		//keine weiteren Argumente erforderlich

		const char * const g_as_text = "\
		f7e1a085d69b3ddecbbcab5c36b857b97994afbbfa3aea82f\
		9574c0b3d0782675159578ebad4594fe67107108180b44916\
		7123e84c281613b7cf09328cc8a6e13c167a8b547c8d28e0a\
		3ae1e2bb3a675916ea37f0bfa213562f1fb627a01243bcca4\
		f1bea8519089a883dfe15ae59f06928b665e807b552564014\
		c3bfecf492a";
		const mpz_class g(g_as_text , 16);

		const char * const p_as_text = "\
		fd7f53811d75122952df4a9c2eece4e7f611b7523cef4400c\
		31e3f80b6512669455d402251fb593d8d58fabfc5f5ba30f6\
		cb9b556cd7813b801d346ff26660b76b9950a5a49f9fe8047\
		b1022c24fbba9d7feb7c61bf83b57e7c6a8a6150f04fb83f6\
		d3c51ec3023554135a169132f675f3ae2b61d72aeff222031\
		99dd14801c7";
		const mpz_class p(p_as_text , 16);

		//Zufallszahl generieren, damit Primzahl finden.
		gmp_randstate_t rstate;
		gmp_randinit_default(rstate);
		gmp_randseed_ui(rstate, time(NULL));
		mpz_class x;
		//haengt von den bits von p ab; modulus soll ganz abgedeckt werden.
		mp_bitcnt_t bits=1024;

		mpz_urandomb(x.get_mpz_t(),rstate,bits);

		if(func=="ServerGeneratePartialKey") {
			cout<<"x: "<<x<<endl;
			cout<<"g^x: "<<(g^x)<<endl;
			return 0;
		}

		else if(func=="ClientGeneratePartialKey") {
			cout<<"y: "<<x<<endl;
			cout<<"g^y: "<<(g^x)<<endl;
			return 0;
		}
	}

	//
	else if (func=="ServerGenerateSessionKey") {

		//Mangelnde Argumente abfangen
		if(argc<=3) {
			//cerr<<"argc:"<<argc<<endl;
			cerr<<"Usage: ./test ServerGenerateSessionKey [<x> <Y>]"<<endl;
			return 1;
		}

		mpz_class x(argv[2]);
		mpz_class Y(argv[3]);
		string session_key;

		//mit (custom) Extract-Funktion die 256 bit extahieren.
		//Y^x entspricht k = k' entspricht g^(xy)
		session_key = ExtractKey(Y^x);


		//in Hexadezimalformat ausgeben
		cout << hex;
		for (size_t i = 0; i < sizeof session_key; i++) {
			//Fuer hex Ausgabe int-cast: (int) session_key[i] cast weglassen!
			cout << setw(2) << setfill('0') << session_key[i];
		}
		cout << endl;
		return 0;

		}


	else if (func=="ClientGenerateSessionKey") {
		//Mangelnde Argumente abfangen
		if(argc<=3) {
			//cerr<<"argc:"<<argc<<endl;
			cerr<<"Usage: ./test ClientGenerateSessionKey [<y> <X>]"<<endl;
			return 1;
		}

		mpz_class y(argv[2]);
		mpz_class X(argv[3]);
		string session_key;

		//mit (custom) Extract-Funktion die 256 bit extahieren.
		session_key = ExtractKey(X^y);

		//in Hexadezimalformat ausgeben
		cout << hex;
		for (size_t i = 0; i < sizeof session_key; i++) {
			cout << setw(2) << setfill('0') << session_key[i];
		}
		cout << endl;
		return 0;
		}


	else if(func=="ServerEncrypt") {

		//Mangelnde Argumente abfangen
		if(argc<3) {
			//cerr<<"argc:"<<argc<<endl;
			cerr<<"Usage: ./test ServerEncrypt [<message> <session-key>]"<<endl;
			return 1;
		}

		// Encryption with AES using Libsodium as described in:
		// https://download.libsodium.org/doc/secret-key_cryptography/aes-256-gcm.html

		const char* MESSAGE (argv[2]);		//plain
		const unsigned long long m_len(strlen(argv[2]));
		const char* key_hex(argv[3]);		//hex

		const unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};
		unsigned char ciphertext[m_len + crypto_aead_aes256gcm_ABYTES];
		unsigned long long ciphertext_len;

		if (sodium_init() == 1) {
			cerr<<"Error: Libsodium did not initialize correctly!"<<endl;
			return 1;
			}
		if (crypto_aead_aes256gcm_is_available() == 0) {
			cerr<<"Error: Libsodium not available on this CPU."<<endl;
			abort(); /* Not available on this CPU */
		}

		//Session key von Hex-Format zu Array umwandeln.
		unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
		HexStringToArray(key_hex,key,crypto_aead_aes256gcm_KEYBYTES);

		crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
                              (unsigned char*) MESSAGE,				//Achtung: Message muss auf unsigned char* gecastet werden; ansonsten gibt es Problem mit dey Typ des key's(!);tricky!!!
							  m_len,
                              NULL, 0,								//Additional tag data --> Null pointer; length=0
                              NULL,
                              nonce,
							  key);

		//in Hexadezimalformat ausgeben
		cout << hex;
		for (size_t i = 0; i < ciphertext_len; i++) {
			cout << setw(2) << setfill('0') << (int)ciphertext[i];
		}
		cout << endl;


		return 0;
	}

	else if(func=="ClientDecrypt") {

		// Decryption with AES using Libsodium as described in:
		// https://download.libsodium.org/doc/secret-key_cryptography/aes-256-gcm.html

		//Mangelnde Argumente abfangen
		if(argc<3) {
			//cerr<<"argc:"<<argc<<endl;
			cerr<<"Usage: ./test ClientDecrypt [<ciphertext> <session-key>]"<<endl;
			return 1;
		}

		//Convert key from hex to dec
		const char* key_hex(argv[3]);								//hex key
		unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
		HexStringToArray(key_hex,key,crypto_aead_aes256gcm_KEYBYTES);

		//Convert ciphertext from hex to dec
		const char* ciphertext_hex (argv[2]);							//ciphertext
		const unsigned long long ciphertext_hex_len(strlen(argv[2]));
		const unsigned long long ciphertext_len=ciphertext_hex_len/2;
		unsigned char ciphertext[ciphertext_len];
		HexStringToArray(ciphertext_hex,ciphertext,ciphertext_hex_len);

		unsigned char decrypted[ciphertext_len];
		unsigned long long decrypted_len;

		const unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};

		if (ciphertext_len < crypto_aead_aes256gcm_ABYTES){
			cerr<<"Error: Key length exceeds ciphertext length! Message not valid/forged."<<endl;
		}

		if (crypto_aead_aes256gcm_decrypt(
				decrypted,
				&decrypted_len,
				NULL,
				(unsigned char*) ciphertext,
				ciphertext_len,          //toDo: ciphertext_hex_len  - stimmt so nicht! Laenge anpassen bei Konversion!
				NULL,
				0,
				nonce,
				key) != 0) {
			/* message forged! */
			cerr<<"Error: Message forged!"<<endl;
		}
		else {
			//Binaere Null anhängen um korrektes Stringende sicherzustellen.
			//(Hack; funktioniert aber gut so. Ggf. Laengen bei encrypt & decrypt revidieren.)
			decrypted[decrypted_len]='\0';
			cout<<decrypted<<endl;
		}
		return 0;
	}

	else {
		cerr<<"No Operation recognized. Please provide one of the following in your call:"<<endl<<
		OperationList<<endl;
	}

  return 0;
}
