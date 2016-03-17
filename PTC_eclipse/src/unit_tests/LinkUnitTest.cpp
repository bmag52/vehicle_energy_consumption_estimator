/*
 * LinkUnitTest.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: vagrant
 */

#include "/vagrant/PTC_eclipse/src/driver_prediction/Link.h"
#include <cstdlib>

using namespace std;
using namespace DriverPrediction;


//Takes in two links, and a boolean stating whether or not the links are equal (expected)
//Returns true if result is same as expected, false otherwise
bool compare_two_links(DriverPrediction::Link link1, DriverPrediction::Link link2, bool equal)
{
	bool result = link1.isEqual(link2);
	return result == equal;
}

//Takes in link and expected hash number
//Returns true if given link corresponds with given hash number, false otherwise
bool test_hash_functions(DriverPrediction::Link link1, int hash)
{
	bool matchesHash = link1.get_hash(link1) == hash;
	bool matchesLink = link1.new_link_from_hash(hash).isEqual(link1);
	return matchesHash && matchesLink;
}

DriverPrediction::Link * generate ()
{
	DriverPrediction::Link r[5];

	//First is link w/ num & dir == 0
	r[0] = Link();

	//Second and third are two identical links
	r[1] = Link(rand() % 2, rand());
	r[2] = Link(r[1].getDirection(), r[1].getNumber());

	//Fourth is random link
	r[3] = Link(rand() % 2, rand());

	// to make sure r[3] never equals r[2]
	while (compare_two_links(r[3], r[2], true) == true){
		r[3] = Link(rand() % 2, rand());
	}

	//Last is a final link
	r[4] = r[1].final_link();


	return r;
}

//write overall single method to be called
//generate 5 random Links: 2 of the same, one final link, one using basic Link() constructor
bool overall_Test()
{

	DriverPrediction:: Link * r = generate();
	bool testCompareLinks = compare_two_links(r[0], Link(), true);
	testCompareLinks = testCompareLinks && compare_two_links(r[1], r[2], true);
	testCompareLinks = testCompareLinks && compare_two_links(r[2], r[3], false);
	testCompareLinks = testCompareLinks && compare_two_links(r[4], r[4].final_link(), true);

	for (int i = 0; i < 5; i++)
	{
		int hash = 2 * r[i].getDirection() + r[i].getNumber();
		if (test_hash_functions(r[i], hash) == false)
		{
			//should throw error message stating i
			return false;
		}

	}

	return testCompareLinks;
}

