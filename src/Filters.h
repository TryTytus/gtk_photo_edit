#pragma once

#include <vips/vips8>

struct FiltersState
{
    double contrast {};
    double sharpness {};
};

class Filters
{
public:
    
    Filters();

    void contrast(vips::VImage& image, const FiltersState& filters);
    void sharpness(vips::VImage& image, const FiltersState& filters);

    vips::VImage update(vips::VImage image, const FiltersState& filters);
};