#pragma once

#include <functional>
#include <optional>
#include <vips/vips8>

class FiltersState
{
public:
    static FiltersState* self_static_;
    static FiltersState& getInstance();
    
    double contrast {};
    double sharpness {};
    double sobel {};
};

class Filters
{
public:

    Filters() = default;
    static Filters& getInstace();

    void contrast(vips::VImage& image, const FiltersState& filters);
    void sharpness(vips::VImage& image, const FiltersState& filters);
    void sobel(vips::VImage& image, const FiltersState& filters);

    vips::VImage update(vips::VImage image, const FiltersState& filters);

    std::optional<vips::VImage> orginal_;
    std::optional<vips::VImage> current_;
    static Filters* self_static_;
    inline static std::optional<std::function<void()>> rerender = std::nullopt;
};
