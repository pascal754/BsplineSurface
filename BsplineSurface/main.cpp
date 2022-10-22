// October 14, 2022
// by SM

import <iostream>;
import <vector>;
import <format>;
import <stdexcept>;
import <array>;
import <cmath>;
import <numbers>;

#include <GL/glut.h>

import Point3d;
import BsplineSurface;

const double LENGTH{ 150.0 };
BsplineSurface bs00{ 3, 3 };
double tractionAngle{};
bool leftMouseButtonPressed{};
bool rightMouseButtonPressed{};
int lastX{}, lastY{};
double zoomScale{ LENGTH };
double aspectRatio{ 1.0 };
double xTranslation{}, yTranslation{};
double sNear{ -LENGTH }, sFar{ LENGTH };
std::array<double, 3> currentVec{}, prevVec{}, rotationAxis{};
GLdouble mxTransform[4][4]{ {-0.7071, -0.5, 0.5, 0.0}, {0.7071, -0.5, 0.5, 0.0}, {0.0, 0.7071, 0.7071, 0.0}, {0.0, 0.0, 0.0, 1.0} }; // isometric view
const double oneOverSquareRoot2{ 1.0 / sqrt(2.0) };

void ptTo3DVec(int x, int y, std::array<double, 3>& vec)
{
	// x^2 + y^2 + z^2 == r^2, (r == 1)

	int w{ glutGet(GLUT_WINDOW_WIDTH) };
	int h{ glutGet(GLUT_WINDOW_HEIGHT) };
	//std::cout << std::format("width: {}, heigh: {}\n", w, h);

	vec[0] = 2.0 * x / w - 1.0;
	vec[1] = -2.0 * y / h + 1.0;
	double hypot{ std::hypot(vec[0], vec[1])};

	if (hypot <= oneOverSquareRoot2) // x^2 + y^2 <= r^2 / 2
	{
		vec[2] = sqrt(1.0 - hypot * hypot); // z == sqrt(r^2 - (x^2 + y^2))
	}
	else
	{
		vec[2] = 0.5 / hypot; // z == (r^2 / 2) / sqrt(x^2 + y^2)
	}

	hypot = std::hypot(vec[0], vec[1], vec[2]);

	vec[0] /= hypot;
	vec[1] /= hypot;
	vec[2] /= hypot;

	//std::cout << std::format("vector: {}, {}, {}\n", vec[0], vec[1], vec[2]);
}

void onKeyStroke(unsigned char key, int x, int y)
{
	if (key == 'r' || key == 'R')
	{
		zoomScale = LENGTH;
		mxTransform[0][0] = -0.7071;
		mxTransform[0][1] = -0.5;
		mxTransform[0][2] = 0.5;
		mxTransform[0][3] = 0.0;
		mxTransform[1][0] = 0.7071;
		mxTransform[1][1] = -0.5;
		mxTransform[1][2] = 0.5;
		mxTransform[1][3] = 0.0;
		mxTransform[2][0] = 0.0;
		mxTransform[2][1] = 0.7071;
		mxTransform[2][2] = 0.7071;
		mxTransform[2][3] = 0.0;
		mxTransform[3][0] = 0.0;
		mxTransform[3][1] = 0.0;
		mxTransform[3][2] = 0.0;
		mxTransform[3][3] = 1.0;
		xTranslation = yTranslation = 0.0;
		glutPostRedisplay();
	}
}
void onMouseButton(int button, int state, int x, int y)
{
	//std::cout << std::format("button: {}, state: {}, x: {}, y: {}\n", button, state, x, y);
	if (button == 0 && state == 0) // left mouse button pressed
	{
		leftMouseButtonPressed = true;
		ptTo3DVec(x, y, prevVec);
	}
	else if (button == 0 && state == 1) // left mouse button released
	{
		leftMouseButtonPressed = false;
	}
	else if (button == 2 && state == 0) // right mouse button pressed
	{
		rightMouseButtonPressed = true;
		lastX = x;
		lastY = y;
	}
	else if (button == 2 && state == 1) // right mouse button released
	{
		rightMouseButtonPressed = false;
	}
	else if (button == 3 && state == 0) // scroll forward
	{
		//std::cout << "scroll forward\n";
		zoomScale *= 0.9;
		glutPostRedisplay();
	}
	else if (button == 4 && state == 0) // scroll backward
	{
		//std::cout << "scroll backward\n";
		zoomScale *= 1.1;
		glutPostRedisplay();
	}
}

void onMouseDrag(int x, int y)
{
	if (leftMouseButtonPressed)
	{
		ptTo3DVec(x, y, currentVec);
		//std::cout << std::format("x: {}, y: {}\n", x, y);
		
		double innerProduct{ currentVec[0] * prevVec[0] + currentVec[1] * prevVec[1] + currentVec[2] * prevVec[2] };
		innerProduct = std::min(innerProduct, 1.0);
		tractionAngle = 180 * std::acos(innerProduct) / std::numbers::pi; // in degree
		//std::cout << std::format("angle: {}\n", tractionAngle);

		rotationAxis[0] = prevVec[1] * currentVec[2] - prevVec[2] * currentVec[1];
		rotationAxis[1] = prevVec[2] * currentVec[0] - prevVec[0] * currentVec[2];
		rotationAxis[2] = prevVec[0] * currentVec[1] - prevVec[1] * currentVec[0];

		//std::cout << std::format("axis: {}, {}, {}\n", rotationAxis[0], rotationAxis[1], rotationAxis[2]);

		prevVec = currentVec;

		glutPostRedisplay();
	}
	else if (rightMouseButtonPressed)
	{
		xTranslation += static_cast<double>(lastX - x) * 2.0 * zoomScale / glutGet(GLUT_WINDOW_WIDTH);
		yTranslation += static_cast<double>(y - lastY) * 2.0 * zoomScale / glutGet(GLUT_WINDOW_HEIGHT);
		//std::cout << std::format("xTranlation: {}, yTranslation: {}\n", xTranslation, yTranslation);
		lastX = x;
		lastY = y;
		glutPostRedisplay();
	}
}

