#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <filesystem> // Для вывода текущей директории

// Структура для заголовка BMP-файла
struct BMPHeader {
    char signature[2]; // Сигнатура "BM"
    int fileSize;      // Размер файла
    int reserved;      // Зарезервировано
    int dataOffset;    // Смещение до данных пикселей
    int headerSize;    // Размер заголовка
    int width;         // Ширина изображения
    int height;        // Высота изображения
    short planes;      // Количество плоскостей (всегда 1)
    short bitCount;    // Глубина цвета (24 для 24-битного BMP)
    int compression;   // Тип сжатия (0 для несжатого)
    int imageSize;     // Размер данных пикселей
    int xPixelsPerMeter; // Горизонтальное разрешение
    int yPixelsPerMeter; // Вертикальное разрешение
    int colorsUsed;    // Количество используемых цветов
    int importantColors; // Количество важных цветов
};

// Функция для чтения BMP-файла
bool readBMP(const std::string& filename, BMPHeader& header, std::vector<uint8_t>& pixels) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return false;
    }

    // Читаем заголовок
    file.read(reinterpret_cast<char*>(&header.signature), 2);
    file.read(reinterpret_cast<char*>(&header.fileSize), 4);
    file.read(reinterpret_cast<char*>(&header.reserved), 4);
    file.read(reinterpret_cast<char*>(&header.dataOffset), 4);
    file.read(reinterpret_cast<char*>(&header.headerSize), 4);
    file.read(reinterpret_cast<char*>(&header.width), 4);
    file.read(reinterpret_cast<char*>(&header.height), 4);
    file.read(reinterpret_cast<char*>(&header.planes), 2);
    file.read(reinterpret_cast<char*>(&header.bitCount), 2);
    file.read(reinterpret_cast<char*>(&header.compression), 4);
    file.read(reinterpret_cast<char*>(&header.imageSize), 4);
    file.read(reinterpret_cast<char*>(&header.xPixelsPerMeter), 4);
    file.read(reinterpret_cast<char*>(&header.yPixelsPerMeter), 4);
    file.read(reinterpret_cast<char*>(&header.colorsUsed), 4);
    file.read(reinterpret_cast<char*>(&header.importantColors), 4);

    // Проверяем, что это 24-битное BMP
    if (header.signature[0] != 'B' || header.signature[1] != 'M' || header.bitCount != 24) {
        std::cerr << "Ошибка: файл не является 24-битным BMP-изображением!" << std::endl;
        return false;
    }

    // Выделяем память для данных пикселей
    pixels.resize(header.imageSize);

    // Читаем данные пикселей
    file.seekg(header.dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixels.data()), header.imageSize);

    file.close();
    return true;
}

// Функция для записи BMP-файла
bool writeBMP(const std::string& filename, const BMPHeader& header, const std::vector<uint8_t>& pixels) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
        return false;
    }

    // Записываем заголовок
    file.write(reinterpret_cast<const char*>(&header.signature), 2);
    file.write(reinterpret_cast<const char*>(&header.fileSize), 4);
    file.write(reinterpret_cast<const char*>(&header.reserved), 4);
    file.write(reinterpret_cast<const char*>(&header.dataOffset), 4);
    file.write(reinterpret_cast<const char*>(&header.headerSize), 4);
    file.write(reinterpret_cast<const char*>(&header.width), 4);
    file.write(reinterpret_cast<const char*>(&header.height), 4);
    file.write(reinterpret_cast<const char*>(&header.planes), 2);
    file.write(reinterpret_cast<const char*>(&header.bitCount), 2);
    file.write(reinterpret_cast<const char*>(&header.compression), 4);
    file.write(reinterpret_cast<const char*>(&header.imageSize), 4);
    file.write(reinterpret_cast<const char*>(&header.xPixelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&header.yPixelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&header.colorsUsed), 4);
    file.write(reinterpret_cast<const char*>(&header.importantColors), 4);

    // Записываем данные пикселей
    file.write(reinterpret_cast<const char*>(pixels.data()), header.imageSize);

    file.close();
    return true;
}

// Функция для поворота изображения на 90 градусов по часовой стрелке
std::vector<uint8_t> rotateClockwise(const std::vector<uint8_t>& pixels, int width, int height) {
    int channels = 3; // 24 бита = 3 канала (BGR)
    int rowSize = (width * channels + 3) & ~3; // Выравнивание строк

    // Размеры повернутого изображения
    int newWidth = height;
    int newHeight = width;
    int newRowSize = (newWidth * channels + 3) & ~3; // Выравнивание строк для нового изображения

    // Выделяем память для повернутого изображения
    std::vector<uint8_t> rotatedPixels(newRowSize * newHeight);

    // Поворачиваем изображение
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                // Вычисляем новую позицию пикселя
                int newX = y;
                int newY = width - x - 1;

                // Копируем пиксель
                rotatedPixels[newY * newRowSize + newX * channels + c] =
                    pixels[y * rowSize + x * channels + c];
            }
        }
    }

    return rotatedPixels;
}

