#ifndef AXISALIGNEDBOUNDINGBOX_H
#define AXISALIGNEDBOUNDINGBOX_H

#include "hittable.h"
#include "vec3.h"

class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox() = default;

    AxisAlignedBoundingBox(const point3 &minPoint, const point3 &maxPoint)
        : m_minPoint{minPoint}, m_maxPoint{maxPoint}
    {
    }

    bool hit(const ray &r, const double &t_min,
             const double &t_max) const
    {
        double min_diff[3], max_diff[3];
        double min = t_min;
        double max = t_max;

        for (int a = 0; a < 3; a++)
        {
            min_diff[a] = (m_minPoint[a] - r.origin()[a]) / r.direction()[a];
            max_diff[a] = (m_maxPoint[a] - r.origin()[a]) / r.direction()[a];
        }
        for (int a = 0; a < 3; a++)
        {
            min = fmax(fmin(min_diff[a], max_diff[a]), t_min);
            max = fmin(fmax(min_diff[a], max_diff[a]), t_max);
            if (max <= min)
                return false;
        }
        return true;
    }

private:
    point3 m_minPoint;
    point3 m_maxPoint;
};

#endif // AXISALIGNEDBOUNDINGBOX_H