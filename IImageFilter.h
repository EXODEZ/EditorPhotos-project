#pragma once
#include "Image.h"

class IImageFilter {
public:
    virtual ~IImageFilter() = default;
    virtual Image apply(const Image& source) const = 0;
};