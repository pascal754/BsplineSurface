import <iostream>;
import <vector>;
import <format>;
import <stdexcept>;
import <array>;
import <cmath>;
import <numbers>;

import Point3d;
import BsplineSurface;

#include <GL/glut.h>

BsplineSurface bs00{ 3, 3 };
double tractionAngle{};
bool leftMouseButtonPressed{};
int lastX{}, lastY{};
double dx{}, dy{}, dz{};
std::array<double, 3> currentVec, prevVec, rotationAxis;
GLdouble mxTransform[4][4]{ {1.0, 0.0, 0.0, 0.0},{0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0} };

void ptTo3DVec(int x, int y, std::array<double, 3>& vec)
{
	double dis{}, a{};
	std::array<int, 4> viewport;
	glGetIntegerv(GL_VIEWPORT, viewport.data());

	vec[0] = (2.0 * x - viewport[2]) / viewport[2];
	vec[1] = (viewport[3] - 2.0 * y) / viewport[3];

	dis = std::hypot(vec[0], vec[1]);
	vec[2] = std::cos((std::numbers::pi / 2.0) * ((dis < 1.0) ? dis : 1.0));
	a = std::hypot(vec[0], vec[1], vec[2]);

	vec[0] /= a;
	vec[1] /= a;
	vec[2] /= a;
	std::cout << std::format("viewport: {}, {}, {}, {}\n", viewport[0], viewport[1], viewport[2], viewport[3]);
	std::cout << std::format("vector: {}, {}, {}\n", vec[0], vec[0], vec[0]);
}

void onMouseButton(int button, int state, int x, int y)
{
	std::cout << std::format("button: {}, state: {}, x: {}, y: {}\n", button, state, x, y);
	if (button == 0 && state == 0) // left mouse button pressed
	{
		leftMouseButtonPressed = true;
		ptTo3DVec(x, y, prevVec);
	}
	else
	{
		leftMouseButtonPressed = false;
	}
}

void onMouseDrag(int x, int y)
{
	if (leftMouseButtonPressed)
	{
		ptTo3DVec(x, y, currentVec);
		//std::cout << std::format("x: {}, y: {}\n", x, y);
		dx = currentVec[0] - prevVec[0];
		dy = currentVec[1] - prevVec[1];
		dz = currentVec[2] - prevVec[2];

		tractionAngle = 45.0 * std::hypot(dx, dy, dz);
		std::cout << std::format("angle: {}, dx: {}, dy: {}, dz: {}\n", tractionAngle, dx, dy, dz);

		rotationAxis[0] = prevVec[1] * currentVec[2] - prevVec[2] * currentVec[1];
		rotationAxis[1] = prevVec[2] * currentVec[0] - prevVec[0] * currentVec[2];
		rotationAxis[2] = prevVec[0] * currentVec[1] - prevVec[1] * currentVec[0];

		std::cout << std::format("axis: {}, {}, {}\n", rotationAxis[0], rotationAxis[1], rotationAxis[2]);

		prevVec = currentVec;

		glutPostRedisplay();
	}
}

//void timer(int value)
//{
//	//angle += 1.0;
//	glutTimerFunc(16, timer, 0);
//	glutPostRedisplay();
//}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double w = glutGet(GLUT_WINDOW_WIDTH);
	double h = glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(60.0, w / h, 0.1, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(100, 100, 100, 0, 0, 0, 0, 0, 1);

	if (leftMouseButtonPressed)
	{
		glPushMatrix();

		glLoadIdentity();
		glRotated(tractionAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
		glMultMatrixd((GLdouble*)mxTransform);
		glGetDoublev(GL_MODELVIEW_MATRIX, (GLdouble*)mxTransform);

		glPopMatrix();
	}

	glMultMatrixd((GLdouble*)mxTransform);

	//srand(0);
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

	glColor3d(1.0, 1.0, 1.0);
	Point3d pt{};
	for (double u{}; u <= 1.0; u += 0.01)
	{
		glBegin(GL_LINE_STRIP); // glBegin(GL_POINTS);
		for (double v{}; v <= 1.0; v += 0.01)
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

		/*mxTransform[0][0] = -0.7071;		mxTransform[0][1] = -0.5;
		mxTransform[0][2] = 0.5;			mxTransform[0][3] = 0.0;

		mxTransform[1][0] = 0.7071;		mxTransform[1][1] = -0.5;
		mxTransform[1][2] = 0.5;			mxTransform[1][3] = 0.0;

		mxTransform[2][0] = 0.0;			mxTransform[2][1] = 0.7071;
		mxTransform[2][2] = 0.7071;		mxTransform[2][3] = 0.0;

		mxTransform[3][0] = 0.0;			mxTransform[3][1] = 0.0;
		mxTransform[3][2] = 0.0;			mxTransform[3][3] = 1.0;*/


		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(640, 480);
		glutCreateWindow("B-spline Surface");
		glutDisplayFunc(display);
		glutMouseFunc(onMouseButton);
		glutMotionFunc(onMouseDrag);
		//glutTimerFunc(0, timer, 0);
		//glLoadIdentity();
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
