#include <iostream>
#include <gmpxx.h>
#include <sodium.h>
#include <sstream>
#include <iomanip>
#include "certhelp.h"
using namespace std;


/*************************************************/
/*												 */
/*			Author:   Sandra Priller			 */
/*			Date:	  05.03.2019				 */
/*			Program:  Challenge-Response-		 */
/*					  Protokoll					 */
/*					 							 */
/*************************************************/


void ServerReadPublicKey(mpz_class e, mpz_class N) {
	cout << "Public key: (" << e << ", " << N << ")" <<endl;
}

//==========================

void ClientReadPrivateKey(mpz_class d, mpz_class N) {
	cout << "Private key: (" << d << ", " << N << ")" <<endl;
}

//==========================

void ServerCreateChallenge(mpz_class e, mpz_class N) {
	mpz_class c, r, n;
	n = N-1;
	gmp_randstate_t randst;
	gmp_randinit_default(randst);
	gmp_randseed_ui(randst, (int)time(0));
	mpz_urandomm(r.get_mpz_t(), randst, n.get_mpz_t());
	mpz_powm(c.get_mpz_t(), r.get_mpz_t(), e.get_mpz_t(), N.get_mpz_t());

	cout << "Challenge: " << c <<endl;
	cout << "Random number: " << r <<endl;
}

//==========================

void libsodium_to_GMP(const unsigned char (&libsodium_value) [crypto_hash_sha512_BYTES], mpz_class &hashValue) {
	stringstream s;
	s << hex;
	for(size_t i = 0; i < sizeof libsodium_value; i++) {
		s << setw(2) << setfill('0') << (int)libsodium_value[i];
		const string string_as_hex = s.str();
		mpz_set_str(hashValue.get_mpz_t(), string_as_hex.c_str(), 16);
	}
}

//==========================

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

//==========================

void ClientCreateResponse(mpz_class c, mpz_class d, mpz_class N) {
	mpz_class H, h;
	string temp;
	mpz_powm(H.get_mpz_t(), c.get_mpz_t(), d.get_mpz_t(), N.get_mpz_t());
	stringstream s;
	s << H;
	s >> temp;

	const char *const n = temp.c_str();
	h = calculateHash(n);
	cout << dec << h <<endl;
}

//==========================

void ServerVerifyResponse(mpz_class h, mpz_class r) {
	mpz_class H;
	string temp;
	stringstream s;
	s << r;
	s >> temp;

	const char *const n = temp.c_str();
	H = calculateHash(n);

	if(H == h) {
		cout << "Authenticated successfully" <<endl;

	} else {
		cout << "Authentication failed" <<endl;
	}

}

//==========================


int main(const int argc, const char * const argv[]) {
	if (sodium_init() == -1)
		return 1;

	string servReadPubKey = "ServerReadPublicKey", clientReadPrKey = "ClientReadPrivateKey";
	string servCrChall = "ServerCreateChallenge", clientCrResp = "ClientCreateResponse", servVerifyResp = "ServerVerifyResponse";
	string crt = ".crt", pem = ".pem";

	if(argc < 3 || argc > 5) {
		cerr << "<Programmname> <Operation> <Filename> oder " <<endl;
		cerr << "<Programmname> <Operation> <Operand 1> [<Operand 2> [<Operand 3>]]" <<endl;
		return 2;

	} else if(argc == 3 && argv[1] == servReadPubKey && crt.substr(crt.find_last_of(".") + 1) == "crt") {
		const char *const pem_filename = argv[2];
		mpz_class e, N;
		ReadPublicKeyFromFile(pem_filename, e, N);
		ServerReadPublicKey(e, N);

	} else if(argc == 3 && argv[1] == clientReadPrKey && pem.substr(pem.find_last_of(".") + 1) == "pem") {
		const char *const crt_filename = argv[2];
		mpz_class d, N;
		ReadPrivateKeyFromFile(crt_filename, d, N);
		ClientReadPrivateKey(d, N);

	} else if(argc == 4 && argv[1] == servCrChall) {
		mpz_class e(argv[2]), N(argv[3]);
		ServerCreateChallenge(e, N);

	} else if(argc == 5 && argv[1] == clientCrResp) {
		mpz_class c(argv[2]), d(argv[3]), N(argv[4]);
		ClientCreateResponse(c, d, N);

	} else if(argc == 4 && argv[1]  == servVerifyResp) {
		mpz_class h(argv[2]), r(argv[3]);
		ServerVerifyResponse(h, r);

	} else {
		cerr << "Falscher Funktionsname!" <<endl;
		return 1;
	}

  return 0;
}
