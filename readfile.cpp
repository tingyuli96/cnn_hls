#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

#define GO
int ReadFile(float *weightsFromFile, const std::string file_name);
int ReadFile(float *weightsFromFile, const std::string file_name)
{
//	streampos size;
//	char *memblock;
	char *fileName;
	char *buffer;
	float buffer_f;
	fileName = (char*)file_name.c_str();
	cout << fileName << endl;
#ifdef GO
	ifstream file("MyFile.bin",ios::in|ios::binary|ios::ate);
	if(file.is_open())
	{
		cout << "file open" << endl;
//		size = file.tellg();
//		memblock = new char[size];
//		file.seekg(0,ios::beg);
		for(int i = 0; i< 10; i++)
		{
			file.seekg(i,ios::beg);
			file.read(buffer,1);
			cout <<"buffer:" << buffer << endl;
			buffer_f = (float)(*buffer);
			weightsFromFile[i] = buffer_f;
			cout << "weights"<< i << "=" << weightsFromFile[i] << endl;
			cout <<"buffer"<<i<<" = " << buffer_f << endl;
		}
		file.close();
/*
		for(int i=0; i<10; i++)
		{
			cout <<i+":"<< memblock[i] <<endl;
		}
*/
//		delete[] memblock;
	}
	else cout << "open file failed"<<endl;
#endif
	return 0;
}

int main()
{
	float weightsFromFile[51902];
	ReadFile(weightsFromFile, "MyFile.bin");
	return 0;
}	
