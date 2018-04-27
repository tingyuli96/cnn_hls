
#define APPL
//#define ACCL
#define ARMONLY
#ifdef APPL
#include <iostream>
#include <stdio.h>
//#include "tiny_cnn/tiny_cnn.h"
//#include "layersCustom.h"
//#include "SdReader.h"
#include <vector>
#include <malloc.h>
#include <cmath>
//#include <xtime_l.h>
//#include <sys/time.h>
//#include <xaxidma.h>
//#include "user_dma.h"
//#include "xparameters.h"

#include "convolution_RS_re302.h"


#define SIZE 1024

//	int numOfParameters=51902;
//	int layerWeights1=150;
//	int layerWeights3=2400;
//	int layerWeights5 = 48000;
//	int layerWeights6 = 1200;
//
//	int layerBias1=6;
//	int layerBias3=16;
//	int layerBias5=120;
//	int layerBias6=10;
//
//	int offSetL3= layerWeights1+layerBias1;
//	int offSetL5 = 2572; //offSetL3 + layerWeights3 + layerBias3;
//	int offSetL6 = offSetL5 + layerWeights5 + layerBias5;



////	XAxiDma AxiDma;
//
//
//#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
//
//#ifndef DDR_BASE_ADDR
//#define MEM_BASE_ADDR		0x01000000
//#else
//#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)
//#endif
//#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
//#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
//#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)
//#define IMG_PKT_LEN			SIZE*4
//#define W_PKT_LEN			numOfParameters*4
//#define RES_PKT_LEN 		10*4
//
//	FATFS FatFs;

using namespace std;
//static int CheckResult(int i);
//static int CheckWeightsLoad(void);

float *weightsFromFile = (float *) malloc(numOfParameters*sizeof(float));
int *test_labels = (int *) malloc(10000 * sizeof(int));
std::vector<std::vector<float> > test_images;
int correctPrediction;




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

	printf("Loading data from SD Card... \n");
//	ReadFloatsFromSDFile(weightsFromFile, "MyFile.bin");
//	parse_mnist_images("images.bin", &test_images, -1.0, 1.0, 2, 2);
//	parse_mnist_labels("labels.bin", test_labels);
//	printf("Data loaded from SD Card...\n");

#ifdef ARMONLY
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
#endif
		printf("Data Structure processed...\n");

#ifdef ARMONLY

//		XTime start, end;
//
//		XTime_GetTime(&start);

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

//	XTime_GetTime(&end);
//	double time = double(end - start) / COUNTS_PER_SECOND;
//	printf("Elapsed time excluding data loading is %.2f seconds\n", time);


	printf("Neural Network Accuracy is  %f \n", correctPrediction*100.0/10000);
#endif

	return 1;
}

//
//static int CheckWeightsLoad(void)
//{
//	float *RxPacket;
//	int Index = 0;
//	int count = 0;
//
//	RxPacket = (float *) RX_BUFFER_BASE;
//
//	for(Index = 0; Index < 10; Index++)
//	{
//		if (RxPacket[Index] != 0) {
//			count ++;
//		}
//	}
//	if(count == 0)
//	printf("Weights loaded into Accelerator....\n");
//	return XST_SUCCESS;
//}
//
//static int CheckResult(int i)
//{
//	float *RxPacket;
//	int Index = 0;
//	int maxIndex = 0;
//
//	RxPacket = (float *) RX_BUFFER_BASE;
//	float max;
//
//	/* Invalidate the DestBuffer before receiving the data, in case the
//	 * Data Cache is enabled
//	 */
//#ifndef __aarch64__
//	Xil_DCacheInvalidateRange((UINTPTR)RxPacket, IMG_PKT_LEN);
//#endif
//
//	max = RxPacket[0];
//	for(Index = 0; Index < 10; Index++)
//	{
//		if(RxPacket[Index] > max)
//		{
//			max = RxPacket[Index];
//			maxIndex = Index;
//
//		}
//	}
//
//	if(test_labels[i] == maxIndex)
//	{
//		correctPrediction++;
//		//printf("CORRECT PREDICTION of : %d\n", maxIndex);
//	}
//	return XST_SUCCESS;
//}

int main() {

//	XTime testBegin, testEnd;
//
//	XTime_GetTime(&testBegin);

	printf ("*******Testing CNN with MNIST Dataset******** \n");
	int check;
    check = test_net("weights.bin");
    if(check!=1)
    {
    	printf("DMA ERROR\n");
    }

//    XTime_GetTime(&testEnd);
//    double diff = double(testEnd - testBegin) / COUNTS_PER_SECOND;
//    printf("Elapsed time is %.2f seconds\n", diff);

    printf("******CNN tested successfully*******\n");

}
#endif


