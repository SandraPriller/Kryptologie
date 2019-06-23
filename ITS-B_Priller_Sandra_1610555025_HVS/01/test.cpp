#include <iostream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <gmpxx.h>
#include <sstream>
//#include "dbghelp.h"
using namespace std;


/*************************************************/
/*			Author:   Sandra Priller			 */
/*			Date:	  25.02.2019				 */
/*			Program:  Sign and Verify	 		 */
/*					  a Message					 */
/*												 */
/*************************************************/


void libsodium_to_GMP(const unsigned char (&libsodium_value) [crypto_hash_sha512_BYTES], mpz_class &hashValue) {
	stringstream s;
	s << hex;
	for(size_t i = 0; i < sizeof libsodium_value; i++)
		s << setw(2) << setfill('0') << (int)libsodium_value[i];
		const string string_as_hex = s.str();
		mpz_set_str(hashValue.get_mpz_t(), string_as_hex.c_str(), 16);

}

//=====================================

mpz_class calculateHash(const char *const n) {
	const size_t len = strlen(n);
	unsigned char hash[crypto_hash_sha512_BYTES];
	crypto_hash_sha512(hash, (const unsigned char*)n, len);
	cout << hex;

	/*for (size_t i = 0; i < sizeof hash; i++)
		cout << setw(2) << setfill('0') <<(int)hash[i];
		cout << endl;*/
		mpz_class hashValue;
		libsodium_to_GMP(hash , hashValue);

	return hashValue;
}

//======================================

mpz_class Sign(mpz_class signature, mpz_class hashValue, mpz_class d, mpz_class N, const char *const message) {
	hashValue = calculateHash(message);
	//cout << "Hash :" << hashValue <<endl;
	mpz_powm(signature.get_mpz_t(), hashValue.get_mpz_t(), d.get_mpz_t(), N.get_mpz_t());
	//cout << "Sig: " << signature <<endl;
	cout << dec << signature <<endl;

	return signature;
}

//=====================================

mpz_class Verify(mpz_class signature, mpz_class e, mpz_class N, const char *const message) {
	mpz_class hashValue = calculateHash(message);
	//cout <<endl << "Hash: " << hashValue <<endl;
	mpz_class verify;
	mpz_powm(verify.get_mpz_t(), signature.get_mpz_t(), e.get_mpz_t(), N.get_mpz_t());
	//cout << "Verify: " << verify <<endl;

	if(verify == hashValue) {
		cout << "Signature valid" <<endl;

	} else {
		cout << "Signature invalid" <<endl;
	}

	return verify;
}


int main(const int argc, const char *const argv[]) {
	if(sodium_init() == -1)
		return 1;

	string paraSign = "Sign";
	string paraVerify = "Verify";

	if(argc < 5 || argc > 6) {
		cerr << "Usage: " << " <Programmname> " << " <Sign> " << " <Nachricht> ";
		cerr << " <d> " << " <N>" << endl;
		cout << "oder" <<endl;
		cerr << "Usage: " << " <Programmname> " << " <Verify> " << " <Nachricht> ";
		cerr << " <signature> " << " <e> " << " <N>" << endl;
		cout <<endl;
		return 1;

	} else if(argc == 5 && argv[1] == paraSign) {
		const char *const message = argv[2];
		mpz_class signature, d(argv[3]), N(argv[4]);
		mpz_class hashValue;
		signature = Sign(signature, hashValue, d, N, message);

	} else if(argc == 6 && argv[1] == paraVerify) {
		const char *const message = argv[2];
		mpz_class m(argv[2]), signature(argv[3]), e(argv[4]), N(argv[5]);
		mpz_class verify;
		verify = Verify(signature, e, N, message);

	} else {
		cerr << "Falscher Funktionsname!" <<endl;
	}

  return 0;
}

