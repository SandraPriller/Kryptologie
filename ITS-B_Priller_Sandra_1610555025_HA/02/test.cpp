#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
using namespace std;


/*************************************************/				//Buchstabenhäufigkeit in Deutsch (gerundet)
/*			Author:   Sandra Priller			 */
/*			Date:	  21.02.2019				 */				// E	17%		D	5%		O	3%		V	1%
/*			Program:  Text-Encryption	 		 */				// N	10%		H	5%		B	2%		J	0%
/*					  Vigenere-Quadrat			 */				// I	 8%		U	4%		W	2%		Y	0%
/*												 */				// S	 7%		L	3%		F	2%		X	0%
/*************************************************/				// R	 7%		C	3%		K	1%		Q	0%
																// A	 7%		G	3%		Z	1%
																// T	 6%		M	3%		P	1%
/*Position im Alphabet: 6 21 11  23 9  6   14 20 6
Buchstaben: 			G V  L	 X	J  G   O  U  G
Schlüssel:				K R  Y	 K  R  Y   K  R  Y
Position:			   10 17 24  10 17 24  10 17 24
Klartext:				W E  N	 N	S  I   E  D  I

Bsp.: 10-1 = 9; 25-9 = 16; 16+6 = 22 -> W;
	  21-17 = 4 -> E
	  24-1 = 23; 25-23 = 2; 2+11 = 13 -> N		*/

//================================================

vector<char> OpenFile(ifstream &datei) {		//File öffnen und zeichenweises einlesen
	vector<char> cipherComplete;
	char c = 0;

	while(datei.get(c)) {						//Der C++ Programmierer, Ulrich Breymann, S. 420 (Zeile 46 und 47)
		cipherComplete.push_back(c);
	}

	//cout <<endl;
	return cipherComplete;
}

//=============================

void PrintVector(vector<char> cipherComplete) {		//Ausgabe des Textes mit Sonderzeichen & Spaces
	vector<char>::iterator i;

	for(i = cipherComplete.begin(); i !=cipherComplete.end(); i++) {
		cout << *i;
	}
}

//==============================

string KeyMapping(string strCipher, string key) {		//Key sooft in string einfügen, bis string mappedKey die gleiche Größe
	string mappedKey;									//wie string strCipher hat

	for(size_t i = 0; i < key.size(); i++) {			//Wenn Schlüssel in Kleinbuchstaben => dann in Großbuchstaben transformieren
		if(islower(key[i])) {
			transform(key.begin(), key.end(), key.begin(), ::toupper);
		}
	}

	for(size_t i = 0, j = 0; i < strCipher.size(); i++) {
		if(!isalpha(strCipher[i])) {							//alle Sonderzeichen ignorieren und unverändert übernehmen
			mappedKey += strCipher[i];

		} else {
			if(j < key.length()) {
				mappedKey += key[j];
				j++;

			} else {
				j = 0;
				mappedKey += key[j];
				j++;
			}
		}
	}

	//cout <<mappedKey<<endl;
	return mappedKey;
}

vector<vector<int> > VigenereQuadrat() {				//Erstellen der Vigenere-Tabelle und einlesen in 2D-Vektor
	int rows = 26;
	int columns = 26;
	vector<vector<int> > vigQuad(rows, vector<int>(columns));
	vector<int> temp;

	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			int c;
			if((i + 'A') + j > 'Z') {
				c = ((i + 'A') + j) - 26;
				vigQuad[i][j] = c;

			} else {
				c = (i + 'A') + j;
				vigQuad[i][j] = c;
			}
		}
	}

	return vigQuad;
}

//================================

void PrintVigTable(vector<vector<int> > vigQuad) {		//Vigenere-Tabelle ausgeben
	int row_size = vigQuad.size();
	int col_size = vigQuad[0].size();

	for(int i = 0; i < row_size; i++) {
        for(int j = 0; j < col_size; j++)
            cout << (char)vigQuad[i][j] << " ";
			cout << endl;
    }
}

//================================

string Decryption(string strCipher, string mappedKey) {							//Entschlüsselung des Ciphertextes
	string decryptedText;

	for(size_t i = 0, j = 0; i < strCipher.size(); i++, j++) {
		if(isalpha(strCipher[i]))
			decryptedText += (strCipher[i] - mappedKey[j] + 26) % 26 + 'A';		//Bsp.: (71 - 75 + 26) % 26 + 65 = 87
		if(!isalpha(strCipher[i]))												//		  G   K 			   A =	W
			decryptedText += strCipher[i];
	}

	return decryptedText;
}

//================================

