/*
AUTHOR: Clemens J. Zuzan
YEAR: 2018
PURPOSE: Signierung eines Dokumentes mit Hilfe der Funktionen von libsodium.

Verwendet (abgewandelt) zum Erstellen dieser Software (Urheberrechtshinweis):
1) https://download.libsodium.org/doc/public-key_cryptography/public-key_signatures.html  (Example - combined mode)
2) HexStringToArray Funktion von Prof.- Dr. A. Unterweger und Dr. F. Knirsch.

*/

#include <iostream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <sstream>
#include <vector>


using namespace std;


//Funktion von Prof. Dr. A.Unterweger und Dr. F. Knirsch.
void HexStringToArray(const char* const text, unsigned char* const array, const size_t arr_size) {
    for(size_t i=0; i < arr_size; i++) {
        const string text_part(text + 2 * i, 2); //Process 2 chars at a time
        stringstream s(text_part);
        s >> hex;
        int value;
        s >> value;
        array[i]=value;
    }
}

void generate_keys(unsigned char* pk,unsigned char* sk) {
    crypto_sign_keypair(pk, sk);
}

int main(const int argc, const char * const argv[]) {
    if (sodium_init() == -1)
        return 1;

	if (argc<2){
		cerr<<"Usage: ./test [<Sign> <Verify>]."<<endl;
		cerr<<"For help on individual parameters for Sign and Verify please try ./test Sign and ./Verify"<<endl;
		return 1;
	}

    string mode=argv[1];

    if (mode=="Sign" && argc != 3) {
        cerr << "Usage: " << argv[0] << " Sign <message>" << endl;
        return 2;
    }

    if (mode=="Verify" && argc != 5) {
        cerr << "Usage: " << argv[0] << " Verify <message> <signature> <public_key>" << endl;
        return 2;
    }

    if (mode=="Verify") {

        //Klartext Nachricht aus den Uebergabeparametern holen und Laenge holen.
        const char* message(argv[2]);
        unsigned long long  message_len = strlen(message);

        //Zeichenketten der Unterschrift fuer libsodium von Hexformat zu unsigned char Werten geordnet in einem Array konvertieren.
        const char* signature_hex((const char*) argv[3]);
        size_t signature_len = crypto_sign_BYTES + message_len;
        unsigned char signature[signature_len];
        HexStringToArray(signature_hex,signature,signature_len);

		//private key ebenfalls von Hexformat zu Array umwandeln
		const char* pk_hex(argv[4]);
		unsigned char pk[crypto_sign_PUBLICKEYBYTES];
		HexStringToArray(pk_hex,pk,crypto_sign_PUBLICKEYBYTES);

        if (crypto_sign_open((unsigned char*) message, (unsigned long long*) &message_len, signature, (unsigned long long) signature_len, pk) != 0) {
            /* Incorrect signature! */
            cerr << "Signature invalid." << endl;
        } else {
            cout << "Signature valid." << endl;
        }
        return(0);
    }


    else if (mode=="Sign") {

        //Nachricht aus den Uebergabeparametern holen und Laenge bestimmen
        const unsigned char* message = (const unsigned char* ) argv[2];
        const unsigned long long  message_len = (unsigned long long) strlen((const char*) message);

        //Schluesselpaar (elliptic curves) generieren
        unsigned char pk[crypto_sign_PUBLICKEYBYTES];
		unsigned char sk[crypto_sign_SECRETKEYBYTES];
        generate_keys(pk,sk);

        unsigned char signed_message[crypto_sign_BYTES + message_len];
        unsigned long long signed_message_len(sizeof(signed_message));

        crypto_sign(signed_message, &signed_message_len, message, message_len, sk);

        cout << "Signed message: " << hex;
        for (size_t i = 0; i < sizeof signed_message; i++)
            cout << setw(2) << setfill('0') << (int)signed_message[i];
        cout << endl;

        cout << "Public key: " << hex;
        for (size_t i = 0; i < sizeof pk; i++)
            cout << setw(2) << setfill('0') << (int)pk[i];
        cout << endl;
        return 0;
    }


    else {
        cerr<<"Mode not valid. Please give Sign or Verify as mode."<<endl;
        return 1;
    }
}
