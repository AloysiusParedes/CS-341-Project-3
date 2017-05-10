/*myvector.h*/

//
// myvector class --- a replacement for std::vector based on linked
// nodes of array "chunks".  The idea is to avoid the need to reallocate
// the underlying dynamic array (and copying elements) when the vector
// becomes full.
//
// <<Aloysius Paredes>>
// U. of Illinois, Chicago
// CS341, Fall 2016
// HW #03
//

#pragma once

#include <iostream>
#include <iterator>
#include <exception>
#include <stdexcept>

using namespace std;

//
// myvector<T>
//
template<typename T>
class myvector
{
private:
	//
	// One node of the vector, i.e. an array "chunk" of size nodeSize.
	//
	class Node
	{
	public:
		T           *elements;
		int          nodeSize;
		int          numElems;
		class Node  *next;

		Node(int nodesize)
			: elements{ new T[nodesize] }, nodeSize{ nodesize }, numElems{ 0 }, next{ nullptr }
		{ }

		~Node()
		{
			delete[] elements;
		}

		void push_back(const T& e)
		{
			if (numElems == nodeSize)
				throw std::exception("node is full!");

			elements[numElems] = e;
			numElems++;
		}
	};


	class MyIterator {
	public:
		//variables
		Node* currNode;	//which node are we on
		int index;	//cur index into that node

		//constructor
		MyIterator(Node* node) {
			currNode = node;
			index = 0;
		}

		//return an iterator one unit after the current iterator
		MyIterator& operator++() {
			index++; //move to next element

			//check if you have reached the node's limit
			if (index == currNode->nodeSize) {
				//check if you're at the tail
				if (currNode->next == nullptr)
					return *this;

				currNode = currNode->next; //move to next node
				index = 0; //start at the beginning of the elements array
			}

			return *this; //return this iterator
		}

		//return object at the index of the elements
		T& operator*() {
			return currNode->elements[index];
		}

		//check to see if right hand side doesn't equal the left hand side
		bool operator!=(const MyIterator& rhs) {
			//check if the current node is passed the last
			if (currNode == nullptr || index == currNode->numElems)
				return false;	
			return true;

			//UNCOMMENT BELOW TO TEST "==" operator
			//return !(*this == rhs);
		}

		bool operator==(const MyIterator& rhs) {
			if (currNode != nullptr && index == currNode->numElems)
				return true;
			else if (currNode == rhs.currNode)
				return true;
			return false;
		}

	};

		//
		// myvector:
		//
	private:
		Node  *head;
		Node  *tail;
		int    nodeSize;
		int    numNodes;
		int    numElems;

	public:
		//constructor
		myvector(int nodesize = 100) {
			head = new Node(nodesize);
			tail = head;
			nodeSize = nodesize;
			numNodes = 1;
			numElems = 0;
		}//end constructor

		//copy constructor
		myvector(const myvector& other){
			//set all values equal to other vector's values
			head = new Node(other.nodeSize);
			tail = head;
			nodeSize = other.nodeSize;
			numNodes = other.numNodes;
			numElems = other.numElems;

			//for traversing the nodes
			Node *currNode = head;
			Node *tempNode = other.head;
			int counter = 0;	//keep track of number of elements pushed back

			//loop until all values have been copied in
			while (counter != other.numElems) {
				//loop to copy all values from one node to the other
				for (int i = 0; i < tempNode->numElems; i++) {
					currNode->elements[i] = tempNode->elements[i];
					currNode->numElems++;
					counter++;
				}

				//check if node is full
				if (counter != other.numElems) {
					//create a new node
					Node *nextNode = new Node(currNode->nodeSize);
					//set the next pointer
					currNode->next = nextNode;
					//set the current node to the next one
					currNode = nextNode;
				}

				//update the tail to be the last node looked at
				tail = currNode;

				//move the the next node in the other vector
				tempNode = tempNode->next;
			}

		}//end copy constructor

		//deconstructor
		~myvector(){
			Node *curr = head; //to traverse the linked list
			Node *prev = nullptr; //hold on the previous

			//loop to delete all the nodes
			while (curr != nullptr) {
				prev = curr;
				curr = curr->next;
				delete prev;
			}
			
			//delete the head and tail pointers
			head = nullptr;
			tail = nullptr;
			delete head;
			delete tail;
		}//end deconstructor

		//function to return the number of elements in a myvector object
		int size()
		{
			return numElems;
		}//end size

		//function to return the max capacity the myvector object can hold
		int capacity()
		{
			return nodeSize * numNodes;
		}//end capacity

		//add element to vector
		void push_back(const T& e)
		{
			//check to see if tail is full
			if (tail->numElems == nodeSize) {
				//create a new node
				Node *newTail = new Node(nodeSize);

				//set new pointers
				tail->next = newTail;
				tail = newTail;

				//add new element
				tail->push_back(e);

				//update number of elements and number of nodes
				numElems++;
				numNodes++;

			}
			else {
				tail->push_back(e);
				numElems++;
			}
		}

		//[] operator to return element at that given input's index
		T& operator[](int i)
		{
			//check to see if input is within bounds
			if (i > numElems || i < 0)
				throw std::exception("invalid index");
			else {
				//keep track of elements index and location in the array
				int counter = 0;
				int elementsIndex = 0;

				//boolean flag to see if we found the element at that index
				bool found = false;

				//for node traversal
				Node *curr = head;

				//loop through nodes
				while (curr != nullptr) {
					//start looking at index 0 of elements array
					elementsIndex = 0;
					//loop through the indices
					for (int j = 0; j < curr->nodeSize; j++) {
						//see if you found it
						if (counter == i) {
							found = true;
							break;
						}

						//didn't find it, so keep looking
						counter++;
						elementsIndex++;
					}

					//see if you found it
					if (found == true)
						break;

					//move to the next node
					else
						curr = curr->next;
				}

				return curr->elements[elementsIndex];
			}//end operator[]
		}

		//function to return the value at the front of the myvector object
		T& front()
		{
			if (numElems == 0)
				throw std::exception("vector is empty!");

			return head->elements[0];
		}

		//function to return the value at the back of the myvector object
		T& back()
		{
			if (numElems == 0)
				throw std::exception("vector is empty!");

			return tail->elements[tail->numElems - 1];
		}

		//function to return an iterator of the beginning of the list
		MyIterator begin()
		{
			return MyIterator(head);
		}//end begin

		//function to return an iterator that's past-the-end of the list
		MyIterator end()
		{
			Node* pastTheEnd = tail + 1;

			return MyIterator(pastTheEnd);
		}//end end

	}; //end class