string Encryption(vector<char> plainText, string mappedKey, vector<vector<int> > vigQuad) {		//Verschlüsselung des Klartextes anhand der Vigenere-Tabelle;
	string strPlain(plainText.begin(),plainText.end());
	string encryptedText;
	string ae = "AE", oe = "OE", ue = "UE";
	string a = "Ä", o = "Ö", u = "Ü";
	size_t start_pos = 0;


	for(size_t i = 0; i < strPlain.size(); i++) {
		while((start_pos = strPlain.find(a, start_pos)) != string::npos) {		//adaptiert aus https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
			strPlain.replace(start_pos, a.length(), ae);						//alle while-Schleifen in Funktion
			start_pos += strPlain.length();
		}

		start_pos = 0;

		while((start_pos = strPlain.find(o, start_pos)) != string::npos) {
			strPlain.replace(start_pos, o.length(), oe);
			start_pos += strPlain.length();
		}

		start_pos = 0;

		while((start_pos = strPlain.find(u, start_pos)) != string::npos) {
			strPlain.replace(start_pos, u.length(), ue);
			start_pos += strPlain.length();
		}

		start_pos = 0;
	}

	for(size_t i = 0; i < strPlain.size(); i++) {
		if(!isalpha(strPlain[i]) && !isalpha(mappedKey[i])) {
			encryptedText += strPlain[i];

		} else {
			int x = (int)strPlain[i] - 65;					//adaptiert aus https://www.youtube.com/watch?v=RI8IXHupKTo, Zeile 174 - 176
			int y = (int)mappedKey[i] - 65;
			encryptedText += (char)vigQuad[x][y];
		}
	}

	//cout <<endl << encryptedText <<endl;
	return encryptedText;
}

//================================

string Umlaute(string decryptedText) {			//ae, oe, ue durch Umlaute ersetzen
	string ae = "AE", oe = "OE", ue = "UE";
	string a = "Ä", o = "Ö", u = "Ü";
	size_t start_pos = 0;


	for(size_t i = 0; i < decryptedText.size(); i++) {
		while((start_pos = decryptedText.find(ae, start_pos)) != string::npos) {	//adaptiert aus https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
			decryptedText.replace(start_pos, ae.length(), a);						//alle while-Schleifen in Funktion
			start_pos -= decryptedText.length();
		}

		start_pos = 0;

		while((start_pos = decryptedText.find(oe, start_pos)) != string::npos) {
			decryptedText.replace(start_pos, oe.length(), o);
			start_pos -= decryptedText.length();
		}

		start_pos = 0;

		while((start_pos = decryptedText.find(ue, start_pos)) != string::npos) {
			decryptedText.replace(start_pos, ue.length(), u);
			start_pos -= decryptedText.length();
		}

		start_pos = 0;
	}

	//cout << decryptedText <<endl;
	return decryptedText;
}

//==================================

void SaveTextInFile(ofstream &ausgabe, string decryptedText) {			//Entschlüsselten Text in Datei schreiben
	ausgabe<<decryptedText;
	ausgabe.close();
}

//================================

vector<char> PrintDecryption(ofstream &ausgabe, string decryptedText, ifstream &plain) {	//Auslesen der Datei des entschlüsselten Textes am Bildschirm
	SaveTextInFile(ausgabe, decryptedText);
	vector<char> plainText;
	vector<char>::iterator i;
	char c = 0;

	while(plain.get(c)) {								//Der C++ Programmierer, Ulrich Breymann, S. 420 (Zeile 46 und 47)
		plainText.push_back(c);
	}

	for(i = plainText.begin(); i !=plainText.end(); i++) {
		cout << *i;
	}

	return plainText;
}

//==================================================

int main(const int argc, const char *const argv[]) {
	vector<char> cipherComplete(0), plainText;
	vector<vector<int> > vigQuad;
	string key, mappedKey, decryptedText, encryptedText;


	if(argc < 4 || argc > 4) {
		cerr << "Sie haben zu wenig/viele Argumente übergeben!" <<endl;
		cerr << "Argument 1: <Programmname>" <<endl;
		cerr << "Argument 2: <Eingabedateipfad>" <<endl;
		cerr << "Argument 3: <Ausgabedateipfad>" <<endl;
		cerr << "Argument 4: <Schlüssel>" <<endl;

		return 1;

	} else if(!isalpha((const char)*argv[3])) {
		cerr << "Sie haben einen falschen Wert eingegeben!" <<endl;
		return 1;

	} else {
		ifstream datei(argv[1]);
		ofstream ausgabe(argv[2], ofstream::trunc);			//trunc => Dateiinhalt vorher immer löschen, bevor geschrieben wird
		ifstream plain(argv[2]);
		key = argv[3];										//Schlüssel ist KRY

		if(datei.bad()) {
			cerr << "Datei konnte nicht geöffnet werden!" <<endl;
			return 1;

		} else {
			cipherComplete = OpenFile(datei);
		}

		//printVector(cipherComplete);
		string strCipher(cipherComplete.begin(),cipherComplete.end());
		mappedKey = KeyMapping(strCipher, key);
		vigQuad = VigenereQuadrat();
		//printVigTable(vigQuad);
		decryptedText = Decryption(strCipher, mappedKey);
		//decryptedText = umlaute(decryptedText);
		plainText = PrintDecryption(ausgabe, decryptedText, plain);
		encryptedText = Encryption(plainText, mappedKey, vigQuad);

		datei.close();
		plain.close();
	}

  return 0;
}

