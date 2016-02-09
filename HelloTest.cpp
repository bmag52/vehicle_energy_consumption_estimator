//============================================================================
// Name        : Test.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

int main() {
	Link LinkA;
	Link* LinkB; // ptr is dynamic ptr is same as array

	LinkA = Link(3,4); // no new means the memory is there already, just fuck with that shit
	LinkB = new Link(3,4); // new means put that shit on the heap

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
