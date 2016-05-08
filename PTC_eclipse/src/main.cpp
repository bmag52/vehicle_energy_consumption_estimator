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
//	buildCity_ut();

	// place wget download
//	std::string wget = "wget -O image.jpg https://www.openstreetmap.org/export#map=16/47.6174/-122.3211";
//	system(wget.c_str());

	// place wkhtmltopdf
//	std::string wkhtmltopdf = "wkhtmltopdf https://www.openstreetmap.org/export#map=16/47.6173/-122.3355 image.pdf";
//	system(wkhtmltopdf.c_str());

	// place cutycapt
	std::string cutycapt = "cutycapt --url=https://www.openstreetmap.org/export#map=18/47.6173/-122.3355 --out=image.png --min-width=2400 --min-height=1800 --delay=2000";
	system(cutycapt.c_str());

	std::cout << "finished" << std::endl;

	return 0;
}
