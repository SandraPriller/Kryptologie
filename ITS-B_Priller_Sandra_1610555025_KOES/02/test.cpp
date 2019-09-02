#include <iostream>
#include <gmpxx.h>
#include <vector>
#include <math.h>
using namespace std;


/*********************************************************/
/*		  Author:   Sandra Priller		 */
/*		  Date:	    24.02.2019			 */
/*		  Program:  Generation of RSA-Key	 */
/*					 		 */							 
/*********************************************************/


mpz_class countDigit(mpz_class N) {			//Funktion adaptiert aus
    mpz_class count = 0;				//https://www.geeksforgeeks.org/program-count-digits-integer-3-different-methods/

    while(N != 0) {
        N = N/10;
        ++count;
    }

    return count;
}

//=================================

void calculateBitsInDigits(mpz_class e, mpz_class d, mpz_class N) {
	mpz_class countSum = 0;
	countSum = countDigit(N) + countDigit(e);
	countSum = 0;
	countSum = countDigit(N) + countDigit(d);
	mpz_class digits = 2048*log10(2) + 1;

	if(countSum < digits) {
		cerr << "Key is smaller than 2048 bits!" <<endl;
	}

}

//=================================

vector<pair<mpz_class, mpz_class> > createKeyPair(mpz_class e, mpz_class d, mpz_class N, vector<pair<mpz_class, mpz_class> > keys) {
	keys.push_back(make_pair(e, N));
	keys.push_back(make_pair(d, N));

	return keys;
}

//=================================

vector<string> insertStr(vector<string> disStr) {
	disStr.push_back("Public key:");
	disStr.push_back("Private key:");

	return disStr;
}

//=================================

void printKeys(vector<pair<mpz_class, mpz_class> > keys, vector<string> disStr) {
	vector<pair<mpz_class, mpz_class> >::const_iterator iter;
	vector<string>::const_iterator k;

	for(iter = keys.begin(), k = disStr.begin(); iter !=keys.end(), k !=disStr.end(); ++iter, ++k) {
		cout << *k << " ";
		cout << "(" << iter->first << ", " << iter->second << ")" <<endl;
	}

}

//=================================


int main(const int argc, const char *const argv[]) {

	if(argc !=1) {
		cerr << "Zu wenig/viele Argumente!" <<endl;
		cerr << "Programmname erforderlich" << argv[0] <<endl;

	} else {

	mpz_class p, q, gcd, m, c;
	mpz_class e;
	gmp_randstate_t randst;
	gmp_randinit_default(randst);
	gmp_randseed_ui(randst, (int)time(0));
	mpz_urandomb(p.get_mpz_t(), randst, 1024);
	mpz_urandomb(q.get_mpz_t(), randst, 1024);
	mpz_nextprime(p.get_mpz_t(), p.get_mpz_t());
	mpz_nextprime(q.get_mpz_t(), q.get_mpz_t());
	mpz_urandomb(e.get_mpz_t(), randst, 1024);
	mpz_class N = p*q;
	mpz_class phi = (p-1)*(q-1);
	mpz_class d = (phi+1);
	vector<pair<mpz_class, mpz_class> > keys;
	vector<string> disStr;


	while(e > 1 && e < phi) {
		mpz_gcd(gcd.get_mpz_t(), e.get_mpz_t(), phi.get_mpz_t());

		if(gcd == 1) {
			mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi.get_mpz_t());
			break;

		} else {
			e++;
		}
	}

		keys = createKeyPair(e, d, N, keys);
		disStr = insertStr(disStr);
		printKeys(keys, disStr);
		calculateBitsInDigits(e, d, N);

	}

	return 0;
}
