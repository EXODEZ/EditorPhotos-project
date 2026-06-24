#pragma once
#include "IImageFilter.h"
#include <algorithm>

class SaturationFilter : public IImageFilter {
private:
    double factor;
public:
    explicit SaturationFilter(double f) : factor(f) {}
    Image apply(const Image& source) const override {
        Image result(source.getWidth(), source.getHeight());
        for (int y = 0; y < source.getHeight(); ++y) {
            for (int x = 0; x < source.getWidth(); ++x) {
                PixelHSL hsl = rgbToHsl(source.getPixel(x, y));
                hsl.s = std::max(0.0, std::min(1.0, hsl.s * factor));
                result.setPixel(x, y, hslToRgb(hsl));
            }
        }
        return result;
    }
};