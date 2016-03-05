namespace DriverPrediction {

class Route {
private:
	Link links[];
	Goal goal;
	int counter;
public:
	Route(Link links[], Goal &goal);
	void addlink(Link &link);
	bool isequal(Route &other);
	Route copy();
	void initiateiterator();
	Link nextlink();
};

}
