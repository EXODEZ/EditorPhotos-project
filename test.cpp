#include "pch.h"
#include "gtest/gtest.h"
#include "Image.h"
#include "BrightnessFilter.h"
#include "GaussianBlurFilter.h"
#include "InverseFilter.h"
#include "PixelateFilter.h"
#include <memory>

// Тест фикстура
class ImageFilterTest : public ::testing::Test {
protected:
    std::unique_ptr<Image> testImage;

    void SetUp() override {
        testImage = std::make_unique<Image>(2, 2);
        testImage->setPixel(0, 0, Pixel{ 10, 20, 30 });
        testImage->setPixel(1, 0, Pixel{ 40, 50, 60 });
        testImage->setPixel(0, 1, Pixel{ 100, 110, 120 });
        testImage->setPixel(1, 1, Pixel{ 200, 210, 220 });
    }
};

// Тесты яркости
TEST_F(ImageFilterTest, BrightnessPositiveShift) {
    BrightnessFilter filter(10);
    Image result = filter.apply(*testImage);
    EXPECT_EQ(result.getPixel(0, 0), Pixel({ 20, 30, 40 }));
}

TEST_F(ImageFilterTest, BrightnessOverflow) {
    BrightnessFilter filter(100);
    Image result = filter.apply(*testImage);
    EXPECT_EQ(result.getPixel(1, 1), Pixel({ 255, 255, 255 }));
}

// Тесты исключений
TEST(ImageExceptionTest, InvalidSizeAndBounds) {
    EXPECT_THROW(Image(-5, 10), std::invalid_argument);
    Image img(10, 20);
    EXPECT_THROW(img.getPixel(10, 21), std::out_of_range);
}

// Тесты размытия по Гауссу
TEST(GaussianBlurTest, BlurFromImageStaysUniform) {
    Image img(3, 3);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            img.setPixel(x, y, Pixel{ 128, 128, 128 });
        }
    }
    GaussianBlurFilter filter(1.0);
    Image result = filter.apply(img);
    EXPECT_EQ(result.getPixel(1, 1), Pixel({ 128, 128, 128 }));
}

// проверка инверсии цветов (InverseFilter)
TEST_F(ImageFilterTest, InverseColorsCorrectly) {
    InverseFilter filter;
    Image result = filter.apply(*testImage);
    // Для пикселя (0,0) со значениями {10, 20, 30} инверсия: 255-10=245, 255-20=235, 255-30=225
    EXPECT_EQ(result.getPixel(0, 0), Pixel({ 245, 235, 225 }));
}

// проверка усреднения цветов 8-bit эффекта (PixelateFilter)
TEST(PixelateFilterTest, DynamicBlockAveraging) {
    Image img(2, 2);
    img.setPixel(0, 0, Pixel{ 100, 50, 200 });
    img.setPixel(1, 0, Pixel{ 0,   50, 100 });
    img.setPixel(0, 1, Pixel{ 200, 50, 0 });
    img.setPixel(1, 1, Pixel{ 100, 50, 100 });

    PixelateFilter filter(2); // Размер блока равен размеру картинки
    Image result = filter.apply(img);

    // Среднее: R=(100+0+200+100)/4=100, G=50, B=(200+100+0+100)/4=100
    Pixel expected{ 100, 50, 100 };
    EXPECT_EQ(result.getPixel(0, 0), expected);
    EXPECT_EQ(result.getPixel(1, 1), expected);
}

// проверка защиты от неверного размера блока 8-bit фильтра
TEST(PixelateFilterTest, InvalidBlockSizeException) {
    EXPECT_THROW(PixelateFilter(0), std::invalid_argument);
    EXPECT_THROW(PixelateFilter(-5), std::invalid_argument);
}

// Главная функция запуска тестов
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
