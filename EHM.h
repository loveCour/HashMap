#pragma once
/*
extensible hash table�� �������� Ű�� �ؽ̰���� ���� ����Ǵ� Bucket�� �����˴ϴ�.
�ؽ� ���̺��� ���丮 ������ ������ ���� ��Ʈ�� synonym(�ؽ̰���� ������ Ű)�Ǵܿ� ����մϴ�.
�ٽ� ���� �ؽ̰���� �ؽ� ���̺��� ���� ��ŭ�� ��Ʈ�� �����(masking) �� ���� ������ synonym�̶�� �Ǵ��մϴ�.
�ؽ� ���̺��� ���������͵��� �����̸�, �� ������ �ε����� �ؽ̰���� masking�� ����Դϴ�.

���� ������ ���� ��, Bucket�� �ִ� ũ�⸦ �ʰ��Ͽ� overflow�� �߻��Ͽ��ٸ�,
���丮 ������ �ø���, �����÷ΰ� �߻��� ��Ŷ�� �����͵��� �ٽ� �ؽ��Ͽ� synonym�� ���� �����͸� split�մϴ�.
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
		int curIndex = mask & hashing(k);//�ؽ̰���� ����ŷ�� ����� ������ �ε����Դϴ�.
		EHMBucket* curBucket = HashTable[curIndex];
		bool isOverFlow = !(curBucket->insert(k, v));//���� �� �����÷ΰ� �߻��ߴٸ� false�� ��ȯ�մϴ�.
		while (isOverFlow)
		{
			if (dirLevel <= curBucket->localLevel)
			{//���� ���丮 ������ �����÷ΰ� �߻��� ��Ŷ�� ���÷����� ���ų� �۴ٸ�,
				int tableEleNum = pow(2,dirLevel);
				dirLevel++;//������ ������Ű��,
				for (int i = 0; i < tableEleNum; i++)
				{//���� ���̺�����Ʈ���� �״�� �����ؼ�
					HashTable.push_back(HashTable[i]);//���̺� �����մϴ�.
				}//���̺��� ũ��� 2�谡 �Ǿ����ϴ�.
			}
			curIndex = split(curIndex);
			if (curIndex == -1) {
				isOverFlow = false;
			}
		}//���ø������ �����÷ΰ� �ƴҶ����� �ݺ��մϴ�.
		//� ��Ŷ�� ���丮 ������ �������ѵ�, synonym�� ������ �����Ͽ� ���ø����� ���� ��찡 �ֱ� �����Դϴ�.
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
		curBucket->localLevel++;//���ø� �� ���÷����� �����մϴ�.
		EHMBucket* newBucket = new EHMBucket(localLevel+1, BUCKETSIZE);
		curBucket->numOfSyn = 0;
		for (int i = 0; i < BUCKETSIZE + 1; i++) {//�����÷ν� bucketsize���� 1��ŭ ���� �����Ͱ� �� ���Ͽ� ����ֽ��ϴ�.
			bool isZero = 0 == (hashing((curBucket->Bucket[i]).first) >> localLevel & 1);
			//synonym�Ǵܿ� ���ο� ��Ʈ�� ����մϴ�. �� ������ �����͵��� �ؽ̰�� ���� ��Ʈ������ ��� �����մϴ�. 
			if (isZero) {//���ο� ��Ʈ�� 0�϶�, ���� ���Ͽ� ����ϴ�.
				curBucket->Bucket[curBucket->numOfSyn] = curBucket->Bucket[i];
				curBucket->numOfSyn++;
			}
			else {//���ο� ��Ʈ�� 1�̶�� ���ο� ���Ͽ� ����ϴ�.
				newBucket->Bucket[newBucket->numOfSyn] = curBucket->Bucket[i];
				newBucket->numOfSyn++;
			}
		}
		//�ؽ����̺� ���ο� ������ �����Ѿ��ϴ� �ε��� ���θ� ������Ʈ �մϴ�.
		//�׷��� ���� �ʴ´ٸ� �ؽ����̺� �����͸� ���� �� ���丮 ������ ���� ������ 2�̻� ���̳��� ��쿡 ������ �߻��մϴ�.
		//���� ��� ���� ���̺�ũ�Ⱑ 2�̰�, �ε��� 0�� ���Ͽ� �����͸� �����Ϸ��� �� ��,
		//���̺��� ���ڶ� ���̺� ��ü�� ���� �ٿ��ֱ��ؼ� 2��ũ��� ��������ϴ�.
		//���� ���̺�ũ�Ⱑ 4�� �Ǿ��µ�, ���ø��� ���� �ʾҰ�, �ٽ� ���ڶ� ���̺�ũ�⸦ 8�� ��������ϴ�. 
		//�̶�, �� ���̺��� �ε��� 1 3 5 7�� ������ ����(B)�� ����Ű�� �ְ� ���÷����� �״�� 1�Դϴ�.
		//�ٽ� � �����͸� �����Ϸ��� �մϴ�. �� �ؽ̰���� �ε��� 1�̾ 1 3 5 7�� ����Ű�� �ִ� ���Ͽ�
		//�����͸� �����Ͽ����� �����÷ΰ� �߻��Ͽ� �����͸� ���ø��Ͽ����մϴ�.
		//���ø� �� ��, ���ο� ����(C)�� �����Ǿ����ϴ�. �� ��, B�� �ε����� 1 3 5 7 �� ����Ű�� �ֽ��ϴ�.
		//3�� 7�� C�� ����Ű���� ������ �մϴ�.
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

