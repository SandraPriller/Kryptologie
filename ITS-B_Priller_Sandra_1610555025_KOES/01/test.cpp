#include <iostream>
#include <gmpxx.h>
#include <string>
using namespace std;


/****************************************************************/
/*		    Author:   Sandra Priller 			*/
/*		    Date:	  24.02.2019			*/
/*		    Program:  Encryption RSA			*/
/*					 			*/
/*								*/
/****************************************************************/


int main(const int argc, const char * const argv[]) {
  	if(argc !=4) {
		cerr << "Sie haben zuwenig Agumente uebergeben!!"<< endl;
		cout << "Programmname <m bzw. c> <d bzw. e> <N> "<<endl;
		return 1;
	}

		mpz_class c(argv[1]), d(argv[2]), N(argv[3]), m;
		mpz_powm(m.get_mpz_t(), c.get_mpz_t(), d.get_mpz_t(), N.get_mpz_t());
		cout << m <<endl;

  return 0;
}
