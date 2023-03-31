#ifndef IMAGE_H
#define IMAGE_H

#include "vec3.h"

class Image
{
public:
  Image(const int width, const int height) : m_width{width}, m_height{height}
  {
    data = new color[width * height];
  }

  ~Image()
  {
    delete[] data;
  }

  void set_pixel(const int i, const int j, const color &c)
  {
    if (i < 0 || i > m_width || j < 0 || j > m_height)
      return;

    data[j * m_width + i] = c;
  }

  color get_pixel(const int i, const int j) const
  {
    if (i < 0 || i > m_width || j < 0 || j > m_height)
      return color(0, 0, 0);

    return data[j * m_width + i];
  }
  void export_ppm(std::ostream &out) const
  {
    out << "P3\n"
        << m_width << " " << m_height << "\n255\n";
    for (int j = 0; j < m_height; ++j)
    {
      for (int i = 0; i < m_width; ++i)
      {
        out << clamp(data[j * m_width + i].x) << ' '
            << clamp(data[j * m_width + i].y) << ' '
            << clamp(data[j * m_width + i].z) << '\n';
      }
    }
  }

private:
  int m_width, m_height;
  color *data;
};

#endif // IMAGE_H