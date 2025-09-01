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
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy; // error term

    while (true)
    {
        putPixel(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(3.0f); // make points visible
    glColor3f(1.f, 1.f, 1.f);

    // Draw the requested segment:
    bresenhamLine(180, 15, 10, 145);

    glFlush();
}

void init()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set up a simple integer-coordinate 2D view
    gluOrtho2D(0, 200, 0, 200); // fits your points comfortably
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Bresenham Line (180,15) -> (10,145)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
