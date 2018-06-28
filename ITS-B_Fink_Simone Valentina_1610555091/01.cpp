/*
 * LB_KA_01.cpp
 *
 *  Created on: Jun 27, 2018
 *      Author: Simone Fink (Hilfe von Clemens J. Zuzan)
 */


#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <sstream>


#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <fstream>
using namespace std;

//Erstellen eines Programms welches ein File engegennimmt, dieses verändert
//und dann solange Leerzeichen hintendran hängt, bis die ersten 4 Ziffern des Hashes
//den ersten 4 Ziffern des Originalhashes entsprechen


//Mit Hilfe von Clemens J. Zuzan
void libsodium_to_GMP(const unsigned char (&libsodium_value) [crypto_hash_sha256_BYTES], mpz_class &GMP_value)
{
	stringstream s;
	s << hex;
	for (size_t i = 0; i < sizeof libsodium_value; i++)
		s << setw(2) << setfill('0') << (int)libsodium_value[i];
	const char * const string_as_hex = s.str().c_str();
	mpz_set_str(GMP_value.get_mpz_t(), string_as_hex , 16);
}

//Mit Hilfe von Clemens J. Zuzan
string hashwert(const string &message){
	unsigned char hash[crypto_hash_sha256_BYTES];

	const size_t message_length = message.length();
	crypto_hash_sha256(hash, (const unsigned char*)message.c_str(), message_length);

	mpz_class hash_mpz;

	libsodium_to_GMP(hash,hash_mpz);
	return hash_mpz.get_str(4); //es werden nur die ersten 4 Ziffern benötigt
}

int faelschen(string origs, string fakes)
{
	string message_orig;
	string message_fake;
	char c;

	ifstream orig;
	orig.open(origs.c_str());
	if(!orig)
	{
		cerr << "Fehler beim öffnen der Originaldatei!" << endl;
		return -1;
	}

	ifstream fake;
	fake.open(fakes.c_str());
	if(!fake)
	{
		cerr << "Fehler beim öffnen der gefälschten Datei!" << endl;
	}
	while(orig >> c)
	{
		message_orig += c;
	}

	while(fake >> c)
	{
		message_fake += c;
	}

	//hashes der beiden dateien werden nun ausgerechnet

	string hash_orig = hashwert(message_orig);
	string hash_fake = hashwert(message_fake);

	//das Inputfile wird nun nicht mehr benötigt
	orig.close();

	//nun werden die hashes miteinander verglichen. wenn sie nicht gleich sind
	//wird ein Leerzeichen ans outputfile angehängt.

	ofstream outputfile("Orig_Fake.html");
	outputfile << message_fake;
	char leer = ' ';
	int leercnt;
	while(hash_orig.compare(hash_fake) != 0)
	{
		outputfile << leer;
		message_fake += leer;
		leercnt++;
		hash_fake = hashwert((const string)message_fake);
	}
	fake.close();

	return leercnt;
}


int main(int argc, char** argv)
{
	string orig(argv[1]);
	string fake(argv[2]);
	int leercnt = faelschen(orig, fake);
	char c;
	ifstream outputfile("Orig_Fake.html");
	while(outputfile >> c)
	{
		cout << c;
	}
	outputfile.close();
	cout << "\n\n leerzeichen die hinzugefügt wurden: " << leercnt << endl;
}




