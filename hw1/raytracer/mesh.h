#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include "axisaligbounbox.h"
#include <limits>
#include "helpers.h"

class Mesh : public Hittable
{
public:
    Mesh(const std::vector<point3> &vertices, const std::vector<int> &indices,
         const std::string &material_id)
        : m_vertices{vertices}, m_indices{indices}, mat_id{material_id}
    {
        boundingBoxInit();
    };
    bool hit(const ray &r, const double &t_min, const double &t_max,
             HitRecord &rec) const override
    {
        if (!m_boundingBox.hit(r, t_min, t_max))
            return false;

        rec.t = INF;
        double t = -1;
        bool ret = false;

        for (size_t j = 0; j < m_indices.size() - 2; j += 3)
        {
            int i0 = m_indices[j] - 1;
            int i1 = m_indices[j + 1] - 1;
            int i2 = m_indices[j + 2] - 1;
            if (intersect(m_vertices[i0], m_vertices[i1], m_vertices[i2], r, t_min,
                          t_max, t) &&
                rec.t > t)
            {
                rec.t = t;
                rec.normal = cross(m_vertices[i1] - m_vertices[i0],
                                   m_vertices[i2] - m_vertices[i0]);
                rec.mat_id = mat_id;
                ret = true;
            }
        }
        return ret;
    }

    bool boundingBoxInit() override
    {
        point3 min_point = point3(INF, INF, INF);
        point3 max_point = point3(-INF, -INF, -INF);

        for (const auto &v : m_vertices)
        {
            min_point.x = fmin(min_point.x, v.x);
            min_point.y = fmin(min_point.y, v.y);
            min_point.z = fmin(min_point.z, v.z);
            max_point.x = fmax(max_point.x, v.x);
            max_point.y = fmax(max_point.y, v.y);
            max_point.z = fmax(max_point.z, v.z);
        }
        m_boundingBox = AxisAlignedBoundingBox(point3(min_point.x, min_point.y, min_point.z),
                                               point3(max_point.x, max_point.y, max_point.z));
        return true;
    }

private:
    const std::vector<point3> &m_vertices;
    std::vector<int> m_indices;
    std::string mat_id;
    AxisAlignedBoundingBox m_boundingBox;
};

#endif // MESH_H