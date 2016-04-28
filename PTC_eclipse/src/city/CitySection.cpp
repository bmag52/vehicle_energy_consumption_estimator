
#include "CitySection.h"
#include <string>

namespace PredictivePowertrain {

CitySection::CitySection() {

}

CitySection::CitySection(std::string roadType, int x, int y) {
	this->roadType = roadType;
	this->x_dim = x;
	this->y_dim = y;
}

CitySection::~CitySection() {

}


} /* namespace PredictivePowertrain */
