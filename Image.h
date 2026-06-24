#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Pixel {
    uint8_t r, g, b;
    bool operator==(const Pixel& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};

struct PixelHSL {
    double h; // 0-360
    double s; // 0.0-1.0
    double l; // 0.0-1.0
};

inline PixelHSL rgbToHsl(Pixel rgb) {
    double r = rgb.r / 255.0;
    double g = rgb.g / 255.0;
    double b = rgb.b / 255.0;

    double max = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    double min = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);

    double h = 0, s = 0, l = (max + min) / 2.0;

    if (max != min) {
        double d = max - min;
        s = l > 0.5 ? d / (2.0 - max - min) : d / (max + min);
        if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (max == g) h = (b - r) / d + 2;
        else if (max == b) h = (r - g) / d + 4;
        h /= 6.0;
    }
    return PixelHSL{ h * 360.0, s, l };
}


inline double hueToRgb(double p, double q, double t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}

inline Pixel hslToRgb(PixelHSL hsl) {
    double h = hsl.h / 360.0;
    double s = hsl.s;
    double l = hsl.l;
    double r, g, b;

    if (s == 0) {
        r = g = b = l;
    }
    else {
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        r = hueToRgb(p, q, h + 1.0 / 3.0);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1.0 / 3.0);
    }
    return Pixel{
        static_cast<uint8_t>(std::round(r * 255.0)),
        static_cast<uint8_t>(std::round(g * 255.0)),
        static_cast<uint8_t>(std::round(b * 255.0))
    };
}

// Объявление функции stb для использования внутри класса Image
extern "C" {
    unsigned char* stbi_load(char const* filename, int* x, int* y, int* channels, int desired_channels);
    void stbi_image_free(void* retval_from_stbi_load);
    int stbi_write_png(char const* filename, int w, int h, int comp, void const* data, int stride_in_bytes);
}

class Image {
private:
    int width;
    int height;
    std::vector<Pixel> data;

public:
    Image(int w, int h) : width(w), height(h) {
        if (w <= 0 || h <= 0) {
            throw std::invalid_argument("Размеры должны быть положительными");
        }
        data.assign(w * h, Pixel{ 0, 0, 0 });
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    Pixel getPixel(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) throw std::out_of_range("Вне границ");
        return data[y * width + x];
    }

    void setPixel(int x, int y, const Pixel& pixel) {
        if (x < 0 || x >= width || y < 0 || y >= height) throw std::out_of_range("Вне границ");
        data[y * width + x] = pixel;
    }

    // Загрузка картинки
    static Image loadFromFile(const std::string& filename) {
        int w, h, channels;
        unsigned char* img_data = stbi_load(filename.c_str(), &w, &h, &channels, 3);

        if (!img_data) {
            throw std::runtime_error("Не удалось открыть файл: " + filename);
        }

        Image img(w, h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int index = (y * w + x) * 3;
                Pixel p{ img_data[index], img_data[index + 1], img_data[index + 2] };
                img.setPixel(x, y, p);
            }
        }

        stbi_image_free(img_data);
        return img;
    }

    // Сохранение картинки
    void saveToFile(const std::string& filename) const {
        std::vector<unsigned char> output_data(width * height * 3);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {

                int index = (y * width + x) * 3;
                Pixel p = getPixel(x, y);
                output_data[index] = p.r;
                output_data[index + 1] = p.g;
                output_data[index + 2] = p.b;
            }
        }

        int success = stbi_write_png(filename.c_str(), width, height, 3, output_data.data(), width * 3);
        if (!success) {
            throw std::runtime_error("Не удалось сохранить файл: " + filename);
        }
    }
};
