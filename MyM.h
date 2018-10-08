#pragma once
#include <iostream>
#include <vector>
#define NOTFOUND -1

using namespace std;

class SimpleNode;
class SimpleBST;

ostream & operator << (ostream &stream, SimpleNode &node);
ostream & operator << (ostream &stream, SimpleBST &tree);

class SimpleNode {
public:
	void Print(ostream &stream) {
		if (LChild != nullptr || RChild != nullptr) {
			if (LChild != nullptr) {
				stream << "키:" << key << "의 LC인 " << *LChild;
			}
			else {
				stream << "키:" << key << "의 LC는 없다" << endl;
			}
			if (RChild != nullptr) {
				stream << "키:" << key << "의 RC인 " << *RChild;
			}
			else {
				stream << "키:" << key << "의 RC는 없다" << endl;
			}
			stream << endl;
		}
		else {
			stream << "키:" << key << "는 리프" << endl;
		}
	}
private:
	int key;
	string v;
	SimpleNode* LChild;
	SimpleNode* RChild;
	SimpleNode() {
		initNode();
	}
	SimpleNode(int nodekey, string value) {
		key = nodekey;
		v = value;
		initNode();
	}
	~SimpleNode() {
	}
	bool insert(SimpleNode* v) {
		if (key == v->key) {
			return false;
		}
		SimpleNode** insertDstNodePtr = nullptr;
		bool isSuccess;
		if (key < v->key) {
			insertDstNodePtr = &RChild;
		}
		else if (key > v->key) {
			insertDstNodePtr = &LChild;
		}
		if (*insertDstNodePtr == nullptr) {
			*insertDstNodePtr = v;
			isSuccess = true;
		}
		else {
			isSuccess = (*insertDstNodePtr)->insert(v);
		}
		return isSuccess;
	}
	void initNode() {
		LChild = nullptr;
		RChild = nullptr;
	}
	bool isLeaf() {
		return LChild == nullptr && RChild == nullptr;
	}
	/*
	bool operator == (const SimpleNode& src) { return key == src.key; }
	bool operator < (const SimpleNode& src) { return key < src.key; }
	bool operator > (const SimpleNode& src) { return key > src.key; }*/
	friend class SimpleBST;
};

class SimpleBST {
public:
	SimpleBST() {
		nodes.push_back(nullptr);
	}
	~SimpleBST() {
	}
	bool isEmpty() {
		return nodes[0] == nullptr;
	}
	bool insert(int k, string v) {
		bool isSuccess;
		SimpleNode* nodeToInsert = new SimpleNode(k, v);
		if (isEmpty()) {
			nodes[0] = nodeToInsert;
			isSuccess = true;
		}
		else {
			bool insertSuccess = nodes[0]->insert(nodeToInsert);
			if (!insertSuccess) {
				delete nodeToInsert;
				isSuccess = false;
			}
			else {
				isSuccess = true;
			}
		}
		return isSuccess;
	}
	string* Search(int searchKey) {
		string* result = nullptr;
		if (saveDecentPath(searchKey)) {
			result = &((nodes.back())->v);
		}
		nodesClear();
		return result;
	}
	bool remove(int key) {
		bool isSuccess;
		if (saveDecentPath(key)) {
			SimpleNode* dstNode = nodes.back();
			if (dstNode != nullptr) isSuccess = remove(dstNode);
		}
		else {
			isSuccess = false;
		}
		nodesClear();
		return isSuccess;
	}

	void Print(ostream &stream) {
		if (!isEmpty()) stream << (*nodes[0]);
		else stream << "빈 트리임" << endl;
	}
private:
	vector<SimpleNode*> nodes;
	bool remove(SimpleNode* dstNode) {
		if (dstNode == nullptr) {
			return false;
		}
		bool isSuccess = false;
		while (dstNode->LChild != nullptr && dstNode->RChild != nullptr) {
			SimpleNode* leastNodeAtRChild = findLeastNodeAtRChild(dstNode);
			dstNode->key = leastNodeAtRChild->key;
			dstNode = leastNodeAtRChild;
		}
		int level = nodes.size() - 1;
		isSuccess = (level > 0) ? parentWork() : rootWork();
		delete dstNode;
		return isSuccess;
	}

	SimpleNode* findLeastNodeAtRChild(SimpleNode* dstNode) {
		SimpleNode* currentNode = dstNode->RChild;
		nodes.push_back(currentNode);
		while (currentNode->LChild != nullptr) {
			currentNode = currentNode->LChild;
			nodes.push_back(currentNode);
		}
		return currentNode;
	}
	void nodesClear() {
		SimpleNode* root = nodes[0];
		nodes.clear();
		nodes.push_back(root);
	}
	bool saveDecentPath(int key) {
		int curLevel = 0;
		SimpleNode* current = nodes[0];
		bool result = false;
		while (current != nullptr) {
			if (key == current->key) {
				result = true;
				break;
			}
			else {
				SimpleNode** dstNode = nullptr;
				if (key < current->key) {
					dstNode = &(current->LChild);
				}
				else if (key > current->key) {
					dstNode = &(current->RChild);
				}
				if (*dstNode != nullptr) {
					current = *dstNode;
					curLevel++;
					nodes.push_back(current);
				}
				else {
					nodesClear();
					break;
				}
			}
		}
		return result;
	}
	bool parentWork() {
		//이 함수에 들어왔다는 건 목적노드가 리프거나 자식이 하나라는 소리다.
		int level = nodes.size() - 1;
		bool isSuccess;
		if (level > 0) {
			SimpleNode* dstNode = nodes[level];
			SimpleNode* parent = nodes[level - 1];
			SimpleNode** childToUpdatePtr = (parent->LChild == dstNode) ?
				&(parent->LChild) : &(parent->RChild);
			if (dstNode->isLeaf()) {
				*childToUpdatePtr = nullptr;
			}
			else if (dstNode->LChild == nullptr) {
				*childToUpdatePtr = dstNode->RChild;
			}
			else if (dstNode->RChild == nullptr) {
				*childToUpdatePtr = dstNode->LChild;
			}
			else {
				cout << "그럴리가 없다. 뭔가 이상하다" << endl;
				return false;

			}
			isSuccess = true;
		}
		else {
			isSuccess = false;
		}
		return isSuccess;
	};
	bool rootWork() {
		//이 함수에 들어왔다는 건 루트노드가 리프거나 자식이 하나라는 소리다.
		bool isSuccess;
		int level = nodes.size() - 1;
		if (level == 0) {
			if (nodes[0]->LChild == nullptr) {
				nodes[0] = nodes[0]->RChild;
			}
			else if (nodes[0]->RChild == nullptr) {
				nodes[0] = nodes[0]->LChild;
			}
			else if (nodes[0]->isLeaf()) {
				//아무것도 안한다.
			}
			else {
				cout << "그럴리가 없다." << endl;
				return false;
			}
			isSuccess = true;
		}
		else {
			isSuccess = false;
		}
		return isSuccess;

	}
};

ostream & operator << (ostream &stream, SimpleNode &node) {
	node.Print(stream);
	return stream;
}
ostream & operator << (ostream &stream, SimpleBST &tree) {
	tree.Print(stream);
	return stream;
}