#pragma once
#include "IImageFilter.h"
#include <algorithm>

class BrightnessFilter : public IImageFilter {
private:
    int shift;
public:
    explicit BrightnessFilter(int shiftValue) : shift(shiftValue) {}
    Image apply(const Image& source) const override {
        Image result(source.getWidth(), source.getHeight());
        for (int y = 0; y < source.getHeight(); ++y) {
            for (int x = 0; x < source.getWidth(); ++x) {
                Pixel p = source.getPixel(x, y);
                p.r = std::max(0, std::min(255, p.r + shift));
                p.g = std::max(0, std::min(255, p.g + shift));
                p.b = std::max(0, std::min(255, p.b + shift));
                result.setPixel(x, y, p);
            }
        }
        return result;
    }
};