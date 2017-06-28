#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <opencv2/core/core.hpp>
using namespace cv;

struct ObjNode
{
	/*Mat hist;*/
	int X;
	int Y;
	double Vx;
	double Vy;
	int upper;
	int lower;
	int left;
	int right;
	float height;
	int No;
	int forward_match;
	int behind_match;
	uchar undetected_cnt;

	ObjNode *next;
	ObjNode() :Vx(6), Vy(2), forward_match(-1), behind_match(-1), undetected_cnt(0){}
};

class Obj
{
public:
	Obj() :first(NULL), length(0), last(NULL){}

	void push(int x, int y, int up, int low, int l, int r, float height)
	{

		ObjNode *newnode = new ObjNode;
		length++;
		newnode->X = x; newnode->Y = y; newnode->upper = up; newnode->lower = low; newnode->left = l; newnode->right = r; newnode->height = height; newnode->No = length;
		if (first == NULL)
		{
			if (newnode == NULL) exit(0);
			newnode->next = first;
			first = newnode;
			last = newnode;
		}
		else
		{
			ObjNode *current = first;
			while (current->next != NULL)
				current = current->next;
			newnode->next = current->next;
			current->next = newnode;
			last = newnode;
		}
	}

	int get_length() { return length; }

	void empty()
	{
		ObjNode *p = first;
		while (p != NULL)
		{
			first = p->next;
			delete p;
			p = first;
		}
		this->length = 0;
		this->last = NULL;
	}

	/*下标操作符重载*/
	ObjNode* operator[] (int i)
	{
		ObjNode *p = first;
		for (int x = 0; x < i; x++)
		{
			p = p->next;
		}
		return p;
	}

	/*拷贝构造函数*/
	Obj(const Obj& r)
	{

		this->first = this->last = NULL;
		this->length = 0;
		ObjNode* p = r.first;
		while (p != NULL)
		{
			this->push(p->X, p->Y, p->upper, p->lower, p->left, p->right, p->height);
			ObjNode* cur = this->last;
			cur->Vx = p->Vx;
			cur->Vy = p->Vy;
			cur->No = p->No;
			cur->forward_match = p->forward_match;
			cur->behind_match = p->behind_match;
			cur->undetected_cnt = p->undetected_cnt;
			p = p->next;
		}
	}

	/*赋值操作符重载*/
	Obj& operator = (Obj& r)
	{

		ObjNode* p = r.first;
		this->empty();
		while (p != NULL)
		{
			this->push(p->X, p->Y, p->upper, p->lower, p->left, p->right, p->height);
			ObjNode* cur = this->last;
			cur->Vx = p->Vx;
			cur->Vy = p->Vy;
			cur->No = p->No;
			cur->forward_match = p->forward_match;
			cur->behind_match = p->behind_match;
			cur->undetected_cnt = p->undetected_cnt;
			p = p->next;
		}
		return *this;
	}

	/*判断是否链表为空*/
	bool noObject()
	{
		return !length;
	}

	ObjNode* getHead()
	{
		return this->first;
	}

	~Obj()
	{
		ObjNode *p = first;
		while (p != NULL)
		{
			first = p->next;
			delete p;
			p = first;
		}
		this->last = NULL;
		this->length = 0;
	}
private:
	ObjNode *first;
	int length;
	ObjNode *last;
};

#endif