#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Vertex
{
    float x, y, z;
};

struct Normal
{
    float nx, ny, nz;
};

std::vector<Vertex> vertices;
std::vector<Normal> normals;

struct FaceIndex
{
    int vIdx; // vertex index
    int nIdx; // normal index
};

std::vector<std::vector<FaceIndex>> faces;

void loadOBJ(const char *filename)
{
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            Vertex v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (type == "vn")
        {
            Normal n;
            ss >> n.nx >> n.ny >> n.nz;
            normals.push_back(n);
        }
        else if (type == "f")
        {
            std::vector<FaceIndex> face;
            std::string token;
            while (ss >> token)
            {
                int v, t, n;
                // format: v//n or v/t/n
                if (sscanf(token.c_str(), "%d//%d", &v, &n) == 2)
                {
                    face.push_back({v - 1, n - 1});
                }
                else if (sscanf(token.c_str(), "%d/%d/%d", &v, &t, &n) == 3)
                {
                    face.push_back({v - 1, n - 1});
                }
                else
                {
                    sscanf(token.c_str(), "%d", &v);
                    face.push_back({v - 1, -1});
                }
            }
            faces.push_back(face);
        }
    }
}

void display()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    GLfloat light_pos[] = {0.0f, 0.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glPushMatrix();
    glTranslatef(0, 0, -5);
    glRotatef(30, 1, 0, 0);
    glRotatef(30, 0, 1, 0);

    glBegin(GL_TRIANGLES);
    for (auto &face : faces)
    {
        for (auto &idx : face)
        {
            if (idx.nIdx >= 0)
            {
                Normal n = normals[idx.nIdx];
                glNormal3f(n.nx, n.ny, n.nz);
            }
            Vertex v = vertices[idx.vIdx];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();

    glPopMatrix();
    glutSwapBuffers();
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Monkey Model Loader");

    init();
    loadOBJ("assets/cube.obj");

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
