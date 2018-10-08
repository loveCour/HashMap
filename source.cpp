#include <iostream>
#include <assert.h>
#include "EHM.h"

using namespace std;

bool EHMTest() {
	EHM* extHashMap = new EHM();
	extHashMap->insert(0, "0000");
	extHashMap->insert(0, "0000");
	extHashMap->insert(8, "1000");
	extHashMap->insert(16, "10000");
	extHashMap->insert(24, "11000");
	extHashMap->print();
	cout << extHashMap->search(24) << endl;
	cout << extHashMap->search(12) << endl;
	return true;
}

int main() {
	EHMTest();
}