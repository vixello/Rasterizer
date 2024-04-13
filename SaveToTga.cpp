#define _CRT_SECURE_NO_WARNINGS
#include "SaveToTga.h"

void SaveToTga::SaveImage(const char* filename, unsigned short* header, int width, int height, const std::vector<unsigned int>& buffer)
{
    FILE* f = fopen(filename, "wb+");
    if (!f) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    header[6] = width;
    header[7] = height;

    fwrite(header, sizeof(unsigned short), 9, f);
    fwrite(buffer.data(), sizeof(unsigned int), width * height, f);

    fclose(f);
    std::cout << "Image saved to " << filename << std::endl;
}

