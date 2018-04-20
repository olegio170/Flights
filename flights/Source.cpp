#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <list>
#include <new>   

using namespace std;

struct Time 
{
	int hour;
	int min;
};

struct TimeSpan
{
	Time start;
	Time end;
};

struct Data 
{
	string code;
	Time departure,
		arrival,
		flight,
		max;
};

struct TreeNode
{
	Data data;
	TreeNode *left, *right, *parent;
};

// Helper functions

Time subtractTime(Time first, Time second)
{
	Time result = { first.hour - second.hour, first.min - second.min };
	
	if (first.min < second.min)
	{
		result.hour--;
		result.min += 60;
	}
	
	return result;
}

// Compare two times
bool isFirstBigger(Time first, Time second)
{
	if (first.hour > second.hour)
		return true;
	else
		if (first.hour == second.hour && first.min > second.min)
			return true;
		else
			return false;
}

// == for time
bool isTimeEqual(Time first, Time second)
{
	if (first.hour == second.hour && first.min == second.min)
		return true;
	else
		return false;
}

TreeNode *createNode(Data data)
{
	TreeNode *temp = new TreeNode;
	temp->data = data;
	temp->left = temp->right = temp->parent = NULL;
	return temp;
}

// Starts from leaves recursively going up to root node
void updateMax(TreeNode *node)
{
	if (!node || !node->parent) // Root or undefined
		return;

	if (isFirstBigger(node->data.max, node->parent->data.max))
	{
		node->parent->data.max = node->data.max;
		updateMax(node->parent);
	}
}

void insertNode(TreeNode *root, TreeNode *newNode)
{
	while (root) // find place to insert
	{
		if (newNode->data.departure.hour > root->data.departure.hour) // decide go left or right
		{
			if (root->right) // NOT leaf
				root = root->right; // go deeper
			else 
			{
				newNode->parent = root;
				root->right = newNode;
				updateMax(newNode);
				break;
			}
		}
		else 
		{
			if (root->left) // NOT leaf
				root = root->left; // go deeper
			else
			{
				newNode->parent = root;
				root->left = newNode;
				updateMax(newNode);
				break;
			}
		}
	}
}

void delNode(TreeNode **root, TreeNode *node)
{
	if (!node) 
		return;
	bool isRoot = *root == node;
	TreeNode *p = node->parent;
	if (!node->left && !node->right) // Node is a leaf
	{
		if (isRoot)
		{
			*root = NULL; // Clear pointer to root
			return;	
		}

		// Update parent pointers
		if (p->left == node) // if node is left side from parent
			p->left = NULL;
		else
			p->right = NULL;
		
		// if parent max value depend on this node
		if(isTimeEqual(p->data.max, node->data.max))
		{ 
			p->data.max = p->data.arrival; // set parent max as default
			updateMax(p); // start upgrade max from parent
		}
	}
	else if(!node->left || !node->right) // Node have one child
		{
			if (node->right) // is child on the right
			{
				if (!isRoot)
				{
					// Update parent pointers
					if (p->left == node)
						p->left = node->right;
					else
						p->right = node->right;
				}
				else
					*root = node->right; // if root set pointer as for parent
				node->right->parent = p; // update parent poiner to child
			}
			else
			{
				if (!isRoot)
				{
					// Update parent pointers
					if (p->left == node)
						p->left = node->left;
					else
						p->right = node->left;
				}
				else
					*root = node->left; // if root set pointer as for parent
				node->left->parent = p; // update parent poiner to child
			}

			// if parent max value depend on this node
			if(!isRoot && isTimeEqual(p->data.max, node->data.max))
			{ 
				// Find most right node to start update max
				TreeNode *mostRight = node;
				while (node->right)
					mostRight = node->right;
				
				updateMax(mostRight);
			}
		}
		else // node have two child
		{
			TreeNode *newNode = node->left, *toUpdate_1 = node->right, *toUpdate_2 = NULL;
			while (newNode->right) // find node to place instead of del node
				newNode = newNode->right;
			
			newNode->right = node->right; // connect right subtree
			if (newNode != node->left) // if finded newNode is not a del node child 
			{
				newNode->parent->right = NULL; // unlink new node from parent
				newNode->left = node->left; // link left subtree to newnode
				node->left->parent = newNode; // link left child parent ptr to newNode
				toUpdate_2 = newNode->parent; // save poiner to start update
			}
			else
				newNode->left = NULL; 

			newNode->parent = p; // set parent to new node
			
			if (!isRoot)
			{
				p->left = newNode; // set new node as child of del node parent
				node->right->parent = newNode; // set parent poiner to right sub tree
			}
			else
				*root = newNode; // change root poiner

			// update max fields from two sides
			updateMax(toUpdate_1);
			updateMax(toUpdate_2);
		}

	delete node;
}

TreeNode* readFromFile()
{
	TreeNode *treeRoot = NULL;

	ifstream inputFile;
	inputFile.open("input.txt");

	Data temp; // Buffer to read
	TreeNode *node; // Buffer to new node ptr
	do 
	{
		inputFile >> temp.code >>
			temp.departure.hour >> temp.departure.min >>
			temp.arrival.hour >> temp.arrival.min;

		temp.flight = subtractTime(temp.arrival, temp.departure); // Calc flight time
		temp.max = temp.arrival; // set default value

		node = createNode(temp);
		if (!treeRoot)  // Tree is empty
		{
			treeRoot = node;
			insertNode(NULL, node);
		}
		else
			insertNode(treeRoot, node);
	} while (!inputFile.eof());
	
	return treeRoot;
}

