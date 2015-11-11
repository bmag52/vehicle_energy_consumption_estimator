/*
 * helloworld.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: huongnvo
 */

#include <iostream>
using namespace std;

class HelloWorld {
	int width, height;

	void set_values(int, int);

	int area() {
		return width * height;
	}
};

void HelloWorld::set_values(int x, int y) {
	width = x;
	height = y;
}

int main() {
	HelloWorld hello;
	hello.set_values(12, 13);
	cout << "area: " << hello.area();
	return 0;
}
