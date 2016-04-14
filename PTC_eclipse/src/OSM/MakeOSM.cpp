/*
 * MakeOSM.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "MakeOSM.h"

using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::lexical_cast;

using namespace boost::property_tree;

namespace PredictivePowertrain {

MakeOSM::MakeOSM() {
	this->latDelta = .05;
	this->lonDelta = .05;
}

MakeOSM::MakeOSM(double latDelta, double lonDelta) {
	this->latDelta = latDelta;
	this->lonDelta = lonDelta;
}

void MakeOSM::pullOSMData(double lat, double lon) {
	double lowLat = lat - .5*this->latDelta;
	double lowLon = lon - .5*this->lonDelta;
	double hiLat = lat + .5*this->latDelta;
	double hiLon = lon + .5*this->lonDelta;

	std::string serverName = "overpass-api.de";
	std::string getCommand = "/api/map?bbox=";
	getCommand += lexical_cast<std::string>(lowLon); getCommand += ",";
	getCommand += lexical_cast<std::string>(lowLat); getCommand += ",";
	getCommand += lexical_cast<std::string>(hiLon); getCommand += ",";
	getCommand += lexical_cast<std::string>(hiLat);

	queryFile(serverName, getCommand, this->mapFile);
}

void MakeOSM::pullSRTMData(double lat, double lon) {

	// get bin for given lat and lon
	std::string latBin = getBin(60.0, -56.75, 24, lat, true);
	std::string lonBin = getBin(176.56, -180.0, 72, lon, false);

	// create srtm file name from bins
	std::string srtmName = "srtm_";
	srtmName += lonBin; srtmName += "_";
	srtmName += latBin;

	// check if srtm file already exists
	// pull it if not
	this->eleFile = srtmName + ".asc";
	std::ifstream test(this->eleFile);
	if(!test)
	{
		// queary srtm for ele data
		std::string serverName = "srtm.csi.cgiar.org";
		std::string getCommand = "/SRT-ZIP/SRTM_v41/SRTM_Data_ArcASCII/";

		getCommand += srtmName;
		getCommand += ".zip";

		std::string wgetData = "wget http://";
		wgetData += serverName;
		wgetData += getCommand;
		const char* wgetDataChar = wgetData.c_str();
		system(wgetDataChar);

		// unpack zip
		std::string unzipData = "unzip ";
		unzipData += srtmName;
		unzipData += ".zip";
		const char* unzipDataChar = unzipData.c_str();
		system(unzipDataChar);
	}
	test.close();

	// read file
	std::ifstream ifs;
	ifs.open(this->eleFile);
	bool isopen = ifs.is_open();
	if(ifs.is_open())
	{
		std::string line;
		double eleFeatures[6];

		std::string feature;
		for(int i = 0; i < 6; i++)
		{
			getline(ifs, line);
			std::stringstream ss(line);
			ss >> feature;
			ss >> eleFeatures[i];
		}

		this->numEleCols = eleFeatures[0];
		this->numEleRows = eleFeatures[1];
		this->eleLowerLeftLon = eleFeatures[2];
		this->eleLowerLeftLat = eleFeatures[3];
		this->eleCellSize = eleFeatures[4];
		this->voidEle = eleFeatures[5];

		int row = 0;
		int col = 0;
		int **eleRay = new int*[this->numEleRows];
		while(getline(ifs, line))
		{
			col = 0;
			std::stringstream ss(line);
			eleRay[row] = new int[this->numEleCols];
			while(ss >> eleRay[row][col]) { col++; }
			row++;
		}
		ifs.close();
		this->eleData = eleRay;
	}
}

std::string MakeOSM::getBin(double hi, double lo, int bins, double latLon, bool isLat) {
	double inc = (hi - lo) / bins;
	int bin;
	for(bin = 1; bin < bins; bin++)
	{
		if(latLon < (lo + bin*inc)) { break; }
	}

	if(isLat) { bin = bins - bin + 1; }

	std::string out;
	if(bin < 10)
	{
		out = "0";
		out += lexical_cast<std::string>(bin);
	} else {
		out = lexical_cast<std::string>(bin);
	}
	return out;
}

void MakeOSM::queryFile(std::string serverName, std::string getCommand, std::string fileName) {

	std::ofstream outFile(fileName, std::ofstream::out | std::ofstream::binary);

	boost::asio::io_service io_service;

	// Get a list of endpoints corresponding to the server name.
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(serverName, "http");
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end;

	// Try each endpoint until we successfully establish a connection.
	tcp::socket socket(io_service);
	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_iterator != end)
	{
		socket.close();
		socket.connect(*endpoint_iterator++, error);
	}

	boost::asio::streambuf request;
	std::ostream request_stream(&request);

	request_stream << "GET " << getCommand << " HTTP/1.0\r\n";
	request_stream << "Host: " << serverName << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	// Send the request.
	boost::asio::write(socket, request);

	// Read the response status line.
	boost::asio::streambuf response;
	boost::asio::read_until(socket, response, "\r\n");

	// Check that response is OK.
	std::istream response_stream(&response);
	std::string http_version;
	response_stream >> http_version;
	unsigned int status_code;
	response_stream >> status_code;
	std::string status_message;
	std::getline(response_stream, status_message);


	// Read the response headers, which are terminated by a blank line.
	boost::asio::read_until(socket, response, "\r\n\r\n");

	// Process the response headers.
	std::string header;
	while (std::getline(response_stream, header) && header != "\r")
	{
	}

	// Write whatever content we already have to output.
	if (response.size() > 0)
	{
		outFile << &response;
	}
	// Read until EOF, writing data to output as we go.
	while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
	{
		outFile << &response;
	}
}

Road* MakeOSM::getRoads() {
	// check if osm file exists
	Road* roads;
	std::ifstream f(this->testXml);
	if(!f) { return roads; }

	ptree tree;
    read_xml(this->testXml, tree);
    const ptree & formats = tree.get_child("pets", empty_ptree());

    BOOST_FOREACH(const ptree::value_type & f, formats){
        std::string at = f.first + ".<xmlattr>";
        const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
        std::cout << "Extracting attributes from " << at << ":" << std::endl;

        BOOST_FOREACH(const ptree::value_type &v, attributes){
            std::cout << "First: " << v.first.data() << " Second: " << v.second.data() << std::endl;
        }
    }
}

const ptree& MakeOSM::empty_ptree() {
    static ptree t;
    return t;
}

}


