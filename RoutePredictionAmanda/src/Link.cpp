/*
 * Link.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#include "Link.h"
#include "boost/utility/binary.hpp"

namespace DriverPrediction {

Link::Link() {
	link_number = 0;
	link_direction = 0;

}

Link::Link(int direction, int linkNumber) {
	link_direction = direction;
	link_number = linkNumber;
}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

Link Link::copy(int direction, int linkNumber) {
	return Link(direction, linkNumber);
}



int Link::get_hash(Link link) {
	std::string linkDirToString = std::to_string(link_direction);
	std::string linkNumToBin = convertToBinary(link_number);
	std::string totalString = linkNumToBin + linkDirToString;
	return BOOST_BINARY(totalString);
	//???

	//return BOOST_BINARY( 100 111000 01 1 110 );
	//MATLAB CODE
	//hash = bin2dec(strcat(dec2bin(obj.link_number),num2str(obj.link_direction)));
}

bool Link::isEqual(Link other) {
	if (other.link_direction == link_direction && other.link_number == link_number) {
		return true;
	} else {
		return false;
	}
}

int Link::getNumber() {
	return link_number;
}

int Link::getDirection() {
	return link_direction;
}

//converts to binary
std::string Link::convertToBinary(int number) {
	int result = number;
	std::string binary = "";
	while (number != 0) {
		int bin = result % 2;
		binary = bin + binary;
		result = result / 2;
	}
	return binary;
}

Link Link::new_link_from_hash(int hash) {
	std::string binaryStuff = convertToBinary(hash);
	///???

	//MATLAB CODE
	/**
	 * hash_bin = dec2bin(hash);
            dir = bin2dec(hash_bin(end));
            link_num = bin2dec(hash_bin(1:(end-1)));
            new_link = Link(link_num, dir);
	 */
}

// %represents the link at the end of a route, when the vehicle stopes
Link Link::final_link() {
	return Link(0, 0);
}

} /* namespace DriverPrediction */
