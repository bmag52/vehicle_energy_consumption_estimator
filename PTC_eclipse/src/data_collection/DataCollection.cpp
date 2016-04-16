/*
 * MakeOSM.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "DataCollection.h"

using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::lexical_cast;

using namespace boost::property_tree;

namespace PredictivePowertrain {

DataCollection::DataCollection() {
	this->latDelta = .02;
	this->lonDelta = .02;
	checkDataFoler();
}

DataCollection::DataCollection(double latDelta, double lonDelta) {
	this->latDelta = latDelta;
	this->lonDelta = lonDelta;
	checkDataFoler();
}

// must call functions in specific order
void DataCollection::pullData(double lat, double lon) {
	pullSRTMData(lat, lon);
	pullOSMData(lat, lon);
}

void DataCollection::pullOSMData(double lat, double lon) {
	std::cout << "pulling OSM data ..." << std::endl;

	this->mapFile = "mapFile_";
	this->mapFile += lexical_cast<std::string>(lat) + "_";
	this->mapFile += lexical_cast<std::string>(lon) + ".xml";

	std::string mapFilePath = this->dataFolder + "/" + this->mapFile;

	std::ifstream test(mapFilePath);
	if(!test)
	{
		double lowLat = lat - .5*this->latDelta;
		double lowLon = lon - .5*this->lonDelta;
		double hiLat = lat + .5*this->latDelta;
		double hiLon = lon + .5*this->lonDelta;

		std::string serverName = "overpass-api.de";
		std::string getCommand = "/api/map?bbox=";
		getCommand += lexical_cast<std::string>(lowLon) + ",";
		getCommand += lexical_cast<std::string>(lowLat) + ",";
		getCommand += lexical_cast<std::string>(hiLon) + ",";
		getCommand += lexical_cast<std::string>(hiLat);

		queryFile(serverName, getCommand, this->dataFolder + "/" + this->mapFile);
	}
	test.close();

	ptree tree;
	read_xml(mapFilePath, tree);

	const ptree& formats = tree.get_child("osm", empty_ptree());
	int nodeCount = 0;
	int wayCount = 0;
	std::cout << "OSM Data Size: " <<  formats.size() << std::endl;

	BOOST_FOREACH(const ptree::value_type & f, formats)
	{

		std::string tagName = lexical_cast<std::string>(f.first);
		std::cout << "============= " << tagName << " =============" << std::endl;

		if(!tagName.compare("node"))
		{
			const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());

			long int id;
			double lat, lon;

			BOOST_FOREACH(const ptree::value_type &v, attributes)
			{
				std::string attr = lexical_cast<std::string>(v.first.data());

				if(!attr.compare("id"))
				{
					id = lexical_cast<long int>(v.second.data());
				} else if(!attr.compare("lat")) {
					lat = lexical_cast<double>(v.second.data());
				} else if(!attr.compare("lon")) {
					lon = lexical_cast<double>(v.second.data());
				} else {
					break;
				}

				std::cout << attr << " = " << v.second.data() << std::endl;
			}

			nodeCount++;
			int ele = getElevation(lat, lon);
			Node* node = new Node(lat, lon, ele, id);
			this->nodeMap.addEntry(nodeCount, node);

		} else if (!tagName.compare("way")) {

			// look at way attributes to get id
			long int id;
			const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
			BOOST_FOREACH(const ptree::value_type &v, attributes)
			{
				std::string attr = lexical_cast<std::string>(v.first.data());
				std::cout << attr << " = " << v.second.data() << std::endl;

				if(!attr.compare("id"))
				{
					id = lexical_cast<long int>(v.second.data());
					break;
				}
			}

			// get children of way specifically looking for node IDs
			int count = 0;
			GenericMap<int, long int>* nodeIDs = new GenericMap<int, long int>();
			BOOST_FOREACH(const ptree::value_type &v, f.second)
			{
				std::string childTagName = lexical_cast<std::string>(v.first);

				// look at child attributes to get node IDs
				if(!childTagName.compare("nd"))
				{
					std::cout << "- " << childTagName << " -" << std::endl;

					long int ref;
					const ptree & attributes = v.second.get_child("<xmlattr>", empty_ptree());
					BOOST_FOREACH(const ptree::value_type &z, attributes)
					{
						std::string attr = lexical_cast<std::string>(z.first.data());

						if(!attr.compare("ref"))
						{
							ref = lexical_cast<long int>(z.second.data());
							std::cout << attr << " = " << z.second.data() << std::endl;
							break;
						}
					}
					count++;
					nodeIDs->addEntry(count, ref);
				}
			}

			wayCount++;
			Way* way = new Way(nodeIDs, id);
			this->wayMap.addEntry(wayCount, way);
		}
	}
}

void DataCollection::pullSRTMData(double lat, double lon) {
	std::cout << "pulling SRTM data ... " << std::endl;

	// get bin for given lat and lon
	std::string latBin = getBin(60.0, -56.75, 24, lat, true);
	std::string lonBin = getBin(176.56, -180.0, 72, lon, false);

	// create srtm file name from bins
	std::string srtmName = "srtm_" + lonBin + "_" + latBin;

	// check if srtm file already exists and pull it if not
	this->eleFile = srtmName + ".asc";
	std::ifstream test(this->dataFolder + "/" + this->eleFile);
	if(!test)
	{
		// queary srtm for ele data
		std::string srtmZip = this->dataFolder + "/" + srtmName + ".zip";
		std::string serverName = "srtm.csi.cgiar.org";
		std::string getCommand = "/SRT-ZIP/SRTM_v41/SRTM_Data_ArcASCII/" + srtmName + ".zip";

		// place download
		std::string wget = "wget -O " + srtmZip + " http://" + serverName + getCommand;
		system(wget.c_str());

		// unpack zip
		std::string unzip = "sudo unzip " + srtmZip + " -d " + this->dataFolder;
		system(unzip.c_str());
	}
	test.close();

	// read file
	std::ifstream ifs;
	ifs.open(this->dataFolder + "/" + this->eleFile);
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

		this->numEleLats = eleFeatures[0];
		this->numEleLons = eleFeatures[1];
		this->eleLowerLeftLon = eleFeatures[2];
		this->eleLowerLeftLat = eleFeatures[3];
		this->eleCellSize = eleFeatures[4];
		this->voidEle = eleFeatures[5];

		int row = 0;
		int col = 0;
		int **eleRay = new int*[this->numEleLons];
		while(getline(ifs, line))
		{
			std::stringstream ss(line);
			eleRay[row] = new int[this->numEleLats];
			while(ss >> eleRay[row][col]) { col++; }
			col = 0;
			std::cout << row++ << std::endl;
		}
		ifs.close();
		this->eleData = eleRay;
	}
}

std::string DataCollection::getBin(double hi, double lo, int bins, double latLon, bool isLat) {
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

void DataCollection::queryFile(std::string serverName, std::string getCommand, std::string fileName) {

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

void DataCollection::checkDataFoler() {
	struct stat sb;
	if(!(stat(this->dataFolder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
	{
		std::string mkdir = "mkdir " + this->dataFolder;
		system(mkdir.c_str());
	}
}

const ptree& DataCollection::empty_ptree() {
    static ptree t;
    return t;
}

// lat lon increase L->R and B->T
int DataCollection::getElevation(double lat, double lon) {

	double nextLat, nextLon, latEleDiff, lonEleDiff;

	for(int k = this->numEleLats; k >= 0; k--)
	{
		nextLat = this->eleLowerLeftLat + (this->numEleLats-k+1)*this->eleCellSize;
		if(lat < nextLat)
		{
			for(int l = 0; l < this->numEleLons; l++)
			{
				nextLon = this->eleLowerLeftLon + (l+1)*this->eleCellSize;
				if(lon < nextLon)
				{
					int elevation = this->eleData[l][k];

					if(elevation == this->voidEle)
					{
						std::cout << "------- void elevation -------" << std::endl;
						return elevation;
					} else if((k > 0) && (l > 0))
					{
						latEleDiff = this->eleData[l-1][k] - this->eleData[l][k];
						lonEleDiff = this->eleData[l][k-1] - this->eleData[l][k];

						latEleDiff *= this->eleCellSize - (nextLat - lat);
						lonEleDiff *= this->eleCellSize - (nextLon - lon);

						elevation += (latEleDiff + lonEleDiff);
					}
					return elevation;
				}
			}
		}
	}
	return -1;
}

GenericMap<int, Node*>* DataCollection::getNodeMap() {
	return &this->nodeMap;
}

GenericMap<int, Way*>* DataCollection::getWayMap() {
	return &this->wayMap;
}

int DataCollection::getVoidEle() {
	return this->voidEle;
}

}
