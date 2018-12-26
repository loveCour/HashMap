#pragma once
/*
extensible hash table는 데이터의 키의 해싱결과에 따라 저장되는 Bucket이 결정됩니다.
해시 테이블의 디렉토리 레벨과 동일한 수의 비트를 synonym(해싱결과가 동일한 키)판단에 사용합니다.
다시 말해 해싱결과를 해시 테이블의 레벨 만큼의 비트를 남기고(masking) 그 것이 같으면 synonym이라고 판단합니다.
해시 테이블은 버켓포인터들의 벡터이며, 각 버켓의 인덱스는 해싱결과를 masking한 결과입니다.

만약 데이터 삽입 후, Bucket이 최대 크기를 초과하여 overflow가 발생하였다면,
디렉토리 레벨을 늘리고, 오버플로가 발생한 버킷의 데이터들을 다시 해싱하여 synonym에 따라 데이터를 split합니다.
*/
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <math.h>
using namespace std;

class EHMBucket {
private:
	int localLevel;
	vector<pair<int, string>> Bucket;
	int numOfSyn;
	EHMBucket(int LLevel, const int bucketSize) : localLevel(LLevel), Bucket(vector<pair<int,string>>(bucketSize+1)) {}
	bool insert(int k, string v) {
		Bucket[numOfSyn] = pair<int, string>(k, v);
		numOfSyn++;
		if (numOfSyn >= Bucket.size()) {
			return false;
		}
		return true;
	}
	void print() {
		cout << this << " ";
		for (int i = 0; i < numOfSyn; i++) {
			cout << Bucket[i].second << " ";
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
		EHMBucket* firstBucket = new EHMBucket(dirLevel, BUCKETSIZE);
		EHMBucket* secondBucket = new EHMBucket(dirLevel, BUCKETSIZE);
		HashTable.push_back(firstBucket);
		HashTable.push_back(secondBucket);
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
		int curIndex = mask & hashing(k);//해싱결과를 마스킹한 결과가 버켓의 인덱스입니다.
		EHMBucket* curBucket = HashTable[curIndex];
		bool isOverFlow = !(curBucket->insert(k, v));//삽입 후 오버플로가 발생했다면 false를 반환합니다.
		while (isOverFlow)
		{
			if (dirLevel <= curBucket->localLevel)
			{//현재 디렉토리 레벨과 오버플로가 발생한 버킷의 로컬레벨이 같거나 작다면,
				int tableEleNum = pow(2,dirLevel);
				dirLevel++;//레벨을 증가시키고,
				for (int i = 0; i < tableEleNum; i++)
				{//현재 테이블엘리먼트들을 그대로 복사해서
					HashTable.push_back(HashTable[i]);//테이블에 삽입합니다.
				}//테이블의 크기는 2배가 되었습니다.
			}
			curIndex = split(curIndex);
			if (curIndex == -1) {
				isOverFlow = false;
			}
		}//스플릿결과가 오버플로가 아닐때까지 반복합니다.
		//어떤 버킷에 디렉토리 레벨을 증가시켜도, synonym이 이전과 동일하여 스플릿되지 않은 경우가 있기 때문입니다.
		return true;
	}
	string search(int k) {
		int mask = pow(2, dirLevel) - 1;
		int curIndex = mask & hashing(k);
		EHMBucket* curBucket = HashTable[curIndex];
		for (int i = 0; i < curBucket->numOfSyn; i++) {
			if (k == (curBucket->Bucket[i]).first) {
				return (curBucket->Bucket[i]).second;
			}
		}
		return "NOTFOUND";
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
		EHMBucket* curBucket = HashTable[dstIndex];
		int localLevel = curBucket->localLevel;
		curBucket->localLevel++;//스플릿 시 로컬레벨이 증가합니다.
		EHMBucket* newBucket = new EHMBucket(localLevel+1, BUCKETSIZE);
		curBucket->numOfSyn = 0;
		for (int i = 0; i < BUCKETSIZE + 1; i++) {//오버플로시 bucketsize보다 1만큼 많은 데이터가 한 버켓에 들어있습니다.
			bool isZero = 0 == (hashing((curBucket->Bucket[i]).first) >> localLevel & 1);
			//synonym판단에 새로운 비트를 사용합니다. 이 버켓의 데이터들을 해싱결과 이전 비트까지는 모두 동일합니다. 
			if (isZero) {//새로운 비트가 0일때, 현재 버켓에 남깁니다.
				curBucket->Bucket[curBucket->numOfSyn] = curBucket->Bucket[i];
				curBucket->numOfSyn++;
			}
			else {//새로운 비트가 1이라면 새로운 버켓에 담습니다.
				newBucket->Bucket[newBucket->numOfSyn] = curBucket->Bucket[i];
				newBucket->numOfSyn++;
			}
		}
		//해시테이블에 새로운 버켓을 가리켜야하는 인덱스 전부를 업데이트 합니다.
		//그렇게 하지 않는다면 해시테이블에 데이터를 삽입 시 디렉토리 레벨과 로컬 레벨이 2이상 차이나는 경우에 문제가 발생합니다.
		//예를 들어 현재 테이블크기가 2이고, 인덱스 0번 버켓에 데이터를 삽입하려고 할 때,
		//테이블이 모자라 테이블 전체를 복사 붙여넣기해서 2배크기로 만들었습니다.
		//이제 테이블크기가 4가 되었는데, 스플릿이 되지 않았고, 다시 모자라서 테이블크기를 8로 만들었습니다. 
		//이때, 이 테이블의 인덱스 1 3 5 7은 동일한 버켓(B)을 가리키고 있고 로컬레벨은 그대로 1입니다.
		//다시 어떤 데이터를 삽입하려고 합니다. 그 해싱결과가 인덱스 1이어서 1 3 5 7이 가리키고 있는 버켓에
		//데이터를 삽입하였더니 오버플로가 발생하여 데이터를 스플릿하여야합니다.
		//스플릿 한 후, 새로운 버켓(C)가 생성되었습니다. 이 때, B는 인덱스가 1 3 5 7 이 가리키고 있습니다.
		//3과 7은 C를 가리키도록 만들어야 합니다.
		int newIndex = dstIndex + pow(2, localLevel);		
		for (int i = newIndex; i < HashTable.size(); i += pow(2, localLevel+1)) {
			HashTable[i] = newBucket;
		}
		int result = -1;
		if (curBucket->numOfSyn > BUCKETSIZE){
			result = dstIndex;
		}
		else if (newBucket->numOfSyn > BUCKETSIZE) {
			result = newIndex;
		}
		return result;
	}
};

