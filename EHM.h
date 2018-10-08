#pragma once

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <math.h>
using namespace std;

class EHMBucket {
private:
	int localLevel;
	vector<pair<int, string>> b;
	int numOfSyn;
	EHMBucket(int LLevel, const int bucketSize) : localLevel(LLevel), b(vector<pair<int,string>>(bucketSize+1)) {}
	bool insert(int k, string v) {
		b[numOfSyn] = pair<int, string>(k, v);
		numOfSyn++;
		if (numOfSyn >= b.size()) {
			return false;
		}
		return true;
	}
	void print() {
		cout << this << " ";
		for (int i = 0; i < numOfSyn; i++) {
			cout << b[i].second << " ";
		}
		cout << endl;
	}
	friend class EHM;
};

class EHM {
private:
	static const int BUCKETSIZE = 4;
	int dirLevel;
	vector < EHMBucket* > HashTable;
public:
	EHM() : dirLevel(1){
		EHMBucket* firstB = new EHMBucket(dirLevel, BUCKETSIZE);
		EHMBucket* secondB = new EHMBucket(dirLevel, BUCKETSIZE);
		HashTable.push_back(firstB);
		HashTable.push_back(secondB);
	}
	~EHM() {
		int tableEleNum = pow(2,dirLevel);
		for (int i = 0; i < tableEleNum; i++) {
			if (HashTable[i] != nullptr) {
				delete HashTable[i];
				HashTable[i] = nullptr;
			}
		}
	}
	bool insert(int k, string v) {
		int mask = pow(2,dirLevel) - 1;
		int curIndex = mask & hashing(k);
		EHMBucket* curB = HashTable[curIndex];
		bool isOverFlow = !(curB->insert(k, v));
		while (isOverFlow) {//그런데 로컬이 2개 이상 차이나면 이 알고리즘은 문제가 있다.
			//자신을 가리키는 모든 포인터들에 대해 자신을 가리키면 안되는 녀석들을 찾아 포인팅을 바꿔줘야한다. 
			if (dirLevel <= curB->localLevel) {
				int tableEleNum = pow(2,dirLevel);
				dirLevel++;
				for (int i = 0; i < tableEleNum; i++) {
					HashTable.push_back(HashTable[i]);
				}
			}
			curIndex = split(curIndex);
			if (curIndex == -1) {
				isOverFlow = false;
			}		
		}
		return true;
	}
	string search(int k) {
		int mask = pow(2, dirLevel) - 1;
		int curIndex = mask & hashing(k);
		EHMBucket* curB = HashTable[curIndex];
		for (int i = 0; i < curB->numOfSyn; i++) {
			if (k == (curB->b[i]).first) {
				return (curB->b[i]).second;
			}
		}
		return "NOIMNOT";
	}
	void print() {
		for (int i = 0; i < HashTable.size(); i++) {
			cout << i << " ";
			HashTable[i]->print();
		}
	}
private:
	int hashing(int k) {
		return k;
	}
	int split(int dstIndex) {
		EHMBucket* curB = HashTable[dstIndex];
		int localLevel = curB->localLevel;
		curB->localLevel++;
		EHMBucket* newB = new EHMBucket(localLevel+1, BUCKETSIZE);
		curB->numOfSyn = 0;
		for (int i = 0; i < BUCKETSIZE + 1; i++) {
			bool isZero = 0 == (hashing((curB->b[i]).first) >> localLevel & 1);
			if (isZero) {
				curB->b[curB->numOfSyn] = curB->b[i];
				curB->numOfSyn++;
			}
			else {
				newB->b[newB->numOfSyn] = curB->b[i];
				newB->numOfSyn++;
			}
		}
		//포인터 재분배
		int newIndex = dstIndex + pow(2, localLevel);		
		for (int i = newIndex; i < HashTable.size(); i += pow(2, localLevel+1)) {
			HashTable[i] = newB;
		}
		int result = -1;
		if (curB->numOfSyn > BUCKETSIZE){
			result = dstIndex;
		}
		else if (newB->numOfSyn > BUCKETSIZE) {
			result = newIndex;
		}
		return result;
	}
};

