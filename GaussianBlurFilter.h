#pragma once
#include "IImageFilter.h"
#include <cmath>
#include <vector>
#include <algorithm>

class GaussianBlurFilter : public IImageFilter {
private:
    double sigma;
    std::vector<std::vector<double>> kernel;

    void generateKernel() {
        kernel.resize(3, std::vector<double>(3));
        double sum = 0.0;
        int radius = 1;
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                double exponent = -(x * x + y * y) / (2.0 * sigma * sigma);
                kernel[y + radius][x + radius] = std::exp(exponent) / (2.0 * M_PI * sigma * sigma);
                sum += kernel[y + radius][x + radius];
            }
        }
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                kernel[y][x] /= sum;
            }
        }
    }

public:
    explicit GaussianBlurFilter(double sigmaValue = 1.0) : sigma(sigmaValue) {
        if (sigma <= 0.0) throw std::invalid_argument("Sigma > 0");
        generateKernel();
    }

    Image apply(const Image& source) const override {
        int w = source.getWidth();
        int h = source.getHeight();
        Image result(w, h);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double sumR = 0.0, sumG = 0.0, sumB = 0.0;
                double weightSum = 0.0;

                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int nx = std::max(0, std::min(w - 1, x + kx));
                        int ny = std::max(0, std::min(h - 1, y + ky));

                        Pixel p = source.getPixel(nx, ny);
                        double weight = kernel[ky + 1][kx + 1];

                        sumR += p.r * weight;
                        sumG += p.g * weight;
                        sumB += p.b * weight;
                        weightSum += weight;
                    }
                }
                result.setPixel(x, y, Pixel{
                    static_cast<uint8_t>(std::max(0.0, std::min(255.0, sumR / weightSum))),
                    static_cast<uint8_t>(std::max(0.0, std::min(255.0, sumG / weightSum))),
                    static_cast<uint8_t>(std::max(0.0, std::min(255.0, sumB / weightSum)))
                    });
            }
        }
        return result;
    }
};