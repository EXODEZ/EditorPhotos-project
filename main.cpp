#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h> 
#include <commdlg.h> 

// Подключение всех модулей проекта
#include "Image.h"
#include "IImageFilter.h"
#include "BrightnessFilter.h"
#include "SaturationFilter.h"
#include "GaussianBlurFilter.h"
#include "InverseFilter.h"
#include "PixelateFilter.h"

// Реализация библиотек stb для чтения/записи изображений
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Диалоговое окно открытия файла
std::string openFileDialog() {
    char filename[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Изображения (*.jpg;*.jpeg;*.png)\0*.jpg;*.jpeg;*.png\0Все файлы (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Выберите фотографию для редактирования";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) return std::string(filename);
    return "";
}

// Диалоговое окно сохранения файла
std::string saveFileDialog() {
    char filename[MAX_PATH] = "output.png";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Изображение PNG (*.png)\0*.png\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Выберите, куда сохранить отредактированное фото";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn)) return std::string(filename);
    return "";
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::cout << "=========================================\n";
    std::cout << "=== КОНСОЛЬНЫЙ ФОТОРЕДАКТОР С ТЕСТАМИ ===\n";
    std::cout << "=========================================\n\n";
    std::cout << "Открытие проводника для выбора фотографии...\n";

    std::string inputPath = openFileDialog();
    if (inputPath.empty()) {
        std::cout << "Файл не был выбран. Завершение работы.\n";
        return 0;
    }

    try {
        std::cout << "Загрузка изображения: " << inputPath << "\n";
        Image myImage = Image::loadFromFile(inputPath);
        std::cout << "Изображение загружено! Размер: " << myImage.getWidth() << "x" << myImage.getHeight() << " пикселей.\n\n";

        // МЕНЮ
        std::cout << "Выбор доступных модулей обработки:\n";
        std::cout << "  1. Изменение яркости (BrightnessFilter)\n";
        std::cout << "  2. Изменение насыщенности (SaturationFilter)\n";
        std::cout << "  3. Размытие по Гауссу (GaussianBlurFilter)\n";
        std::cout << "  4. Инверсия цветов (InverseFilter)\n";
        std::cout << "  5. Ретро 8-bit эффект (PixelateFilter)\n";
        std::cout << "-----------------------------------------\n";
        std::cout << "Введите номер фильтра: ";
        int choice;
        std::cin >> choice;

        Image* processedImage = nullptr;
        BrightnessFilter* bFilter = nullptr;
        SaturationFilter* sFilter = nullptr;
        GaussianBlurFilter* gFilter = nullptr;
        InverseFilter* iFilter = nullptr;
        PixelateFilter* pFilter = nullptr;

        if (choice == 1) {
            int shift;
            std::cout << "Введите сдвиг яркости (от -255 до 255): ";
            std::cin >> shift;
            bFilter = new BrightnessFilter(shift);
            processedImage = new Image(bFilter->apply(myImage));
        }
        else if (choice == 2) {
            double factor;
            std::cout << "Введите коэффициент насыщенности (например, 0.5 или 2.0): ";
            std::cin >> factor;
            sFilter = new SaturationFilter(factor);
            processedImage = new Image(sFilter->apply(myImage));
        }
        else if (choice == 3) {
            double sigma;
            std::cout << "Введите силу размытия sigma (например, 2.0): ";
            std::cin >> sigma;
            gFilter = new GaussianBlurFilter(sigma);
            processedImage = new Image(gFilter->apply(myImage));
        }
        else if (choice == 4) {
            std::cout << "Применение инверсии цветов...\n";
            iFilter = new InverseFilter();
            processedImage = new Image(iFilter->apply(myImage));
        }
        else if (choice == 5) {
            int size;
            std::cout << "Введите размер 8-bit блока в пикселях (рекомендуется от 4 до 16): ";
            std::cin >> size;
            std::cout << "Группировка пикселей в ретро-блоки...\n";
            pFilter = new PixelateFilter(size);
            processedImage = new Image(pFilter->apply(myImage));
        }
        else {
            std::cout << "Некорректный выбор.\n";
            return 0;
        }

        if (processedImage != nullptr) {
            std::cout << "\nОткрытие проводника для сохранения результата...\n";
            std::string outputPath = saveFileDialog();

            if (!outputPath.empty()) {
                processedImage->saveToFile(outputPath);
                std::cout << "Файл успешно сохранен по пути: " << outputPath << "\n";
            }
            else {
                std::cout << "Путь не выбран. Изменения потеряны.\n";
            }

            delete processedImage;
            if (bFilter) delete bFilter;
            if (sFilter) delete sFilter;
            if (gFilter) delete gFilter;
            if (iFilter) delete iFilter;
            if (pFilter) delete pFilter;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "\nКРИТИЧЕСКАЯ ОШИБКА МОДУЛЯ: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\nРабота программы успешно завершена. Нажмите Enter для выхода...";
    std::cin.get(); std::cin.get();
    return 0;
}
