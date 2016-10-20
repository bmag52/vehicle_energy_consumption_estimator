/*
 * DataCollection.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "../data_management/DataCollection.h"

using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::lexical_cast;
using boost::property_tree::ptree;

using namespace boost::property_tree;

namespace PredictivePowertrain {

DataCollection::DataCollection() {
    this->initialize(.0002, .0002);
}

DataCollection::DataCollection(double latDelta, double lonDelta) {
    this->initialize(latDelta, lonDelta);
}
    
void DataCollection::initialize(double latDelta, double lonDelta) {
    this->latDelta = latDelta;
    this->lonDelta = lonDelta;
    this->zoomMax = 19;
    this->zoomMin = 18;
    this->wayCount = 0;
    this->boundsCountXML = 0;
    this->boundsCountPNG = 0;
    this->setZoomSpreads();
    checkDataFoler();
}

// must call functions in specific order
void DataCollection::pullDataXML(double lat, double lon) {
	this->pullSRTMData(lat, lon);
	this->pullOSMDataXML(lat, lon);
}

int DataCollection::pullDataPNG(double lat, double lon) {
	this->pullSRTMData(lat, lon);
	return this->pullOSMDataPNG(lat, lon);
}

void DataCollection::pullOSMDataXML(double lat, double lon) {
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
	std::cout << "OSM Data Size: " <<  formats.size() << std::endl;

	BOOST_FOREACH(const ptree::value_type & f, formats)
	{

		std::string tagName = lexical_cast<std::string>(f.first);
		std::cout << "============= " << tagName << " =============" << std::endl;

		if(!tagName.compare("node"))
		{
			long int nodeID;
			float lat, lon;
			const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
			BOOST_FOREACH(const ptree::value_type &v, attributes)
			{
				std::string attr = lexical_cast<std::string>(v.first.data());

				if(!attr.compare("id"))
				{
					nodeID = lexical_cast<long int>(v.second.data());
					std::cout << attr << " = " << nodeID << std::endl;
				} else if(!attr.compare("lat")) {
					lat = lexical_cast<float>(v.second.data());
					std::cout << attr << " = " << lat << std::endl;
				} else if(!attr.compare("lon")) {
					lon = lexical_cast<float>(v.second.data());
					std::cout << attr << " = " << lon << std::endl;
				}

			}
			int ele = getElevation(lat, lon);
			Node* node = new Node(lat, lon, ele, nodeID);
			this->nodeMap.addEntry(nodeID, node);

		} else if (!tagName.compare("way")) {

			// get children of way specifically looking for node IDs and way types to see if way is desireable
			int waySpeed = -1;
			int refCount = 0;
			std::string wayType;
			bool isDesiredWay = false;
			GenericMap<int, long int>* nodeIDs = new GenericMap<int, long int>();
			BOOST_FOREACH(const ptree::value_type &v, f.second)
			{
				std::string childTagName = lexical_cast<std::string>(v.first);

				// look at child attributes to get node IDs
				if(!childTagName.compare("nd") || !childTagName.compare("tag"))
				{
					std::cout << "- " << childTagName << " -" << std::endl;
					const ptree & attributes = v.second.get_child("<xmlattr>", empty_ptree());
					BOOST_FOREACH(const ptree::value_type &z, attributes)
					{
						std::string attr = lexical_cast<std::string>(z.first.data());
						if(!childTagName.compare("nd") && !attr.compare("ref"))
						{
							long int ref = lexical_cast<long int>(z.second.data());
							std::cout << attr << " = " << z.second.data() << std::endl;
							refCount++;
							nodeIDs->addEntry(refCount, ref);
						} else if(!childTagName.compare("tag") && !attr.compare("v")) {
							std::string val = lexical_cast<std::string>(z.second.data());
							std::size_t found = val.find("mph");
							if(!val.compare("motorway") || !val.compare("primary") || !val.compare("secondary") || !val.compare("tertiary")
									|| !val.compare("motorway link") || !val.compare("primary link") || !val.compare("unclassified")
									|| !val.compare("road") || !val.compare("residential") || !val.compare("service"))
							{
								wayType = val;
								isDesiredWay = true;
								std::cout << attr << " = " << val << std::endl;
							} else if(found!=std::string::npos) {
								std::stringstream ss(val);
								ss >> waySpeed;
								std::cout << attr << " = " << val << std::endl;
							}
						}
					}
				}
			}

			if(isDesiredWay)
			{
				// grab the way ID and bounce
				long int wayID;
				const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
				BOOST_FOREACH(const ptree::value_type &v, attributes)
				{
					std::string attr = lexical_cast<std::string>(v.first.data());
					if(!attr.compare("id"))
					{
						wayID = lexical_cast<long int>(v.second.data());
						std::cout << attr << " = " << wayID << std::endl;
						break;
					}
				}

				this->wayCount++;
				Way* way = new Way(nodeIDs, wayID, wayType, waySpeed);
				this->wayMap.addEntry(this->wayCount, way);
			}
            
		} else if(!tagName.compare("bounds")) {

			double maxLat, maxLon, minLat, minLon;
			const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
			BOOST_FOREACH(const ptree::value_type &v, attributes)
			{
				std::string attr = lexical_cast<std::string>(v.first.data());
				if(!attr.compare("minlat"))
				{
					minLat = lexical_cast<double>(v.second.data());
					std::cout << attr << " = " << minLat << std::endl;
				} else if(!attr.compare("minlon")) {
					minLon = lexical_cast<double>(v.second.data());
					std::cout << attr << " = " << minLon << std::endl;
				} else if(!attr.compare("maxlat")) {
					maxLat = lexical_cast<double>(v.second.data());
					std::cout << attr << " = " << maxLat << std::endl;
				} else if(!attr.compare("maxlon")) {
					maxLon = lexical_cast<double>(v.second.data());
					std::cout << attr << " = " << maxLon << std::endl;
				}
			}
			this->boundsCountXML++;
			Bounds* bounds = new Bounds(maxLat, maxLon, minLat, minLon);
			boundsMapXML.addEntry(this->boundsCountXML, bounds);
		}
	}
}

void DataCollection::pullSRTMData(double lat, double lon) {

	std::cout << "Gathering Elevation Data" << std::endl;

	// get bin for given lat and lon
	double latMax = 60.0;
	double latMin = -56.75;
	double lonMax = 176.56;
	double lonMin = -180.0;

	assert(lat < latMax && lat > latMin && lon < lonMax && lon > lonMin);

	std::string latBin = getBin(latMax, latMin, 24, lat, true);
	std::string lonBin = getBin(lonMax, lonMin, 72, lon, false);

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
	std::string eleFilePath = this->dataFolder + "/" + this->eleFile;
	ifs.open(eleFilePath.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
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

		this->numEleLatsSRTM = eleFeatures[0];
		this->numEleLonsSRTM = eleFeatures[1];
		this->eleLowerLeftLon = eleFeatures[2];
		this->eleLowerLeftLat = eleFeatures[3];
		this->eleCellSize = eleFeatures[4];
		this->voidEle = eleFeatures[5];

		int row = 0;
		int col = 0;
		int **eleRay = new int*[this->numEleLonsSRTM];
		while(getline(ifs, line))
		{
			std::stringstream ss(line);
			eleRay[row] = new int[this->numEleLatsSRTM];
			while(ss >> eleRay[row][col]) { col++; }
			if(row % 1000 == 0) { std::cout << row << " ..." << std::endl; }
			col = 0;
			row++;
		}
		ifs.close();
		this->eleDataSRTM = eleRay;
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
    
void DataCollection::updateElevationData(GenericMap<long int, Road*>* roads)
{
    const char* latPrint = "\"lat\"";
    const char* lonPrint = "\"lon\"";
    const char* shapePrint = "\"shape\"";
    
    std::string server = "elevation.mapzen.com";
    std::string command = "/height?json=";
    std::string apiKey = "&api_key=mapzen-4mUVq1A";
    std::string elevFile = this->dataFolder + "/newRoadElevation.txt";

    roads->initializeCounter();
    GenericEntry<long int, Road*>* nextRoad = roads->nextEntry();
    while(nextRoad != NULL)
    {
        std::cout << nextRoad->key << std::endl;
        
        std::stringstream json;
        json << "{" << shapePrint << ":[";
        
        // iterate through nodes and add them to string to be concatenated yo
        bool alreadyAssignedElevation = false;
        nextRoad->value->getNodes()->initializeCounter();
        GenericEntry<long int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
        
        json << "{" << latPrint << ":";
        json << boost::lexical_cast<std::string>(nextNode->value->getLat());
        json << "," << lonPrint << ":";
        json << boost::lexical_cast<std::string>(nextNode->value->getLon());
        json << "}";
        
        while(true)
        {
            nextNode = nextRoad->value->getNodes()->nextEntry();
            if(nextNode == NULL) { break; }
            
            if(nextNode->value->getEle() > -500)
            {
                alreadyAssignedElevation = true;
                break;
            }
            
            json << ",{" << latPrint << ":";
            json << boost::lexical_cast<std::string>(nextNode->value->getLat());
            json << "," << lonPrint << ":";
            json << boost::lexical_cast<std::string>(nextNode->value->getLon());
            json << "}";
        }
        delete(nextNode);
        
        if(alreadyAssignedElevation)
        {
            nextRoad = roads->nextEntry();
            continue;
        }
        
        // check start and end ints if they have elevation
        bool startIntHasElevation = nextRoad->value->getStartIntersection()->getElevation() > -500;
        bool endIntHasElevation = nextRoad->value->getEndIntersection()->getElevation() > -500;
        
        if(!startIntHasElevation)
        {
            json << ",{" << latPrint << ":";
            json << boost::lexical_cast<std::string>(nextRoad->value->getStartIntersection()->getLat());
            json << "," << lonPrint << ":";
            json << boost::lexical_cast<std::string>(nextRoad->value->getStartIntersection()->getLon());
            json << "}";
        }

        if(!endIntHasElevation)
        {
            json << ",{" << latPrint << ":";
            json << boost::lexical_cast<std::string>(nextRoad->value->getEndIntersection()->getLat());
            json << "," << lonPrint << ":";
            json << boost::lexical_cast<std::string>(nextRoad->value->getEndIntersection()->getLon());
            json << "}";
        }
        
        // make query
        json << "]}";
        std::string newCommand = command + json.str() + apiKey;
        this->queryFile(server, newCommand, elevFile);
        
        auto start = std::chrono::system_clock::now();
        while(1)
        {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end - start;
            
            if(diff.count() > 1.0) { break; }
        }
        
        // read query output
        ptree elevData;
        read_json(elevFile, elevData);
        BOOST_FOREACH(ptree::value_type &u, elevData)
        {
            std::string elevFeature = u.first.data();
            if(!elevFeature.compare("height"))
            {
                long int elevCount = 0;
                // iterate through heigts and add them to road
                BOOST_FOREACH(ptree::value_type &v, u.second)
                {
                    float elev = lexical_cast<float>(v.second.data());
                    
                    if(elevCount < nextRoad->value->getNodes()->getSize())
                    {
                        nextRoad->value->getNodes()->getEntry(elevCount)->setEle(elev);
                    }
                    else if(elevCount == nextRoad->value->getNodes()->getSize() && !startIntHasElevation)
                    {
                        nextRoad->value->getStartIntersection()->setElev(elev);
                    }
                    else if(elevCount == nextRoad->value->getNodes()->getSize() && startIntHasElevation && !endIntHasElevation)
                    {
                        nextRoad->value->getEndIntersection()->setElev(elev);
                    }
                    else if(elevCount == nextRoad->value->getNodes()->getSize() + 1 && !startIntHasElevation && !endIntHasElevation)
                    {
                        nextRoad->value->getEndIntersection()->setElev(elev);
                    }
                    
                    elevCount++;
                }
            }
        }

        nextRoad = roads->nextEntry();
    }
    delete(nextRoad);
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

// WARNING! map image is always overwritten
int DataCollection::pullOSMDataPNG(double lat, double lon) {

	std::cout << "Pulling Map Image" << std::endl;

	double latDeltaMin = 10;
	double lonDeltaMin = 10;
    int zoomIdx = this->zoomMin;

    for(int i = this->zoomMin; i <= this->zoomMax; i++)
    {
        std::pair<double, double>* latLonDeltas = this->zoomSpreads.getEntry(i);
        double currLatDelta = latLonDeltas->first;
        double currLonDelta = latLonDeltas->second;

        if(this->latDelta < currLatDelta && this->lonDelta < currLonDelta && this->latDelta < latDeltaMin && this->lonDelta < lonDeltaMin)
        {
            latDeltaMin = currLatDelta;
            lonDeltaMin = currLonDelta;
            zoomIdx = i;
        }
    }

	std::string webkit2png = "/usr/local/bin/webkit2png -W 6000 -H 6000 -o ";
    webkit2png += this->dataFolder + "/" + this->mapPNGName + " ";
    webkit2png += "https://www.openstreetmap.org/export#map=";
	webkit2png += lexical_cast<std::string>(zoomIdx) + "/";
	webkit2png += lexical_cast<std::string>(lat) + "/";
    webkit2png += lexical_cast<std::string>(lon);
    
    std::cout << "command used ..." << std::endl;
    std::cout << webkit2png << std::endl;
    
    system(webkit2png.c_str());

	this->boundsCountPNG++;
	Bounds* newBounds = new Bounds(lat+.5*latDeltaMin, lon+.5*lonDeltaMin, lat-.5*latDeltaMin, lon-.5*lonDeltaMin);
	newBounds->assignID(this->boundsCountPNG);
	this->boundsMapPNG.addEntry(this->boundsCountPNG, newBounds);

	return zoomIdx;
}

void DataCollection::setZoomSpreads() {
	// spreads for zoom index 19
	double latSpread = 0.01075;
	double lonSpread = 0.01515;

	for(int i = this->zoomMax; i >= this->zoomMin; i--)
	{
		this->zoomSpreads.addEntry(i, new std::pair<double, double>(latSpread, lonSpread));
		latSpread *= 2.0;
		lonSpread *= 2.0;
	}
}

std::string DataCollection::getDataFolder() {
	return this->dataFolder;
}

GenericMap<int, Bounds*>* DataCollection::getBoundsMapPNG() {
	return &this->boundsMapPNG;
}

std::string DataCollection::getMapPNGName() {
	return this->mapPNGName;
}

// lat lon increase L->R and B->T
int DataCollection::getElevation(double lat, double lon) {

	double nextLat, nextLon, latEleDiff, lonEleDiff, latScalar, lonScalar;

	for(int k = this->numEleLatsSRTM; k >= 0; k--)
	{
		nextLat = this->eleLowerLeftLat + (this->numEleLatsSRTM-k+1)*this->eleCellSize;
		if(lat < nextLat)
		{
			for(int l = 0; l < this->numEleLonsSRTM; l++)
			{
				nextLon = this->eleLowerLeftLon + (l+1)*this->eleCellSize;
				if(lon < nextLon)
				{
					int elevation = this->eleDataSRTM[l][k];

					if(elevation == this->voidEle)
					{
						std::cout << "------- void elevation -------" << std::endl;
						return elevation;
					} else if((k < this->numEleLonsSRTM-1) && (l < this->numEleLatsSRTM-1))
					{
						latEleDiff = this->eleDataSRTM[l][k] - this->eleDataSRTM[l+1][k];
						lonEleDiff = this->eleDataSRTM[l][k] - this->eleDataSRTM[l][k+1];

						latScalar = (nextLat - lat) / this->eleCellSize;
						lonScalar = (nextLon - lon) / this->eleCellSize;
						assert(latScalar >= 0 && lonScalar >= 0);
						assert(latScalar <= 1 && lonScalar <= 1);

						latEleDiff *= latScalar;
						lonEleDiff *= lonScalar;

						elevation += (latEleDiff + lonEleDiff);
					}
					return elevation;
				}
			}
		}
	}
	return -1;
}

GenericMap<long int, Node*>* DataCollection::getNodeMap() {
	return &this->nodeMap;
}

GenericMap<int, Way*>* DataCollection::getWayMap() {
	return &this->wayMap;
}

// for loading into http://www.gpsvisualizer.com/
GenericMap<long int, Road*>* DataCollection::makeRawRoads() {
    
    GPS converter;

	std::cout << "making raw roads" << std::endl;

	std::string csvName = this->dataFolder + "/" + "rawMapData.csv";
	GenericMap<long int, Road*>* rawRoads = new GenericMap<long int, Road*>();

	// delete existing csv if found
	std::string rm = "rm " + csvName;
	system(rm.c_str());

	// create new csv
    FILE* csv;
    csv = std::fopen(csvName.c_str(), "w");
    
	// add header to csv
	fprintf(csv, "name, description, color, latitude, longitude\n");

	// begin iterating through ways
	this->wayMap.initializeCounter();
	GenericEntry<int, Way*>* nextWay = this->wayMap.nextEntry();
	while(nextWay != NULL)
	{
		Way* way = nextWay->value;

		// grab nodes from way
		GenericMap<long int, Node*>* nodes = new GenericMap<long int, Node*>();

		// iterate through node IDs of way
        long int node_count = 0;
		way->getNodeIDs()->initializeCounter();
		GenericEntry<int, long int>* nextWayNodeID = way->getNodeIDs()->nextEntry();
		while(nextWayNodeID != NULL)
		{
			Node* node = this->nodeMap.getEntry(nextWayNodeID->value);
			if(node != NULL)
			{
				nodes->addEntry(node_count, node);
                node_count++;
            }
			nextWayNodeID = way->getNodeIDs()->nextEntry();
		}
		delete(nextWayNodeID);

		// make a raw road if enough nodes exist
		if(nodes->getSize() > 2)
		{
			Road* newRoad = new Road(way->getWayType(), way->getID(), nodes);
			
			try { // potential source of really weird run-time errors
                
                // acquire data for curve fitting
                int latLonCount = 0;
                
                // for splines
				Eigen::MatrixXd points(2, nodes->getSize());

                std::cout << "******** node control points ********" << std::endl;
                
                // set control points for spline
				nodes->initializeCounter();
				GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
				while(nextNode != NULL)
				{
                    double lat = nextNode->value->getLat();
                    double lon = nextNode->value->getLon();
                    
                    // print control points
                    printf("%.6f,%.6f\n", lat, lon);
                    
                    // for splines
                    points(0, latLonCount) = lat;
                    points(1, latLonCount) = lon;
                    
					nextNode = nodes->nextEntry();
					latLonCount++;
				}
				delete(nextNode);

				// fit first order spline spline
				typedef Eigen::Spline<double, 2> spline2f;

                // fit spline
                std::cout << "******** 1st order spline " << "********" << std::endl;
                spline2f rawRoadSpline = Eigen::SplineFitting<spline2f>::Interpolate(points, 1);
                
                // evaluate spline and save points to csv for viewing
                Eigen::Spline<double,2>::PointType pt = rawRoadSpline(0);
                double prev_lat = pt(0,0);
                double prev_lon = pt(1,0);
                float dist = 0.0;
                
                // get distance of spline to set evaluation distance correctly
                for(double u = 0; u <= 1; u += 0.025)
                {
                    pt = rawRoadSpline(u);
                    double curr_lat = pt(0,0);
                    double curr_lon = pt(1,0);
                    
                    // get distance of eval point along spline
                    dist += converter.deltaLatLonToXY(prev_lat, prev_lon, curr_lat, curr_lon);
                    
                    prev_lat = curr_lat;
                    prev_lon = curr_lon;
                }
                
                double evalStepSize = 5.0 / dist;
                pt = rawRoadSpline(0);
                prev_lat = pt(0,0);
                prev_lon = pt(1,0);
                dist = 0.0;
                
                for(double u = 0; u <= 1; u += evalStepSize)
                {
                    pt = rawRoadSpline(u);
                    double curr_lat = pt(0,0);
                    double curr_lon = pt(1,0);
                    
                    // to console
                    printf("%.6f,%.6f\n", curr_lat, curr_lon);
                    
                    // get distance of eval point along spline
                    dist += converter.deltaLatLonToXY(prev_lat, prev_lon, curr_lat, curr_lon);
                    
                    // to csv
                    fprintf(csv, "%ld,", way->getID());
                    fprintf(csv, "Way ID: %ld | ", way->getID());
                    fprintf(csv, "Way Type: %s | ", way->getWayType().c_str());
                    fprintf(csv, "Way Speed: %d | ", way->getWaySpeed());
                    fprintf(csv, "Lat & Lon: %.12f %.12f | ", curr_lat, curr_lon);
                    fprintf(csv, "Distance: %.2f,", dist);
                    fprintf(csv, "red,");
                    fprintf(csv, "%.12f,%.12f\n", curr_lat, curr_lon);
                    
                    prev_lat = curr_lat;
                    prev_lon = curr_lon;
                }

                newRoad->assignSplineLength(dist);
				newRoad->assignSpline(rawRoadSpline);
                newRoad->setMinMaxLatLon();

			} catch(const std::exception& e) {
				std::cout << e.what() << std::endl;
			}

			rawRoads->addEntry(way->getID(), newRoad);
		}
		nextWay = this->wayMap.nextEntry();
	}
	delete(nextWay);

    fclose(csv);
	return rawRoads;
}

int DataCollection::getVoidEle() {
	return this->voidEle;
}

GenericMap<int, Bounds*>* DataCollection::getBoundsMapXML() {
	return &this->boundsMapXML;
}

}


