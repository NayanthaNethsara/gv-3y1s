// Compile on macOS:
// clang++ smooth_suzanne.cpp -o smooth_suzanne -framework OpenGL -framework GLUT

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

struct Vertex
{
    float x, y, z;
};
struct Normal
{
    float nx, ny, nz;
};
struct FaceIndex
{
    int vIdx;
    int nIdx;
};

std::vector<Vertex> vertices;
std::vector<Normal> normals;
std::vector<std::vector<FaceIndex>> faces;

void loadOBJ(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open file\n";
        return;
    }

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
                int vi, ti, ni;
                if (sscanf(token.c_str(), "%d//%d", &vi, &ni) == 2)
                    face.push_back({vi - 1, ni - 1});
                else if (sscanf(token.c_str(), "%d/%d/%d", &vi, &ti, &ni) == 3)
                    face.push_back({vi - 1, ni - 1});
                else
                    sscanf(token.c_str(), "%d", &vi), face.push_back({vi - 1, -1});
            }
            faces.push_back(face);
        }
    }
    file.close();
}

void initLighting()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glDisable(GL_CULL_FACE);

    GLfloat lightPos[] = {0.0f, 5.0f, 5.0f, 1.0f};
    GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    GLfloat mat_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.5f, 0.3f, 1.0f}; // 🔸 orangish color
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 1, 1);

    glPushMatrix();
    glTranslatef(0, -0.5f, -5); // adjust view
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

void initGL()
{
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    initLighting();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Suzanne Smooth 3D");

    loadOBJ("assets/monkey.obj"); // your Blender-exported Suzanne OBJ
    initGL();

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
