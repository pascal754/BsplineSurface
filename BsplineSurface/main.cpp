import <iostream>;
import <vector>;
import <format>;
import <stdexcept>;

import Point3d;
import BsplineSurface;

#include <GL/glut.h>

BsplineSurface bs00{ 3, 3 };
double angle{};;

void onMouseButton(int button, int state, int x, int y)
{
	std::cout << std::format("button: {}, state: {}, x: {}, y: {}\n", button, state, x, y);
	
}

void onMouseDrag(int x, int y)
{
	std::cout << std::format("x: {}, y: {}\n", x, y);
}

void timer(int value)
{
	angle += 1.0;
	glutTimerFunc(16, timer, 0);
	glutPostRedisplay();
}

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

	glRotated(angle, 0, 0, 1);

	srand(0);
	glBegin(GL_LINES);

	// x-axis
	glColor3d(1.0f, 0.0f, 0.0f);
	glVertex3d(0.0f, 0.0f, 0.0f);
	glVertex3d(100.0f, 0.0f, 0.0f);

	// y-axis
	glColor3d(0.0f, 1.0f, 0.0f);
	glVertex3d(0.0f, 0.0f, 0.0f);
	glVertex3d(0.0f, 100.0f, 0.0f);

	// z-axis
	glColor3d(0.0f, 0.0f, 1.0f);
	glVertex3d(0.0f, 0.0f, 0.0f);
	glVertex3d(0.0f, 0.0f, 100.0f);
	glEnd();

	glColor3d(1.0f, 1.0f, 1.0f);
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

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(640, 480);
		glutCreateWindow("B-spline Surface");
		glutDisplayFunc(display);
		glutMouseFunc(onMouseButton);
		glutMotionFunc(onMouseDrag);
		glutTimerFunc(0, timer, 0);
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
