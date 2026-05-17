#include "Filters.h"

#include <cmath>

void Filters::contrast(vips::VImage& image, const FiltersState& filters)
{
    const double contrast_scale = std::lerp(1.0, 2.6, filters.contrast);
    if (std::abs(contrast_scale - 1.0) > 0.001) {
        image = image.linear(contrast_scale, 128.0 * (1.0 - contrast_scale));
    }
}

void Filters::sharpness(vips::VImage& image, const FiltersState& filters)
{
    const double sharpness_sigma = std::lerp(0.0, 3.0, filters.sharpness);
    if (sharpness_sigma > 0.001) {
        image = image.sharpen(vips::VImage::option()->set("sigma", sharpness_sigma));
    }
}

vips::VImage Filters::update(vips::VImage image, const FiltersState& filters)
{
    sharpness(image, filters);
    contrast(image, filters);

    return image;
}
