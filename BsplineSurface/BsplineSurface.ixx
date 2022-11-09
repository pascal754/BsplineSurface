// October 14, 2022
// by SM

export module BsplineSurface;

import <vector>;
import Point3D;

export class BsplineSurface
{
public:
	BsplineSurface(int p, int q) : p_degree{ p }, q_degree{ q } { uBasis.resize(p + 1); vBasis.resize(q + 1); }
	void surfacePoint(double u, double v, Point3D& S);
	void addVector(const std::vector<Point3D>& vPt);
	void makeKnots();
private:
	void basisFuns(int span, double x, int degree, std::vector<double>& knotVector, std::vector<double>& basis);
	void makeKnots(int size, int degree, std::vector<double>& knotVector);
	int findKnotSpan(int size, int degree, double x, const std::vector<double>& knotVector) const;

	int p_degree{}; // degree p
	int q_degree{}; // degree q
	std::vector<std::vector<Point3D>> controlPoints; // n + 1 by m + 1 matrix
	std::vector<double> uKnots; // r == n + p + 1, p + 1 duplicates at each end
	std::vector<double> vKnots; // q == m + q + 1, q + 1 duplicates at each end
	std::vector<double> uBasis;
	std::vector<double> vBasis;
};
