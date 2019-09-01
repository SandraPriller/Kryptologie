#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <math.h>
using namespace std;


/****************************************************/
/*		Author:   Sandra Priller	    */
/*		Date:	  17.02.2019		    */
/*		Program:  Letter frequency & 	    */
/*			  Kasiski-Test		    */												 */
/****************************************************/



void PrintVector(vector<char> cipherComplete) {		//Ausgabe des Textes mit Sonderzeichen & Spaces
	vector<char>::iterator i;

	for(i = cipherComplete.begin(); i !=cipherComplete.end(); i++) {
		cout << *i;
	}
}

//======================================

size_t TotalElements(vector<char> cipherComplete) {
	size_t total = cipherComplete.size();
		cout << "Anzahl Zeichen Cipher-Text: " <<total<<endl;

	return total;
}

//======================================

vector<char> OpenFile(ifstream &datei) {		//File öffnen und zeichenweises einlesen
	vector<char> cipherComplete;
	char c = 0;

	while(datei.get(c)) {				//Der C++ Programmierer, Ulrich Breymann, S. 420 (Zeile 46 und 47)
		cipherComplete.push_back(c);
	}

	//cout <<endl;
	return cipherComplete;
}

//======================================

vector<int> PrimeFactors(int n) {		//Funktion adaptiert aus https://www.geeksforgeeks.org/print-all-prime-factors-of-a-given-number/
	vector<int> primes;

	while(n%2 == 0) {
		primes.push_back(2);
		n = n/2;
	}

	for(int i = 3; i <= sqrt(n); i = i+2) {
		while(n%i == 0) {
			primes.push_back(i);
			n = n/i;
		}
	}

	if(n > 2)
		primes.push_back(n);

    return primes;
}

//===========================

int Gcd(int a, int b) {				//Funktion adaptiert aus https://www.geeksforgeeks.org/gcd-two-array-numbers/
    if(a == 0)
        return b;
    return Gcd(b % a, a);
}

//===========================

int FindGCD(vector<int> primes, int n) { 	//Funktion adaptiert aus https://www.geeksforgeeks.org/gcd-two-array-numbers/
	int result = 0;

    for(int i = 1; i < n; i++) {
        result = Gcd(primes[i], result);
    }

    return result;
}

//===========================

int KasiskiTest(vector<pair<string, int> > index) {		//Berechnen der Abstände von gleichen Buchstabenfolgen
	vector<int> position, distances, primes;		//die Abstände faktorisieren, jener, der bei allen das
	vector<int>::iterator i, k;				//gleiche Ergebnis liefert -> ergibt Key-Länge -> Bsp.: 3*2*5 und 3*7
	vector<pair<string, int> >::const_iterator iter;	//-> Schlüssellänge 3 wahrscheinlich
	int keyLength = 0;

	for(iter = index.begin(); iter != index.end(); ++iter) {
		position.push_back(iter->second);
    }

    for(size_t k = position.size(); k > 9; k--) {
		distances.push_back(position[9]-position[8]);
		distances.push_back(position[8]-position[7]);
		distances.push_back(position[7]-position[6]);
		distances.push_back(position[5]-position[4]);
		distances.push_back(position[3]-position[2]);
		distances.push_back(position[2]-position[1]);
		distances.push_back(position[1]-position[0]);
    }

    for(i = distances.begin(); i !=distances.end(); i++) {
		primes = PrimeFactors(*i);
		//cout <<endl;

		for(k = primes.begin(); k !=primes.end(); k++) {
			//cout << *k << " ";
		}
    }

    keyLength = FindGCD(distances, distances.size());

	//cout << endl;
	return keyLength;
}

//=============================

vector<pair<string, int> >FindWords(string onlyLetters) {			//Suchen der Buchstabenfolgen, die im Text mehrmals
	string gvl = "GVL", vlx = "VLX", ugo = "UGO";				//vorkommen (min. 3 Buchstaben)
	string::size_type pos_gvl = 0, pos_vlx = 0, pos_ugo = 0;
	vector<pair<string, int> > index;


	while(string::npos != (pos_gvl = onlyLetters.find(gvl, pos_gvl))) {	//https://stackoverflow.com/questions/5815838/how-do-i-find-all-the-positions-of-a-substring-in-a-string
		index.push_back(make_pair(gvl, pos_gvl));
		++pos_gvl;
	}

	while(string::npos != (pos_vlx = onlyLetters.find(vlx, pos_vlx))) {
		index.push_back(make_pair(vlx, pos_vlx));
		++pos_vlx;
	}

	while(string::npos != (pos_ugo = onlyLetters.find(ugo, pos_ugo))) {
		index.push_back(make_pair(ugo, pos_ugo));
		++pos_ugo;
	}

	for(vector<pair<string, int> >::const_iterator iter = index.begin();
        iter != index.end(); ++iter) {
		//cout << iter->first << ": " << iter->second <<endl;
    }

	return index;
}

//=============================

string OnlyLetters(string strCipher) {		//Alle Sonderzeichen und Spaces extrahieren
	string onlyLetter;
	string::const_iterator si;
	int count = 0;
	char c = 0;

	for(si = strCipher.begin(); si !=strCipher.end(); ++si) {
		if(isalpha(c = *si)) {
			onlyLetter += c;
			count++;
		}
	}

	//cout << "Anzahl Zeichen ohne Spaces und Sonderzeichen: " << count << endl;

	//findWords(onlyLetter);		//Aufruf der Funktion, die nach gleichen Buchstabenfolgen sucht
	return onlyLetter;
}

