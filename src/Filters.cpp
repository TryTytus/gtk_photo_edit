#include "Filters.h"
#include <cmath>


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
    if (filters.brightness > 0.01 || filters.brightness < -0.01)
    {
        double offset = filters.brightness * 100.0;
        image = image.linear(1.0, offset);
    }
}

void Filters::exposure(vips::VImage &image, const FiltersState &filters)
{
    if (filters.exposure > 0.01 || filters.exposure < -0.01)
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

    saturation(image, filters);
    warmth(image, filters);
    tint(image, filters);
    highlight(image, filters);
    shadows(image, filters);
    white_black(image, filters);
    vibrance(image, filters);
    vignette(image, filters);
    clarity(image, filters);
    noise(image, filters);

    return image;
}


void Filters::saturation(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.saturation) > 0.01) {
        auto b = image.bandsplit();

        auto gray = b[0] * 0.299 + b[1] * 0.587 + b[2] * 0.114;
        auto gray3 = gray.bandjoin(gray).bandjoin(gray);

        double s = filters.saturation;

        double factor = s > 0.0
            ? std::lerp(1.0, 2.3, s)
            : std::lerp(1.0, 0.0, -s);

        image = gray3 + (image - gray3) * factor;
    }
}

void Filters::warmth(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.warmth) > 0.01) {
        double t = filters.warmth;

        image = image.linear(
            std::vector<double>{
                1.0 + 0.15 * t,
                1.0,
                1.0 - 0.15 * t
            },
            std::vector<double>{
                0.0,
                0.0,
                0.0
            }
        );
    }
}

void Filters::tint(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.tint) > 0.01) {
        double t = filters.tint;

        image = image.linear(
            std::vector<double>{
                1.0 + 0.08 * t,
                1.0 - 0.12 * t,
                1.0 + 0.08 * t
            },
            std::vector<double>{
                0.0,
                0.0,
                0.0
            }
        );
    }
}

void Filters::highlight(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.highlight) > 0.01) {
        auto b = image.bandsplit();

        auto gray = b[0] * 0.299 + b[1] * 0.587 + b[2] * 0.114;

        auto mask = ((gray - 128.0) / 127.0).clamp(
            vips::VImage::option()
                ->set("min", 0.0)
                ->set("max", 1.0)
        );

        auto mask3 = mask.bandjoin(mask).bandjoin(mask);

        image = image + mask3 * filters.highlight * 80.0;
    }
}

void Filters::shadows(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.shadows) > 0.01) {
        auto b = image.bandsplit();

        auto gray = b[0] * 0.299 + b[1] * 0.587 + b[2] * 0.114;

        auto mask = ((128.0 - gray) / 128.0).clamp(
            vips::VImage::option()
                ->set("min", 0.0)
                ->set("max", 1.0)
        );

        auto mask3 = mask.bandjoin(mask).bandjoin(mask);

        image = image + mask3 * filters.shadows * 80.0;
    }
}

void Filters::white_black(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.black) < 0.01 && std::abs(filters.white) < 0.01)
        return;

    const double black_point = filters.black * 80.0;
    const double white_point = 255.0 - filters.white * 80.0;

    if (white_point <= black_point + 1.0)
        return;

    const double scale = 255.0 / (white_point - black_point);
    const double offset = -black_point * scale;

    image = image.linear(scale, offset).cast(VIPS_FORMAT_UCHAR);


}
void Filters::vibrance(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.vibrance) > 0.01) {
        auto b = image.bandsplit();

        auto gray = b[0] * 0.299 + b[1] * 0.587 + b[2] * 0.114;
        auto gray3 = gray.bandjoin(gray).bandjoin(gray);

        double factor = 1.0 + filters.vibrance * 1.2;

        image = gray3 + (image - gray3) * factor;
    }
}

void Filters::vignette(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.vignette) > 0.01) {
        int width = image.width();
        int height = image.height();

        auto xy = vips::VImage::xyz(width, height);
        auto b = xy.bandsplit();

        auto x = b[0];
        auto y = b[1];

        double cx = width / 2.0;
        double cy = height / 2.0;

        auto dx = (x - cx) / cx;
        auto dy = (y - cy) / cy;

        auto dist = dx * dx + dy * dy;

        auto mask = ((dist - 0.25) / 0.75).clamp(
            vips::VImage::option()
                ->set("min", 0.0)
                ->set("max", 1.0)
        );

        auto mask3 = mask.bandjoin(mask).bandjoin(mask);

        image = image - mask3 * filters.vignette * 120.0;
    }
}

void Filters::clarity(vips::VImage& image, const FiltersState& filters)
{
    if (std::abs(filters.clarity) > 0.01) {
        auto blurred = image.gaussblur(3.0);
        auto detail = image - blurred;

        image = image + detail * filters.clarity * 1.5;
    }
}

void Filters::noise(vips::VImage& image, const FiltersState& filters)
{
    if (filters.noise > 0.01) {
        double sigma = std::lerp(0.3, 2.0, filters.noise);

        auto blurred = image.gaussblur(sigma);

        image = image * (1.0 - filters.noise)
              + blurred * filters.noise;
    }
}