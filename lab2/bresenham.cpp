#include <algorithm>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <cstdlib>
#include <cmath>

void putPixel(int x, int y)
{
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void bresenhamLine(int x0, int y0, int x1, int y1)
{
    // Ensure drawing left → right
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    // Assumes slope 0 < m < 1
    int d = 2 * dy - dx; // decision parameter
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        putPixel(x, y);
        if (d > 0)
        {
            y++;
            d -= 2 * dx;
        }
        d += 2 * dy;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(3.0f);
    glColor3f(1.f, 1.f, 1.f);
    bresenhamLine(20, 30, 180, 120);

    glFlush();
}

void init()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 200, 0, 200); // simple 2D coordinate system
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Bresenham Line (0<m<1)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
