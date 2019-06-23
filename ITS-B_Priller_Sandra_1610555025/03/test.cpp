#include <iostream>
#include <string.h>
#include <iomanip>
#include <gmpxx.h>
#include <sodium.h>
using namespace std;


/*************************************************/
/*												 */
/*		    Author:   Sandra Priller		 	 */
/*			Date:	  27.02.2019				 */
/*			Program:  Libsodium					 */
/*					  Functions	 				 */
/*												 */
/*************************************************/


string ToHex(void *const data, const size_t dataLength, string &dest) {  //adaptiert aus https://tweex.net/post/c-anything-tofrom-a-hex-string/
    unsigned char *byteData = reinterpret_cast<unsigned char*>(data);
    stringstream hexStringStream;

    hexStringStream << hex << setfill('0');
    for(size_t index = 0; index < dataLength; ++index)
        hexStringStream << setw(2) << static_cast<int>(byteData[index]);
		dest = hexStringStream.str();

    return dest;
}

//====================================

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

//===================================

void Sign(const unsigned char *message, const int message_length) {
	unsigned char pk[crypto_sign_PUBLICKEYBYTES];
	unsigned char sk[crypto_sign_SECRETKEYBYTES];
	unsigned char signed_message[crypto_sign_BYTES + message_length];
	unsigned long long signed_message_len;
	string pubKey, signedMess, signature;

	crypto_sign_keypair(pk, sk);
	crypto_sign(signed_message, &signed_message_len, message, message_length, sk);
	signedMess = ToHex(signed_message, signed_message_len, signedMess);
	cout << "Signed message: " <<signedMess <<endl;

	pubKey = ToHex(pk, sizeof pk, pubKey);
	cout << "Public key: " << pubKey <<endl;
	signature = ToHex(signed_message, crypto_sign_BYTES, signature);

}

//====================================

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

//=====================================

void Verify(const unsigned char *message, const int message_length, const unsigned char *signature, const unsigned char *pubkey) {
	const size_t signed_message_len = strlen((char*)signature)/2;
	unsigned char c[signed_message_len];
	HexStringToArray((const char *)signature, c, signed_message_len);

	const size_t pk_len = strlen((char*)pubkey)/2;
	unsigned char pk[pk_len];
	HexStringToArray((const char *)pubkey, pk, pk_len);
	int verify = 0;
	unsigned long long len = (unsigned long long)message_length;

	verify = crypto_sign_open((unsigned char*)message, &len, c, signed_message_len, pk);

	if(verify != 0) {
		cout << "Signature invalid." <<endl;

	} else {
		cout << "Signature valid." <<endl;
	}
}

//====================================

int main(const int argc, const char *const argv[]) {
	if(sodium_init() == -1)
		return 1;

	string sign = "Sign", verify = "Verify";
	string dest;

	if(argc < 3 || argc > 5) {
		cerr << "Usage: " << "<Programmname> <Funktionsname> <message>" << endl;
		cerr << "oder" <<endl;
		cerr << "<Programmname> <Funktionsname> <message> <signature> <Publickey>" <<endl;
		return 2;

	} else if(argc == 3 && argv[1] == sign) {
		const unsigned char *message = (const unsigned char*)argv[2];
		const int message_length = strlen((char*)message);
		dest = ToHex((void *const)message, message_length, dest);
		Sign(message, message_length);

	} else if(argc == 5 && argv[1] == verify) {
		const unsigned char *message = (const unsigned char*)argv[2];
		const int message_length = strlen((char*)message);
		const unsigned char *signature = (const unsigned char*)argv[3];
		const unsigned char *pubkey = (const unsigned char*)argv[4];
		Verify(message, message_length, signature, pubkey);

	} else {
		cerr << "Falscher Funktionsname!" <<endl;
	}

	return 0;
}

