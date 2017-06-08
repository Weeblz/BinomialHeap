#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>

class student;
class group {
public:
	std::string name;
	std::vector<student*> groupList;

	group(std::string a) {
		name = a;
	}
};

class student {
public:
	std::string name;
	std::string homeCountry;
	std::string homeCity;
	double rating;
	group* specialization;

	student(std::string n, std::string c, std::string c_, double r, group* s) : name(n), homeCountry(c), homeCity(c_), rating(r), specialization(s) {}

	void show() const {
		std::cout << "Student Name: " << name << "\nStudent Country: " << homeCountry << "\nStudent City: " << homeCity << "\nStudent Grade: " << rating << std::endl;
	}
};

class Node {
	friend class BHeap;
	student* key;
	int degree;
	Node* fellow;
	Node* parent;
	Node* child;

public:
	Node() : key(nullptr), degree(0), fellow(nullptr), parent(nullptr), child(nullptr) {}
	Node(student* key) : key(key), degree(0), fellow(nullptr), parent(nullptr), child(nullptr) {}
	student* getKey() { return key; }
	Node* getFellow() { return fellow; }
	Node* getChild() { return child; }
};

class BHeap {
	std::vector<Node*> roots;
	Node* min;
	void linkTrees(Node*, Node*);
	std::vector<Node*> mergeRoots(BHeap*, BHeap*);

public:
	BHeap() : roots(0) {}
	BHeap(Node* x) : roots(0) { roots.push_back(x); }
	bool isEmpty() { return !roots.size(); }
	void insert(Node* x) { merge(new BHeap(x)); }
	void merge(BHeap*);
	Node* first();
	Node* extractMin();
	void decreaseKey(Node*, int);
	void Delete(Node*);
	Node* getMin() { return min; }
	Node* getFirstRoot() { return roots[0]; }
	void show();
};

std::vector<Node*> construct(Node* a) {
	std::vector<Node*> ret;
	Node* temp = a;
	while (temp) {
		ret.push_back(temp);
		temp = temp->getFellow();
	}
	return ret;
}

void treeShow(Node* root, int lvl) {
	if (!root) return;
	std::cout << "level " << lvl << "   " << root->getKey()->name << "   " << root->getKey()->rating << std::endl;
	if(lvl!=1) treeShow(root->getFellow(), lvl);
	treeShow(root->getChild(), lvl + 1);
}

void BHeap::show() {
	for (int i = 0; i < this->roots.size(); i++) {
		std::cout << i + 1 << " binomial tree:\n" << std::endl;
		treeShow(roots[i], 1);
	}
}

void BHeap::linkTrees(Node* y, Node* z) {
	// Precondition: y -> key >= z -> key
	y->parent = z;
	y->fellow = z->child;
	z->child = y;
	z->degree = z->degree + 1;
}

std::vector<Node*> BHeap::mergeRoots(BHeap *x, BHeap *y) {
	std::vector<Node*> ret;
	Node *end = new Node();

	std::vector<Node*> L = x->roots;
	std::vector<Node*> R = y->roots;
	if (x->isEmpty()) return R;
	if (y->isEmpty()) return L;
	Node* left = L[0];
	Node* right = R[0];

	while (left != nullptr || right != nullptr) {
		if (left == nullptr) {
			ret.push_back(right);
			end->fellow = right;
			end = end->fellow;
			right = right->fellow;
		}
		else if (right == nullptr) {
			ret.push_back(left);
			end->fellow = left;
			end = end->fellow;
			left = left->fellow;
		}
		else {
			if (left->degree < right->degree) {
				ret.push_back(left);
				end->fellow = left;
				end = end->fellow;
				left = left->fellow;
			}
			else {
				ret.push_back(right);
				end->fellow = right;
				end = end->fellow;
				right = right->fellow;
			}
		}
	}
	return ret;
}

void BHeap::merge(BHeap *bh) {
	BHeap *H = new BHeap();
	H->roots = mergeRoots(this, bh);

	if (H->isEmpty()) {
		this->roots = std::vector<Node*>(0);
		this->min = nullptr;
		return;
	}

	Node *prevX = nullptr;
	Node *x = H->getFirstRoot();
	Node *nextX = x->fellow;
	while (nextX != nullptr) {
		if (x->degree != nextX->degree || (nextX->fellow != nullptr && nextX->fellow->degree == x->degree)) {
			prevX = x;
			x = nextX;
		}
		else if (x->key->rating <= nextX->key->rating) {
			x->fellow = nextX->fellow;
			linkTrees(nextX, x);
		}
		else {
			if (prevX == nullptr) H->roots = construct(nextX);
			else prevX->fellow = nextX;
			linkTrees(x, nextX);
			x = nextX;
		}
		nextX = x->fellow;
	}

	this->roots = H->roots;
	this->min = H->getFirstRoot();
	Node *cur = this->getFirstRoot();
	while (cur != nullptr) {
		if (cur->key->rating < this->min->key->rating) this->min = cur;
		cur = cur->fellow;
	}
}

Node* BHeap::first() {
	return this->min;
}

Node* BHeap::extractMin() {
	if (this->isEmpty()) return nullptr;
	Node *ret = this->first();

	// delete ret from the list of roots
	Node *prevX = nullptr;
	Node *x = this->getFirstRoot();
	while (x != ret) {
		prevX = x;
		x = x->fellow;
	}
	if (prevX == nullptr) this->roots.erase(this->roots.begin());
	else prevX->fellow = x->fellow;

	// reverse the list of ret's children
	Node *revChd = nullptr;
	Node *cur = ret->child;
	while (cur != nullptr) {
		Node *next = cur->fellow;
		cur->fellow = revChd;
		revChd = cur;
		cur = next;
	}

	// merge the two lists
	BHeap *H = new BHeap();
	H->roots.push_back(revChd);
	this->merge(H);

	return ret;
}

void BHeap::decreaseKey(Node *x, int newKey) {
	// Precondition: x -> key > newKey
	x->key->rating = newKey;
	Node *y = x;
	Node *z = y->parent;
	while (z != nullptr && y->key->rating < z->key->rating)	{
		// swap contents
		std::swap(y->key, z->key);

		y = z;
		z = y->parent;
	}

	if (y->key->rating < this->min->key->rating) this->min = y;
}

void BHeap::Delete(Node *x) {
	decreaseKey(x, -INT_MAX);
	extractMin();
}

int main() {
	std::vector<group*> Groups(10);
	std::vector<student*> Students(100);

	BHeap* heap = new BHeap();

	std::ifstream in("base.txt");
	std::string temp, tempCountry, tempCity, tempSpec, tempR;
	double tempRating;

	for (int i = 0; i < 10; i++) {
		getline(in, temp);
		Groups[i] = new group(temp);
	}

	getline(in, temp);

	for (int i = 0; i < 100; i++) {
		getline(in, temp);
		getline(in, tempR);
		getline(in, tempCity);
		getline(in, tempCountry);
		getline(in, tempSpec);
		tempRating = atof(tempR.c_str());
		Students[i] = new student(temp, tempCountry, tempCity, tempRating, new group(tempSpec));

		heap->insert(new Node(Students[i]));

		for (int j = 0; j < 10; j++) {
			if (Groups[j]->name == temp) {
				Groups[j]->groupList.push_back(Students[i]);
			}
		}
		getline(in, temp);
	}

	heap->show();
	heap->extractMin();
	heap->show();

	/*heap->getMin()->getKey()->show();
	heap->extractMin();
	heap->getMin()->getKey()->show();*/


	system("pause");
    return 0;
}