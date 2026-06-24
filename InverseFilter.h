#pragma once
#include "IImageFilter.h"
#include <algorithm>

class InverseFilter : public IImageFilter {
public:
    explicit InverseFilter() {}

    Image apply(const Image& source) const override {
        Image result(source.getWidth(), source.getHeight());
        for (int y = 0; y < source.getHeight(); ++y) {
            for (int x = 0; x < source.getWidth(); ++x) {
                Pixel p = source.getPixel(x, y);
                p.r = 255 - p.r;
                p.g = 255 - p.g;
                p.b = 255 - p.b;
                result.setPixel(x, y, p);
            }
        }
        return result;
    }
};
