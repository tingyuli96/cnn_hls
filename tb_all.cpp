
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <malloc.h>
#include <cmath>

#include "SdReader.h"

#include "convolution_RS_re302.h"


#define SIZE 1024


using namespace std;
//static int CheckResult(int i);
//static int CheckWeightsLoad(void);
//
float *weightsFromFile = (float *) malloc(numOfParameters*sizeof(float));
int *test_labels = (int *) malloc(10000 * sizeof(int));
std::vector<std::vector<float> > test_images;
int correctPrediction;
//
//float weightsFromFile[numOfParameters];
//int test_labels[10000];
//
//int load_file(){
//	ifstream fweights("MyFile.bin",std::ios::binary);
//	fweights.read((char*)weightsFromFile,numOfParameters*sizeof(float));
//	fweights.close();
//
//	ifstream fimages("images.bin",std::ios::binary);
//
//	ifstream flabel("labels.bin",std::ios::binary);
//
//}


int test_net(const std::string& weights)

{
	int img_label;
	int index;
	int predictedLabel;

	//declaration and allocation of memory for intermediate resutls

	float input_image[1][32][32]; // = (float *) malloc(1 * 32 * 32 * sizeof(float));
	float layerOutput1[6][28][28]; // = (float *) malloc(6 * 28 * 28 * sizeof(float));
	float layerOutput2[6][14][14]; // = (float *) malloc(6 * 14 * 14 * sizeof(float));
	float layerOutput3[16][10][10]; // = (float *) malloc(16 * 10 * 10 * sizeof(float));
	float layerOutput4[16][5][5];   // = (float *) malloc(16 * 5 * 5 * sizeof(float));
	float layerOutput5[120][1][1]; // = (float *) malloc(120 * sizeof(float));
	float layerOutput6[10];  //= (float *) malloc(10 * sizeof(float));

//	#ifdef DEBUG
//		print("start loading data\n");
//	#endif
		printf("Loading data from SD Card... \n");
		ReadFloatsFromFile(weightsFromFile, "MyFile.bin");
		parse_mnist_images("images.bin", &test_images, -1.0, 1.0, 2, 2);
		parse_mnist_labels("labels.bin", test_labels);
		printf("Data loaded from SD Card...\n");


	float weights_layer1[150];
	float weights_layer3[2400];
	float weights_layer5[48000];
	float weights_layer6[1200]; //check this




	float bias_layer1[6];
	float bias_layer3[16];
	float bias_layer5[120];
	float bias_layer6[10];//check this



	for(int i=0; i<layerWeights1; i++)
	{
		weights_layer1[i] = weightsFromFile[i];

	}

	for(int i=0; i<layerWeights3; i++)
	{
		weights_layer3[i] = weightsFromFile[offSetL3 +i];
	}

	for(int i=0; i<layerWeights5; i++)
	{
		weights_layer5[i] = weightsFromFile[offSetL5 +i];
	}

	for(int i=0; i<layerWeights6; i++)
	{
		weights_layer6[i] = weightsFromFile[offSetL6 +i];
	}


	for(int i=0; i<layerBias1; i++)
		bias_layer1[i] = weightsFromFile[layerWeights1 + i];
	for(int i=0; i<layerBias3; i++)
		bias_layer3[i] = weightsFromFile[offSetL3+layerWeights3 + i];
	for(int i=0; i<layerBias5; i++)
		bias_layer5[i] = weightsFromFile[offSetL5 + layerWeights5 + i];
	for(int i=0; i<layerBias6; i++){
		bias_layer6[i] = weightsFromFile[offSetL6 + layerWeights6 + i];
	}

		printf("Data Structure processed...\n");


	for(int imageNumber=0; imageNumber< 1; imageNumber++)
	{
		img_label = test_labels[imageNumber];
		for(int i=0; i<32; i++)
		{
			for(int j=0; j<32; j++)
			{
				input_image[0][i][j] = test_images[imageNumber][i*32 + j];
			}
		}
		//Check how to input image
		convolutionLayer<float, 1, 32, 28, 5, 6>(input_image, weights_layer1, bias_layer1, layerOutput1);
		maxPoolLayer<float, 6, 28, 2>(layerOutput1, layerOutput2);
		convolutionLayer<float, 6, 14, 10, 5, 16>(layerOutput2, weights_layer3, bias_layer3, layerOutput3);
		maxPoolLayer<float, 16, 10, 2>(layerOutput3, layerOutput4);
		convolutionLayer<float, 16, 5, 1, 5, 120>(layerOutput4, weights_layer5, bias_layer5, layerOutput5);
		fullyConnectedLayer<float, 120, 10>(layerOutput5, weights_layer6, bias_layer6, layerOutput6);

		float max = layerOutput6[0];
		index =0;

		for(int i=0; i<10; i++)
		{
			if(layerOutput6[i] > max)
			{
				max = layerOutput6[i];
				index = i;
			}
		}

		predictedLabel = index;
		if(predictedLabel == img_label) correctPrediction++;
	}


	printf("Neural Network Accuracy is  %f \n", correctPrediction*100.0/10000);


	return 1;
}


int main() {


	printf ("*******Testing CNN with MNIST Dataset******** \n");
	int check;
    check = test_net("weights.bin");
    if(check!=1)
    {
    	printf("DMA ERROR\n");
    }


    printf("******CNN tested successfully*******\n");

}



