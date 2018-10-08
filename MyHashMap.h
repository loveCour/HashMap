#pragma once
#include <array>
#include <string>

using namespace std;
class MHM {
private :
	array<string, 100> myarray;
public :
	MHM() : myarray(array<string, 100>()) {}
	void insert(int k, string v) {
		myarray[k % 100] = v;
	}
	string get(int k) {
		return myarray[k % 100];
	}
};