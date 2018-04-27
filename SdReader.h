
#include <exception>
#include <string>
#include <sstream>
//#include "xparameters.h"
//#include "xsdps.h"
#include <stdio.h>
//#include "ff.h"
//#include "xil_cache.h"
#include <vector>
#include <assert.h>
#include <fstream>




//void ReadFloatsFromFile(float *weightsFromFile, const std::string file_name);
void ReadBytesFromFile(std::stringstream &stream, const std::string file_name);
void parse_mnist_labels(const std::string& label_file, int *labels);

void parse_mnist_images(const std::string& image_file,
    std::vector<std::vector<float> > *images,
    float scale_min,
    float scale_max,
    int x_padding,
    int y_padding);


struct mnist_header {
    uint32_t magic_number;
    uint32_t num_items;
    uint32_t num_rows;
    uint32_t num_cols;
};


class image {
public:
    typedef unsigned char intensity_t;

    image() : width_(0), height_(0) {}

    image(size_t width, size_t height) : width_(width), height_(height), data_(width * height, 0) {}

    /*void write(const std::string& path) const {
        std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::out);
//#ifndef BOOST_LITTLE_ENDIAN
//        throw nn_error("not implemented");
//#endif

        const uint32_t line_pitch = ((width_ + 3) / 4) * 4;
        const uint32_t header_size = 14 + 12 + 256 * 3;
        const uint32_t data_size = line_pitch * height_;

        // file header(14 byte)
        const uint16_t file_type = ('M' << 8) | 'B';
        const uint32_t file_size = header_size + data_size;
        const uint32_t reserved = 0;
        const uint32_t offset_bytes = header_size;

        ofs.write((const char*)&file_type, 2);
        ofs.write((const char*)&file_size, 4);
        ofs.write((const char*)&reserved, 4);
        ofs.write((const char*)&offset_bytes, 4);

        // info header(12byte)
        const uint32_t info_header_size = 12;
        const uint16_t width = static_cast<uint16_t>(width_);
        const uint16_t height = static_cast<uint16_t>(height_);
        const uint16_t planes = 1;
        const uint16_t bit_count = 8;

        ofs.write((const char*)&info_header_size, 4);
        ofs.write((const char*)&width, 2);
        ofs.write((const char*)&height, 2);
        ofs.write((const char*)&planes, 2);
        ofs.write((const char*)&bit_count, 2);

        // color palette (256*3byte)
        for (int i = 0; i < 256; i++) {
            const char v = static_cast<const char>(i);
            ofs.write(&v, 1);//R
            ofs.write(&v, 1);//G
            ofs.write(&v, 1);//B
        }

        // data
        for (size_t i = 0; i < height_; i++) {
            ofs.write((const char*)&data_[i * width_], width_);
            if (line_pitch != width_) {
                uint32_t dummy = 0;
                ofs.write((const char*)&dummy, line_pitch - width_);
            }
        }
    }*/

    void resize(size_t width, size_t height) {
        data_.resize(width * height);
        width_ = width;
        height_ = height;
    }

    void fill(intensity_t value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    intensity_t& at(size_t x, size_t y) {
        assert(x < width_);
        assert(y < height_);
        return data_[y * width_ + x];
    }

    const intensity_t& at(size_t x, size_t y) const {
        assert(x < width_);
        assert(y < height_);
        return data_[y * width_ + x];
    }

    size_t width() const { return width_; }
    size_t height() const { return height_; }
    const std::vector<intensity_t>& data() const { return data_; }
private:
    size_t width_;
    size_t height_;
    std::vector<intensity_t> data_;
};
