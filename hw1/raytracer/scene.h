#pragma once

#include "camera.h"
#include "hittable.h"
#include "vec3.h"
#include <vector>
#include <string>

struct Material
{
    std::string id;
    color ambient, diffuse, specular, mirror_refl;
    double phong_exp{0};
};

struct Pointlight
{
    point3 position;
    color intensity;
};

struct Scene
{
    Camera camera;
    color background, ambient;
    std::vector<Pointlight> lights;
    std::vector<Material> materials;
    std::vector<Hittable *> hittables;
    std::vector<point3> vertices;

    Material get_material(std::string id) const
    {
        for (auto &mat : materials)
        {
            if (mat.id == id)
            {
                return mat;
            }
        }
        return Material();
    }

    bool hit(const ray &r, const double t_min, const double t_max,
             HitRecord &rec) const
    {
        HitRecord temp;
        rec.t = t_max;
        bool is_hit = false;
        for (auto &o : hittables)
        {
            if (o->hit(r, t_min, t_max, temp) && rec.t >= temp.t)
            {
                rec = temp;
                is_hit = true;
            }
        }

        return is_hit;
    }
};