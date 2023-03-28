#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "xml_parser/pugixml.hpp"

class Vector4
{
    float l;
    float r;
    float b;
    float t;
};

class Vector3
{
    float x;
    float y;
    float z;
};

// color which is a vector3 with using
using Color = Vector3;

class Vector2
{
    float nx;
    float ny;
};

class Camera
{
    std::string id;
    Vector3 position;
    Vector3 gaze;
    Vector3 up;
    Vector4 nearPlane;
    float nearDistance;
    Vector2 image_resolution;
    std::string image_name;
};

enum LightType
{
    AMBIENT,
    POINT,
};

class Light
{
    LightType type;
};

// 2 light class which are ambient and point
class AmbientLight : public Light
{
    Vector3 color;
};

class PointLight : public Light
{
    std::string id;
    Vector3 position;
    Vector3 intensity;
};

class Material
{
    std::string id;
    Vector3 ambient_reflectance;
    Vector3 diffuse_reflectance;
    Vector3 specular_reflectance;
    Vector3 mirror_reflectance;
    int phong_exponent;
};

class VertexData
{
    std::vector<Vector3> vertices;
};

enum ObjectType
{
    SPHERE,
    TRIANGLE,
    MESH,
};

// three types of objects which are sphere, triangle and mesh

class Object
{
    ObjectType type;
};

class Sphere : public Object
{
};

class Scene
{
    int maxRayTraceDepth;
    Color backgroundColor;
    std::vector<Camera> cameras;
    std::vector<Light> lights;
    std::vector<Material> materials;
    std::vector<VertexData> vertices;
    std::vector<Object> objects;
};

// traverse all the nodes recursively and save the string data
// give function as third parameter
void traverse(pugi::xml_node node, std::vector<std::string> &data)
{
    for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
    {
        if (child.value() != "")
        {
            data.push_back(child.name());
        }
        traverse(child, data);
    }
}

int main()
{
    pugi::xml_document doc;
    if (!doc.load_file("scene.xml"))
        return -1;

    pugi::xml_node scene = doc.child("Scene");

    std::vector<std::string> data;
    traverse(scene, data);

    for (std::string &d : data)
        std::cout << d << std::endl;

    // traverse all the nodes

    return 0;
}