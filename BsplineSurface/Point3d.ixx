// October 14, 2022
// by SM

export module Point3d;

export struct Point3d {
	Point3d() = default;
	Point3d(double xCoord, double yCoord, double zCoord) : x{ xCoord }, y{ yCoord }, z{ zCoord } {}
	Point3d(const Point3d& pt) = default;
	double x{}, y{}, z{};
};
