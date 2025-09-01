#define GL_SILENCE_DEPRECATION

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION // required to generate stb_image function definitions
#include "lib/stb_image.h"

GLuint texture;
struct Vertex
{
    float x, y, z;
};
struct Normal
{
    float nx, ny, nz;
};
struct TexCoord
{
    float u, v;
};
std::vector<TexCoord> texcoords;

struct FaceIndex
{
    int vIdx;
    int tIdx; // texture index
    int nIdx;
};

std::vector<Vertex> vertices;
std::vector<Normal> normals;
std::vector<std::vector<FaceIndex>> faces;

GLuint loadTexture(const char *filename)
{
    int w, h, n;
    unsigned char *data = stbi_load(filename, &w, &h, &n, 0);
    if (!data)
    {
        std::cerr << "Texture load failed\n";
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum format = (n == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

void loadOBJ(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open OBJ file: " << filename << "\n";
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
        else if (type == "vt")
        {
            TexCoord t;
            ss >> t.u >> t.v;
            texcoords.push_back(t);
        }
        else if (type == "f")
        {
            std::vector<FaceIndex> face;
            std::string token;
            while (ss >> token)
            {
                int vi = -1, ti = -1, ni = -1;

                if (sscanf(token.c_str(), "%d/%d/%d", &vi, &ti, &ni) == 3)
                    face.push_back({vi - 1, ti - 1, ni - 1});
                else if (sscanf(token.c_str(), "%d//%d", &vi, &ni) == 2)
                    face.push_back({vi - 1, -1, ni - 1});
                else if (sscanf(token.c_str(), "%d/%d", &vi, &ti) == 2)
                    face.push_back({vi - 1, ti - 1, -1});
                else if (sscanf(token.c_str(), "%d", &vi) == 1)
                    face.push_back({vi - 1, -1, -1});
            }
            faces.push_back(face);
        }
    }

    file.close();
    std::cout << "Loaded OBJ: " << vertices.size() << " vertices, "
              << texcoords.size() << " UVs, "
              << normals.size() << " normals, "
              << faces.size() << " faces.\n";
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPushMatrix();
    glTranslatef(0, -0.5f, -5);
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
            if (idx.tIdx >= 0)
            {
                TexCoord t = texcoords[idx.tIdx];
                glTexCoord2f(t.u, t.v);
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_TEXTURE_2D);

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
    glutCreateWindow("Textured Suzanne");

    loadOBJ("assets/monkey.obj");
    texture = loadTexture("assets/texture.jpeg");
    initGL();

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
