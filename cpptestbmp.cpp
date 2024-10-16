#include <iostream>
#include <fstream>
#include <windows.h>

class BMPReader {
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char* pixelData;
    int width;
    int height;
    int bitCount;

public:
    BMPReader() : pixelData(nullptr) {}

    bool openBMP(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file.\n";
            return false;
        }

        // Read the file header
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
        if (fileHeader.bfType != 0x4D42) {  
            std::cerr << "Error: Not a valid BMP file.\n";
            file.close();
            return false;
        }

        // Read the info header
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));
        width = infoHeader.biWidth;
        height = abs(infoHeader.biHeight);
        bitCount = infoHeader.biBitCount;

        if (bitCount != 24 && bitCount != 32) {
            std::cerr << "Error: Only 24-bit or 32-bit BMP files are supported.\n";
            file.close();
            return false;
        }

        // Read pixel data
        int rowSize = ((bitCount * width + 31) / 32) * 4; 
        int dataSize = rowSize * height;
        pixelData = new unsigned char[dataSize];

        file.seekg(fileHeader.bfOffBits, std::ios::beg);
        file.read(reinterpret_cast<char*>(pixelData), dataSize);
        file.close();

        return true;
    }

    void displayBMP() const {
        if (!pixelData) {
            std::cerr << "Error: No pixel data loaded.\n";
            return;
        }

        // Display the image in the console
        for (int y = height - 1; y >= 0; --y) { 
            for (int x = 0; x < width; ++x) {
                int pixelIndex = (y * width + x) * (bitCount / 8);
                unsigned char blue = pixelData[pixelIndex];
                unsigned char green = pixelData[pixelIndex + 1];
                unsigned char red = pixelData[pixelIndex + 2];

                if (red == 255 && green == 255 && blue == 255) {
                    std::cout << ' ';  
                }
                else if (red == 0 && green == 0 && blue == 0) {
                    std::cout << '#';  
                }
                else {
                    std::cerr << "Error: Unsupported color detected. Only black and white are allowed.\n";
                    return;
                }
            }
            std::cout << '\n';
        }
    }

    void closeBMP() {
        delete[] pixelData;
        pixelData = nullptr;
    }

    ~BMPReader() {
        closeBMP();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: drawBmp.exe <path_to_bmp_file>\n";
        return 1;
    }

    BMPReader bmpReader;
    if (!bmpReader.openBMP(argv[1])) {
        return 1;
    }

    bmpReader.displayBMP();
    bmpReader.closeBMP();
    return 0;
}
