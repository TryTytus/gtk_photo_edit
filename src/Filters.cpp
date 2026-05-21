#include "Filters.h"
#include <cmath>
#include <vips/VImage8.h>


FiltersState* FiltersState::self_static_ = nullptr;
Filters* Filters::self_static_ = nullptr;

FiltersState& FiltersState::getInstance()
{
    if (self_static_ == nullptr)
    {
        self_static_ = new FiltersState();
    }

    return *self_static_;
}

Filters& Filters::getInstace()
{
    if (self_static_ == nullptr)
    {
        self_static_ = new Filters();
    }

    return *self_static_;
}


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

void Filters::sobel(vips::VImage& image, const FiltersState& filters)
{
    if (filters.sobel > 0.001) {
        const auto edges = image.sobel();
        image = image.linear(1.0 - filters.sobel, 0.0)
                    .add(edges.linear(filters.sobel, 0.0));
    }
}

void Filters::brightness(vips::VImage &image, const FiltersState &filters)
{
    if (filters.brightness > 0.01)
    {
        double offset = filters.brightness * 100.0;
        image = image.linear(1.0, offset);
    }
}

void Filters::exposure(vips::VImage &image, const FiltersState &filters)
{
    if (filters.exposure > 0.01)
    {
        double factor = std::pow(2.0, filters.exposure);
        image = image * factor;
    }
}

void Filters::blur(vips::VImage &image, const FiltersState &filters)
{
    if (filters.blur > 0.01)
    {
        double sigma = filters.blur * 10.0;
        image = image.gaussblur(sigma);
    }
}

void Filters::gamma(vips::VImage &image, const FiltersState &filters)
{
    double exponent = std::pow(2.0, -filters.gamma);
    image = image.gamma(vips::VImage::option()
        ->set("exponent", exponent));

}




vips::VImage Filters::update(vips::VImage image, const FiltersState& filters)
{
    sharpness(image, filters);
    contrast(image, filters);
    sobel(image, filters);
    brightness(image, filters);

    exposure(image, filters);
    blur(image, filters);
    gamma(image, filters);

    return image;
}