//============================

string GroupTextByThree(string onlyLetter, int keyLength) {				//nach 3 Buchstaben Leerzeichen einfügen,
	for(size_t i = keyLength; i < onlyLetter.size(); i = i+(keyLength+1)) {		//dient der besseren Lesbarkeit
		  onlyLetter.insert(i, " ");
	}

	return onlyLetter;
}

//============================

vector<char> ShowFirstRow(string onlyLetter, int keyLength) {		//alle 1., 4., 7. etc. Buchstaben in
	vector<char> firstRow;						//Vektor speichern
	vector<char>::iterator i;

	for(size_t i = 0; i < onlyLetter.size(); i = i+keyLength) {
		firstRow.push_back(onlyLetter[i]);
	}

		for(i = firstRow.begin(); i !=firstRow.end(); i++) {
			//cout << *i;
		}

	//cout <<endl;
	return firstRow;
}

//=============================

vector<char> ShowSecondRow(string onlyLetter, int keyLength) {		//alle 2., 5., 8. etc. Buchstaben in
	vector<char> secondRow;						//in Vektor speichern
	vector<char>::iterator i;

	for(size_t i = 1; i < onlyLetter.size(); i = i+keyLength) {
		secondRow.push_back(onlyLetter[i]);
	}

		for(i = secondRow.begin(); i !=secondRow.end(); i++) {
			//cout << *i ;
		}

	//cout <<endl;
	return secondRow;
}

//=============================

vector<char> ShowThirdRow(string onlyLetter, int keyLength) {		//alle 3., 6., 9. etc. Buchstaben in
	vector<char> thirdRow;						//in Vektor speichern
	vector<char>::iterator i;

	for(size_t i = 2; i < onlyLetter.size(); i = i+keyLength) {
		thirdRow.push_back(onlyLetter[i]);
	}

		for(i = thirdRow.begin(); i !=thirdRow.end(); i++) {
			//cout << *i ;
		}

	//cout << endl;
	return thirdRow;
}

//==============================

bool SortData(pair<char, float> p1, pair<char, float> p2) {		//Sortieren der Vektoren in absteigender Reihenfolge
	if(p1.second == 0 && p2.second == 0) 				//Funktion adaptiert aus http://www.cplusplus.com/forum/beginner/115630/
		return(p1.first);
	else
        return (p1.second > p2.second);
}

//==============================

vector<pair<char, float> > CalculateFrequency(vector<char> letters) {	//Berechnen der rel. Häufigkeiten der
	vector<pair<char, float> > relH(0);				//drei Vektoren
	vector<pair<char, float> >::iterator iter;
	int sum = 0;
	int factor = 100;
	float alpha[26] = {0};

	for(size_t k = 0; k < letters.size(); k++) {
		char c = letters[k];
		if(c >= 'A' && c <= 'Z') {
            alpha[c-'A']++;
        }
	}

	for(int j = 0; j < 26; j++)
		sum+=alpha[j];

    for(int i = 0; i < 26; i++) {
		char c = 'A' + i;
		relH.push_back(pair<char, float>(make_pair(c, (alpha[i]/sum)*factor)));
		sort(relH.begin(), relH.end(), SortData);
    }

	return relH;
}

//=============================

int main(const int argc, const char *const argv[]) {
	vector<char> cipherComplete(0);
	vector<char> firstRow, secondRow, thirdRow;
	vector<pair<string, int> > index;
	string onlyLetter, group;
	vector<pair<char, float> > relHFirst(0), relHSecond(0), relHThird(0);;
	vector<pair<char, float> >::iterator iFirst, iSecond, iThird;
	int keyLength = 0;


  if(argc < 2 || argc > 2) {
	cerr << "Sie haben zu wenig/viele Argumente übergeben!" <<endl;
	cerr << "Argument 1: <Programmname>" <<endl;
	cerr << "Argument 2: <Dateiname>" <<endl;
	return 1;

  }

	ifstream datei(argv[1]);

	if(datei.bad()) {
		cerr << "Datei konnte nicht geöffnet werden!" <<endl;
		return 1;

	} else {
		cipherComplete = OpenFile(datei);
	}

		//printVector(cipherComplete);
		//totalElements(cipherComplete);
		string strCipher(cipherComplete.begin(),cipherComplete.end());
		onlyLetter = OnlyLetters(strCipher);
		index = FindWords(onlyLetter);
		keyLength = KasiskiTest(index);
		group = GroupTextByThree(onlyLetter, keyLength);
		firstRow = ShowFirstRow(onlyLetter, keyLength);
		secondRow = ShowSecondRow(onlyLetter, keyLength);
		thirdRow = ShowThirdRow(onlyLetter, keyLength);
		relHFirst = CalculateFrequency(firstRow);
		relHSecond = CalculateFrequency(secondRow);
		relHThird = CalculateFrequency(thirdRow);


		for(iFirst = relHFirst.begin(),
			iSecond = relHSecond.begin(), iThird = relHThird.begin();
			iFirst != relHFirst.end(), iSecond != relHSecond.end(), iThird != relHThird.end();
			++iFirst, ++iSecond, ++iThird) {
			cout << iFirst->first << ": " << fixed << setprecision(0) << iFirst->second << "% ; ";
			cout << iSecond->first << ": " << fixed << setprecision(0) << iSecond->second << "% ; ";
			cout << iThird->first << ": " << fixed << setprecision(0) << iThird->second << "%"<<endl;
		}

	datei.close();

  return 0;
}