void recInProgres(TreeNode *root, Time start, Time end, list <TreeNode *> &list)
{
	if (!root)
		return;

	// IF NOT started after end AND NOT finished before start
	if (!isFirstBigger(start, root->data.arrival) && !isFirstBigger(root->data.departure, end))
		list.push_back(root); // Add to result list

	// Recursion calls
	recInProgres(root->left, start, end, list);
	recInProgres(root->right, start, end, list);
}

list<TreeNode *> getInProgress(TreeNode *root, Time start, Time end)
{
	list <TreeNode *> result;
	recInProgres(root, start, end, result);
	return result;
}


void recFinished(TreeNode *root, Time start, Time end, list <TreeNode *> &list)
{
	if (!root)
		return;

	// IF NOT finished after end AND finished after start
	if (!isFirstBigger(root->data.arrival, end) && isFirstBigger(root->data.arrival, start))
		list.push_back(root); // Add to result list

	// Recursion calls
	recFinished(root->left, start, end, list);
	recFinished(root->right, start, end, list);
}

list<TreeNode *> getFinished(TreeNode *root, Time start, Time end)
{
	list <TreeNode *> result;
	recFinished(root, start, end, result);
	return result;
}

void recLatestArrival(TreeNode *root, Time h, Time *max)
{
	if (!root)
		return;

	/*
		In right subtree we have bigger values, so we don't
		need to check right part if current value is not less
		than h.
	*/
	if (isFirstBigger(h, root->data.departure)) // Check if h bigger than node departure
	{
		if (isFirstBigger(root->data.arrival, *max)) // If value bigger than max
			*max = root->data.arrival; // set value as result
		recLatestArrival(root->right, h, max); // Recursion call to right  
	}

	recLatestArrival(root->left, h, max); // Recursion call to left  
}

Time getLatestArrival(TreeNode *root, Time h)
{
	Time max = { -1,0 }; 

	while (root && isFirstBigger(root->data.departure, h)) // Find node to start
		root = root->left;

	recLatestArrival(root, h, &max);
	return max;
}

void recNearestDeparture(TreeNode *root, Time h, Time *min, Time *minDelta)
{
	if (!root)
		return;

	/*
		Like in previous task, in left subtree we have less values,
		so we don't need to check left part if current value is not bigger
		then h.
	*/

	if (isFirstBigger(root->data.departure, h)) // Check if h less than node departure
	{
		Time delta = subtractTime(root->data.departure, h); // 'distance' from node departure to h
		if (isFirstBigger(*minDelta, delta)) // Check if delta less than current minimum delta
		{
			*minDelta = delta; // Save delta to future comparisons
			*min = root->data.departure; // Set node value as min time
		}

		recNearestDeparture(root->left, h, min, minDelta); // Recursion call to left  
	}

	recNearestDeparture(root->right, h, min, minDelta); // Recursion call to right  
}

Time getNearestDeparture(TreeNode *root, Time h)
{
	Time min = {-1, 0}, minDelta = { 255, 0 };
	while (root && isFirstBigger(h, root->data.departure))
		root = root->right;

	recNearestDeparture(root, h, &min, &minDelta);
	return min;
}

// Output helper functions

string getTimeStr(Time t)
{
	stringstream ss;
	ss << setfill('0') << setw(2) << t.hour << ':' << setw(2) << t.min;
	return  ss.str();
} 

void printNode(TreeNode *ptr)
{
	cout << ptr->data.code << endl <<
		"Departure: " << getTimeStr(ptr->data.departure) << endl <<
		"Arrival: " << getTimeStr(ptr->data.arrival) << endl <<
		"Flight: " << getTimeStr(ptr->data.flight) << endl <<
		"Max Af: " << getTimeStr(ptr->data.max) << endl << endl;
}

string getTimeSpanStr(TimeSpan ts)
{
	return "[" + getTimeStr(ts.start) + ',' + getTimeStr(ts.end) + ']';
}

int main()
{
	// 1 Task
	TreeNode *root = readFromFile();

	// 2 Task
	TimeSpan timeSpan = {{ 12, 00 }, { 14,00 }};
	list <TreeNode *> finishedList = getFinished(root, timeSpan.start, timeSpan.end);
	cout << "[TASK2]Finished list for " << getTimeSpanStr(timeSpan) << endl << endl;
	for (TreeNode *ptr : finishedList)
		printNode(ptr);

	// 3 Task
	timeSpan = {{ 12,00 }, { 14,00 }};
	list<TreeNode *> inProgressList = getInProgress(root, timeSpan.start, timeSpan.end);
	cout << endl << "[TASK3]In progress list for " << getTimeSpanStr(timeSpan)  << endl << endl;
	for (TreeNode *ptr : inProgressList)
		printNode(ptr);

	// 4 Task
	Time h = { 11,30 };
	Time max = getLatestArrival(root, h);
	cout << endl << "[TASK4]Latest arrival for h = " << getTimeStr(h) << endl;
	cout << "Time :" << getTimeStr(max) << endl;

	// 5 Task
	h = { 13,30 };
	Time min = getNearestDeparture(root, h);
	cout << endl << "[TASK5]Nearest departure for h = " << getTimeStr(h) << endl;
	cout << "Time :" << getTimeStr(min) << endl;
	
	cin.ignore(256, '\n');
	cin.get();

	return 0;
}