/*
 * Goal.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#include "Goal.h"

namespace PredictivePowertrain {

Goal::Goal(int destination)
{
	this->destination = destination;
}

Goal::Goal(int destination, std::vector<float>* bins)
{
	this->destination = destination;
	this->bins = bins;
	this->numSeen = 1;
}

Goal::Goal(Goal * other)
{
	this->destination = other->destination;
    this->bins = new std::vector<float>(other->getBins()->size());
	for (int i = 0; i < this->bins->size(); i++)
    {
        this->bins->at(i) = other->bins->at(i);
	}
	this->numSeen = other->numSeen;
}

bool Goal::isSimilar(Goal * other)
{
	if (this->bins->size() == other->getBins()->size())
    {
		for (int i = 0; i < this->bins->size(); i++)
        {
			if (this->bins->at(i) != other->bins->at(i))
            {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool Goal::isEqual(Goal * other)
{
	return isSimilar(other) && this->destination == other->destination;
}

int Goal::getHash() const
{
	int hash = this->destination;
	int bits;
	for (int i = 0; i < this->bins->size(); i++) {
        bits = (int) std::log2(this->bins->at(i));
        hash = hash << bits;
        hash += this->bins->at(i);
	}
	return hash;
}

Goal::Goal() {
}

void Goal::incrementNumSeen()
{
	this->numSeen++;
}

void Goal::setNumSeen(int numSeen)
{
	this->numSeen = numSeen;
}

std::vector<float>* Goal::getBins()
{
	return this->bins;
}

Goal::~Goal() {
}

int Goal::getNumSeen()
{
	return this->numSeen;
}

} /* namespace PredictivePowertrain */
