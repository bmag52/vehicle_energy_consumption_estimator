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
	this->latDelta = .005;
	this->lonDelta = .005;
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

	std::ofstream outFile(this->xmlFile, std::ofstream::out | std::ofstream::binary);

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
	system("echo $PWD");

	ptree tree;
    read_xml(this->xmlFile, tree);
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