void reshape(int x, int y)
{
	aspectRatio =  static_cast<double>(y) / x; // the inverse of aspect ratio
	glViewport(0, 0, x, y);
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double w = glutGet(GLUT_WINDOW_WIDTH);
	double h = glutGet(GLUT_WINDOW_HEIGHT);
	//gluPerspective(60.0, w / h, 0.1, 1000.0);
	

	glOrtho(-zoomScale + xTranslation, zoomScale + xTranslation, -zoomScale * aspectRatio + yTranslation, zoomScale * aspectRatio + yTranslation, sNear, sFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(100, 100, 100, 0, 0, 0, 0, 0, 1);

	if (leftMouseButtonPressed)
	{
		glPushMatrix();

			glLoadIdentity();
			glRotated(tractionAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
			glMultMatrixd(&mxTransform[0][0]);
			glGetDoublev(GL_MODELVIEW_MATRIX, &mxTransform[0][0]);

		glPopMatrix();
	}

	glMultMatrixd(&mxTransform[0][0]);

	glBegin(GL_LINES);

		// x-axis
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(100.0, 0.0, 0.0);
	
		// y-axis
		glColor3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 100.0, 0.0);

		// z-axis
		glColor3d(0.0, 0.0, 1.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 100.0);
	glEnd();

	double tx{ 100.0 * mxTransform[0][0] + mxTransform[3][0] };
	double ty{ 100.0 * mxTransform[0][1] + mxTransform[3][1] };
	double tz{ 100.0 * mxTransform[0][2] + mxTransform[3][2] };

	glColor3d(1.0, 0.0, 0.0);

	glPushMatrix();
		glLoadIdentity();
		glTranslated(tx, ty, tz);
		glScaled(0.1, 0.1, 0.1);
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'x');
	glPopMatrix();

	tx = 100.0 * mxTransform[1][0] + mxTransform[3][0];
	ty = 100.0 * mxTransform[1][1] + mxTransform[3][1];
	tz = 100.0 * mxTransform[1][2] + mxTransform[3][2];

	glColor3d(0.0, 1.0, 0.0);

	glPushMatrix();
		glLoadIdentity();
		glTranslated(tx, ty, tz);
		glScaled(0.1, 0.1, 0.1);
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'y');
	glPopMatrix();

	glColor3d(0.0, 0.0, 1.0);

	tx = 100.0 * mxTransform[2][0] + mxTransform[3][0];
	ty = 100.0 * mxTransform[2][1] + mxTransform[3][1];
	tz = 100.0 * mxTransform[2][2] + mxTransform[3][2];

	glPushMatrix();
		glLoadIdentity();
		glTranslated(tx, ty, tz);
		glScaled(0.1, 0.1, 0.1);
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'z');
	glPopMatrix();

	glColor3d(1.0, 1.0, 1.0);
	Point3d pt{};
	for (double u{}; u <= 1.0; u += 0.05)
	{
		glBegin(GL_LINE_STRIP); // glBegin(GL_POINTS);
			for (double v{}; v <= 1.0; v += 0.05)
			{
				bs00.surfacePoint(u, v, pt);
				//std::cout << std::format("u: {:15.5f}, v: {:15.5f}, ({:15.5f}, {:15.5f}, {:15.5f})\n", u, v, pt.x, pt.y, pt.z);
				glVertex3d(pt.x, pt.y, pt.z);
			}
		glEnd();
	}

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	try
	{
		std::vector<Point3d> vp0{ {50, -30, 10}, {20, -30, 10}, {0, -30, 30}, {-20, -30, 30} };
		std::vector<Point3d> vp1{ {50, 10, 10}, {20, 10, 10}, {0, 10, 30}, {-20, 10, 30} };
		std::vector<Point3d> vp2{ {50, 30, 0}, {20, 30, 0}, {0, 30, 10}, {-20, 30, 10} };
		std::vector<Point3d> vp3{ {50, 50, 0 }, { 20, 50, 0 }, {0, 50, 10}, {-20, 50, 10} };
		std::vector<Point3d> vp4{ {50, 60, 0}, {20, 60, 0}, {0, 60, 10}, {-20, 60, 10} };

		bs00.addVector(vp0);
		bs00.addVector(vp1);
		bs00.addVector(vp2);
		bs00.addVector(vp3);
		bs00.addVector(vp4);
		bs00.makeKnots();

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(600, 600);
		glutCreateWindow("B-spline Surface");
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutMouseFunc(onMouseButton);
		glutMotionFunc(onMouseDrag);
		glutKeyboardFunc(onKeyStroke);
		glutMainLoop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	catch (...)
	{
		std::cerr << "something wrong\n";
		return 1;
	}
}
