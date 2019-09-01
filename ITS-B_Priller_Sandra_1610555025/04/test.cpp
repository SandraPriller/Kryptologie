#include <iostream>
#include <gmpxx.h>
#include <sodium.h>
#include <sstream>
#include <iomanip>
#define ADDITIONAL_DATA (const unsigned char *) "Secret"
#define ADDITIONAL_DATA_LEN 6
using namespace std;


/***************************************************/												 */
/*		Author:   Sandra Priller	   */
/*		Date:	  28.02.2019		   */
/*		Program:  Hybrid Encryption,	   */
/*			  Diffie-Hellman	   */					 							 */
/***************************************************/


mpz_class GenerateRandomNumber() {
	mpz_class n;
	gmp_randstate_t prng_state;
	gmp_randinit_default(prng_state);
	gmp_randseed_ui(prng_state, time(NULL));
	mpz_urandomb(n.get_mpz_t(), prng_state, 1024);

	return n;
}

//===============================

void ServerGeneratePartialKey(mpz_class g, mpz_class p) {
	mpz_class S, x;
	x = GenerateRandomNumber();
	mpz_powm(S.get_mpz_t(), g.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t());

	cout << "x: "<< x <<endl;
	cout << "S: " << S <<endl;
}

//===============================

void ClientGeneratePartialKey(mpz_class g, mpz_class p) {
	mpz_class C, y;
	y = GenerateRandomNumber();
	mpz_powm(C.get_mpz_t(), g.get_mpz_t(), y.get_mpz_t(), p.get_mpz_t());

	cout << "y: "<< y <<endl;
	cout << "C: " << C <<endl;
}

//===============================

string ExtractKey(const mpz_class &g_xy) {
 	mpz_class two_256;
 	mpz_ui_pow_ui(two_256.get_mpz_t(), 2, 256);
 	mpz_class g_xy_short;
 	mpz_mod(g_xy_short.get_mpz_t(), g_xy.get_mpz_t(), two_256.get_mpz_t());

 	stringstream s;
 	s << hex << g_xy_short << endl;
 	cout<<s.str();

 	return s.str();
}

//================================

void ServerGenerateSessionKey(mpz_class C, mpz_class x, mpz_class p) {
	mpz_class k;
	mpz_powm(k.get_mpz_t(), C.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t());

	ExtractKey(k);
	cout <<endl;
}

//===============================

void ClientGenerateSessionKey(mpz_class S, mpz_class y, mpz_class p) {
	mpz_class k;
	mpz_powm(k.get_mpz_t(), S.get_mpz_t(), y.get_mpz_t(), p.get_mpz_t());

	ExtractKey(k);
	cout <<endl;
}

//===============================

string ToHex(void *const data, const size_t dataLength, string &dest) {  //adaptiert aus https://tweex.net/post/c-anything-tofrom-a-hex-string/
    unsigned char *byteData = reinterpret_cast<unsigned char*>(data);
    stringstream hexStringStream;

    hexStringStream << hex << setfill('0');
    for(size_t index = 0; index < dataLength; ++index)
        hexStringStream << setw(2) << static_cast<int>(byteData[index]);
		dest = hexStringStream.str();

    return dest;
}

//=================================

unsigned char *FromHex(const string &in, void *const data) {		//https://tweex.net/post/c-anything-tofrom-a-hex-string/
    size_t length = in.length();
    unsigned char *byteData = reinterpret_cast<unsigned char*>(data);

    stringstream hexStringStream; hexStringStream >> hex;
    for(size_t strIndex = 0, dataIndex = 0; strIndex < length; ++dataIndex) {
        const char tmpStr[3] = { in[strIndex++], in[strIndex++], 0 };
        hexStringStream.clear();
        hexStringStream.str(tmpStr);

        int tmpValue = 0;
        hexStringStream >> tmpValue;
        byteData[dataIndex] = static_cast<unsigned char>(tmpValue);
    }

    return byteData;
}

//==================================

void HexStringToArray(const char *const text, unsigned char *const array, const size_t array_size) {
	for(size_t i = 0; i < array_size; i++) {
		const string text_part(text + 2 * i, 2); //Process 2 , ? chars at a time
		stringstream s(text_part);
		s >> hex;
		int value;
		s >> value;
		array[i] = value;
	}
}

//==================================