// Функция для поворота изображения на 90 градусов против часовой стрелки
std::vector<uint8_t> rotateCounterClockwise(const std::vector<uint8_t>& pixels, int width, int height) {
    int channels = 3; // 24 бита = 3 канала (BGR)
    int rowSize = (width * channels + 3) & ~3; // Выравнивание строк

    // Размеры повернутого изображения
    int newWidth = height;
    int newHeight = width;
    int newRowSize = (newWidth * channels + 3) & ~3; // Выравнивание строк для нового изображения

    // Выделяем память для повернутого изображения
    std::vector<uint8_t> rotatedPixels(newRowSize * newHeight);

    // Поворачиваем изображение
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                // Вычисляем новую позицию пикселя
                int newX = height - y - 1;
                int newY = x;

                // Копируем пиксель
                rotatedPixels[newY * newRowSize + newX * channels + c] =
                    pixels[y * rowSize + x * channels + c];
            }
        }
    }

    return rotatedPixels;
}

// Функция для применения фильтра Гаусса
std::vector<uint8_t> applyGaussianFilter(const std::vector<uint8_t>& pixels, int width, int height) {
    int channels = 3; // 24 бита = 3 канала (BGR)
    int rowSize = (width * channels + 3) & ~3; // Выравнивание строк

    // Ядро Гаусса 3x3
    const float kernel[3][3] = {
        {1.0f / 16, 2.0f / 16, 1.0f / 16},
        {2.0f / 16, 4.0f / 16, 2.0f / 16},
        {1.0f / 16, 2.0f / 16, 1.0f / 16}
    };

    // Выделяем память для результата
    std::vector<uint8_t> result(pixels.size());

    // Применяем фильтр Гаусса
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;

                // Свёртка с ядром Гаусса
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int pixelIndex = ((y + ky) * rowSize) + ((x + kx) * channels) + c;
                        sum += pixels[pixelIndex] * kernel[ky + 1][kx + 1];
                    }
                }

                // Записываем результат
                int resultIndex = (y * rowSize) + (x * channels) + c;
                result[resultIndex] = static_cast<uint8_t>(sum);
            }
        }
    }

    return result;
}

int main() {
    // Вывод текущей директории
    std::cout << "Текущая директория: " << std::filesystem::current_path() << std::endl;

    const std::string inputFile = "input.bmp"; // Входное изображение
    const std::string outputClockwise = "clockwise.bmp"; // Поворот по часовой стрелке
    const std::string outputCounterClockwise = "counter_clockwise.bmp"; // Поворот против часовой стрелки
    const std::string outputFiltered = "filtered.bmp"; // Фильтр Гаусса

    BMPHeader header;
    std::vector<uint8_t> pixels;

    // Читаем BMP-файл
    if (!readBMP(inputFile, header, pixels)) {
        return -1;
    }

    // Выводим информацию о выделенной памяти
    std::cout << "Выделено памяти для изображения: " << pixels.size() << " байт" << std::endl;

    // 1. Поворачиваем изображение на 90 градусов по часовой стрелке и сохраняем
    std::vector<uint8_t> clockwisePixels = rotateClockwise(pixels, header.width, header.height);
    BMPHeader clockwiseHeader = header;
    std::swap(clockwiseHeader.width, clockwiseHeader.height);
    clockwiseHeader.imageSize = clockwisePixels.size();

    if (!writeBMP(outputClockwise, clockwiseHeader, clockwisePixels)) {
        return -1;
    }
    std::cout << "Изображение повернуто по часовой стрелке и сохранено в " << outputClockwise << std::endl;

    // 2. Поворачиваем изображение на 90 градусов против часовой стрелки и сохраняем
    std::vector<uint8_t> counterClockwisePixels = rotateCounterClockwise(pixels, header.width, header.height);
    BMPHeader counterClockwiseHeader = header;
    std::swap(counterClockwiseHeader.width, counterClockwiseHeader.height);
    counterClockwiseHeader.imageSize = counterClockwisePixels.size();

    if (!writeBMP(outputCounterClockwise, counterClockwiseHeader, counterClockwisePixels)) {
        return -1;
    }
    std::cout << "Изображение повернуто против часовой стрелки и сохранено в " << outputCounterClockwise << std::endl;

    // 3. Применяем фильтр Гаусса к повёрнутому изображению (по часовой стрелке) и сохраняем
    std::vector<uint8_t> filteredPixels = applyGaussianFilter(clockwisePixels, clockwiseHeader.width, clockwiseHeader.height);

    if (!writeBMP(outputFiltered, clockwiseHeader, filteredPixels)) {
        return -1;
    }
    std::cout << "К повёрнутому изображению применён фильтр Гаусса и сохранено в " << outputFiltered << std::endl;

    return 0;
}
