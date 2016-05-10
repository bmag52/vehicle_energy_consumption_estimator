/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <iostream>
#include <assert.h>

#include "unit_tests/UnitTests.h"
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

int main() {

//	dataManagement_ut();
//	dataCollection_ut();
	buildCity_ut();

	std::cout << "finished" << std::endl;

	return 0;
}
