#include "SdReader.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;
void ReadFloatsFromFile(float *weightsFromFile, const std::string file_name)
{
	char *file;

}
void ReadBytesFromFile(std::stringstream &stream, const std::string file_name)
{
//		FIL fil;
//		FATFS fatfs;
//		FILINFO file_info;
//		char *SD_File;
//		std::stringstream sstream;
//		FRESULT Res;
//
//		UINT NumBytesRead;
//
//		Res = f_mount(&fatfs, "0:/", 0);
//
//		if (Res != FR_OK) printf("Could not mount SD card.\n");
//
//		SD_File = (char *)file_name.c_str();
//
//		Res = f_open(&fil, SD_File, FA_READ | FA_OPEN_EXISTING);
//		if (Res) throw Res;
//
//		//printf("IMAGES FILE OPENED\n");
//
//		Res = f_lseek(&fil, 0);
//		if (Res) printf("Failed to seek opened file.\n");
//
//
//		Res = f_stat(SD_File, &file_info);
//		DWORD fil_size = file_info.fsize;
//
//		const int Buff_Size = 1;
//		char fbuffer[Buff_Size];
//		for(DWORD i = 0; i < fil_size/Buff_Size; i++) {
//
//			Res = f_read(&fil, fbuffer, Buff_Size, &NumBytesRead);
//			if (Res) throw "Failed to read file.";
//			if (NumBytesRead != 1) throw "Too many bytes read";
//			stream.write(fbuffer, Buff_Size);
//			//if(i==9999)printf("ALL DATA READ SUCCEFULLY\n");
//		}
//
//		Res = f_close(&fil);
//		if (Res) printf("Failed to close file\n");
//		//printf("IMAGES FILE CLOSED \n");
	char *file;
	char *buffer;

	file = (char *)file_name.c_str();
	std::ifstream fstr(file,std::ios::binary);
	while(fstr.get(*buffer)){
		stream.write(buffer,1);
	}

	fstr.close();
//	else{
//		cerr << "couldn't open" + string(File) << endl;
//	}


}


void parse_mnist_labels(const std::string& label_file, int *labels) {
//
////	FIL fil;
////	FATFS fatfs;
////	FILINFO file_info;
//	char *SD_File;
////	//std::stringstream sstream;
////	FRESULT Res;
////	UINT NumBytesRead;
//
//	//printf("OPENED LABEL FILE \n");
//
//	Res = f_mount(&fatfs, "0:/", 0);
//
//	if (Res != FR_OK) printf("Could not mount SD card.\n");
//
//	SD_File = (char *)label_file.c_str();
//
//	Res = f_open(&fil, SD_File, FA_READ | FA_OPEN_EXISTING);
//	if (Res) throw Res;
//
//	int magicnumber[4];
//	int numberofimages[4];
//
//	Res = f_lseek(&fil, 0);
//	if (Res) printf("Failed to seek opened file.\n");
//
//	Res = f_stat(SD_File, &file_info);
//	DWORD fil_size = file_info.fsize;
//
//	const int Buff_Size = 1;
//	char fbuffer;
	char *file;
	file = (char *)label_file.c_str();
	int magicnumber[4];
	int numberofimages[4];
	char buffer;
	ifstream fstr(file,std::ios::binary);
	int i = 0;
	while(fstr.get(buffer)) {
		if(i<4)
		{
			magicnumber[i] = buffer;
		}
		else if(i>3 && i< 8)
		{
			numberofimages[i] = buffer;
		}
		else if(i>7)
		{
			labels[i-8] =  buffer;
		}
		i++;
	}

	if(magicnumber[0]!=0 || magicnumber[1]!=0 || magicnumber[2]!=8 || magicnumber[3]!=1)// printf("Magic Number failed \n");
		fstr.close();
	else
		cout << "fail to close file\n"<< endl;
}

void parse_mnist_header(std::stringstream& ifs, mnist_header& header) {
    ifs.read((char*) &header.magic_number, 4);
    ifs.read((char*) &header.num_items, 4);
    ifs.read((char*) &header.num_rows, 4);
    ifs.read((char*) &header.num_cols, 4);

   /* reverse_endian(&header.magic_number);
    reverse_endian(&header.num_items);
    reverse_endian(&header.num_rows);
    reverse_endian(&header.num_cols);
*/

    if (header.magic_number != 0x00000803 || header.num_items <= 0)
       // printf("MNIST image header format error");
    if (ifs.fail() || ifs.bad())
        printf("file error");

   // printf("MNIST HEADER PASSED \n");
}


void parse_mnist_image2(std::stringstream& ifs,
    const mnist_header& header,
    float scale_min,
    float scale_max,
    int x_padding,
    int y_padding,
    std::vector<float>& dst) {
    const int width = 28 + 2 * x_padding;
    const int height = 28 + 2 * y_padding;

    std::vector<uint8_t> image_vec(28 * 28);

    ifs.read((char*) &image_vec[0], 28 * 28);

    dst.resize(width * height, scale_min);

    for (size_t y = 0; y < 28; y++)
    for (size_t x = 0; x < 28; x++)
        dst[width * (y + y_padding) + x + x_padding]
        = (image_vec[y * 28 + x] / 255.0) * (scale_max - scale_min) + scale_min;
}

void parse_mnist_images(const std::string& image_file,
    std::vector<std::vector<float> > *images,
    float scale_min = -1.0,
    float scale_max = 1.0,
    int x_padding = 2,
    int y_padding = 2)
{
	std::stringstream ifs;
	ReadBytesFromFile(ifs, image_file.c_str());

   if (ifs.bad() || ifs.fail())
        printf("failed to open file: images.bin\n");
   //printf("DONe\n");

    mnist_header header;

    parse_mnist_header(ifs, header);

    for (size_t i = 0; i <10000; i++) {
        std::vector<float> image;
        parse_mnist_image2(ifs, header, scale_min, scale_max, x_padding, y_padding, image);
        images->push_back(image);
    }
}
