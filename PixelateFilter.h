#pragma once
#include "IImageFilter.h"
#include <algorithm>

class PixelateFilter : public IImageFilter {
private:
    int blockSize;
public:
    explicit PixelateFilter(int size) : blockSize(size) {
        if (size <= 0) throw std::invalid_argument("Размер блока пикселизации должен быть больше 0");
    }

    Image apply(const Image& source) const override {
        Image result(source.getWidth(), source.getHeight());
        int w = source.getWidth();
        int h = source.getHeight();

        for (int y = 0; y < h; y += blockSize) {
            for (int x = 0; x < w; x += blockSize) {

                // Вычисление реальные границы текущего блока (защита от выхода за края фото)
                int endY = (std::min)(y + blockSize, h);
                int endX = (std::min)(x + blockSize, w);

                // Считаем средний цвет в блоке
                long long sumR = 0, sumG = 0, sumB = 0;
                int count = 0;

                for (int by = y; by < endY; ++by) {
                    for (int bx = x; bx < endX; ++bx) {
                        Pixel p = source.getPixel(bx, by);
                        sumR += p.r; sumG += p.g; sumB += p.b;
                        count++;
                    }
                }

                Pixel avgPixel{
                    static_cast<uint8_t>(sumR / count),
                    static_cast<uint8_t>(sumG / count),
                    static_cast<uint8_t>(sumB / count)
                };

                // Заполнение весь блок средним цветом
                for (int by = y; by < endY; ++by) {
                    for (int bx = x; bx < endX; ++bx) {
                        result.setPixel(bx, by, avgPixel);
                    }
                }
            }
        }
        return result;
    }
};
