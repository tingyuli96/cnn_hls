#include "convolution_RS_re302.h"
#include <iostream>
using namespace std;
#define DEBUG

int main(void){
	for(int i=0; i<32; i++){
		for(int j=0; j<32; j++){
			input_image[0][i][j] = 1;
		}
	}
	for(int i=0; i<150; i++){
		weights_layer1[i] = 1;
	}
	for(int i=0; i<6; i++){
		bias_layer1[i] = 0;
	}
	convolutionLayer<float, 1, 32, 28, 5, 6>(input_image, weights_layer1, bias_layer1, layerOutput1);
	for(int i=0; i<28; i++){
		for(int j=0; j<28; j++){
			cout << "layerOutput1["<< i << "] = " << layerOutput1[i] << endl;
		}
	}
	return 0;
}
