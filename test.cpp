#include <string>
#include <iostream>
#include "MyHashMap.h"

using namespace std;

int main() {
	MHM mhm;
	mhm.insert(1, "hi1");
	mhm.insert(2, "hi2");
	mhm.insert(3, "hi3");
	mhm.insert(2, "hi23");
	cout << mhm.get(1);
	cout << mhm.get(2);
}