void ServerEncrypt(const char *const message, const int message_length, const char *const sessKey) {
	if (crypto_aead_aes256gcm_is_available() == 0) {
		abort();
	}

	unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};
	unsigned char ciphertext[message_length + crypto_aead_aes256gcm_ABYTES];
	unsigned long long ciphertext_len;
	string c;

	crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
							(const unsigned char*)message, message_length,
							ADDITIONAL_DATA, ADDITIONAL_DATA_LEN,
							NULL, nonce, (const unsigned char*)sessKey);

	c = ToHex((void *const)ciphertext, sizeof ciphertext, c);
	cout << c <<endl;
}

//===============================

void ClientDecrypt(const unsigned char *ciphertext, const unsigned char *sessKey) {
	const size_t ciphertext_len = strlen((char*)ciphertext)/2;
	unsigned char c[ciphertext_len];
	HexStringToArray((const char *)ciphertext, c, ciphertext_len);

	unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};
	unsigned char decrypted[ciphertext_len - crypto_aead_aes256gcm_ABYTES];
	unsigned long long decrypted_len;

	if(ciphertext_len < crypto_aead_aes256gcm_ABYTES ||
			crypto_aead_aes256gcm_decrypt(decrypted, &decrypted_len,
							NULL,
							c, ciphertext_len,
							ADDITIONAL_DATA,
							ADDITIONAL_DATA_LEN,
							nonce, (const unsigned char*)sessKey) != 0) {

		cout << "Decryption failed" <<endl;

	} else {
		cout << decrypted <<endl;
	}
}

//===============================


int main(const int argc, const char *const argv[]) {
	if(sodium_init() == -1)
		return 1;

	string serverPartK = "ServerGeneratePartialKey", clientPartK = "ClientGeneratePartialKey",
	serverSessK = "ServerGenerateSessionKey", clientSessK = "ClientGenerateSessionKey", serverEnc = "ServerEncrypt",
	clientDec = "ClientDecrypt";

const char *const g_as_text = "\
f7e1a085d69b3ddecbbcab5c36b857b97994afbbfa3aea82f\
9574c0b3d0782675159578ebad4594fe67107108180b44916\
7123e84c281613b7cf09328cc8a6e13c167a8b547c8d28e0a\
3ae1e2bb3a675916ea37f0bfa213562f1fb627a01243bcca4\
f1bea8519089a883dfe15ae59f06928b665e807b552564014\
c3bfecf492a";
const mpz_class g(g_as_text, 16);


const char *const p_as_text = "\
fd7f53811d75122952df4a9c2eece4e7f611b7523cef4400c\
31e3f80b6512669455d402251fb593d8d58fabfc5f5ba30f6\
cb9b556cd7813b801d346ff26660b76b9950a5a49f9fe8047\
b1022c24fbba9d7feb7c61bf83b57e7c6a8a6150f04fb83f6\
d3c51ec3023554135a169132f675f3ae2b61d72aeff222031\
99dd14801c7";
const mpz_class p(p_as_text, 16);


	if(argc < 2 || argc > 4) {
		cerr << "<Programmname> <Operation> oder " <<endl;
		cerr << "<Programmname> <Operation> [<Operand 1> <Operand 2>]" <<endl;
		return 2;

	} else if(argc == 2 && argv[1] == serverPartK) {
		ServerGeneratePartialKey(g, p);

	} else if(argc == 2 && argv[1] == clientPartK) {
		ClientGeneratePartialKey(g, p);

	} else if(argc == 4 && argv[1] == serverSessK) {
		mpz_class C(argv[2]), x(argv[3]);
		ServerGenerateSessionKey(C, x, p);

	} else if(argc == 4 && argv[1] == clientSessK) {
		mpz_class S(argv[2]), y(argv[3]);
		ClientGenerateSessionKey(S, y, p);

	} else if(argc == 4 && argv[1] == serverEnc) {
		const char *const message = argv[2];
		const int message_length = strlen((char*)message);
		const char *const sessKey = argv[3];

		for(size_t i = 0; i < strlen(message); i++) {
			if(!isalpha(message[i])) {
				cerr << "Bitte Nachricht als Text eingegeben!" <<endl;
				return 1;

			}
		}

		ServerEncrypt(message, message_length, sessKey);

	} else if(argc == 4 && argv[1] == clientDec) {
		const unsigned char *cipher = (const unsigned char*)argv[2];
		const unsigned char *sessKey = (const unsigned char*)argv[3];
		ClientDecrypt(cipher, sessKey);

	} else {
		cout << "Falscher Funktionsname!" <<endl;
		return 1;
	}

  return 0;
}
