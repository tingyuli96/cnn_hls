/**********************************************/
//this test used row stationary in convolutional layer
/********************************************/
#ifndef CONVOLUTION_RS_H
#define CONVOLUTION_RS_H
#include <assert.h>
#include <ap_axi_sdata.h>
#include <math.h>
#include "ap_int.h"
#include "hls_stream.h"
//#include "cmath"
typedef ap_axiu<32,1,1,1> AXI_VAL;

#define IN_SIZE 32*32
#define OUT_SIZE 10

#define WEIGHTSIZE 51902
#define INPUTSIZE 1024
#define OUTPUTSIZE 10

	float input_image[1][32][32];
	float layerOutput1[6][28][28];
	float layerOutput2[6][14][14];
	float layerOutput3[16][10][10];
	float layerOutput4[16][5][5];
	float layerOutput5[120][1][1];
	float layerOutput6[10];
	//Load and rearrange weights*/

	float weights_layer1[150];
	float weights_layer3[2400];
	float weights_layer5[48000];
	float weights_layer6[1200]; //check this

	float bias_layer1[6];
	float bias_layer3[16];
	float bias_layer5[120];
	float bias_layer6[10];//check this

	//Get weights and biases separately.

	float max_out[6][14][14];
	float conv2_out[16][10][10];
	float max2_out[16][5][5];
	float conv3_out[120][1][1];
//define int
	/*
	int numOfParameters=51902;
	int layerWeights1=150;
	int layerWeights3=2400;
	int layerWeights5 = 48000;
	int layerWeights6 = 1200;

	int layerBias1=6;
	int layerBias3=16;
	int layerBias5=120;
	int layerBias6=10;

	int offSetL3= layerWeights1+layerBias1;
	int offSetL5 = offSetL3 + layerWeights3 + layerBias3;
	int offSetL6 = offSetL5 + layerWeights5 + layerBias5;
*/
	//using #define
#define numOfParameters 51902
#define layerWeights1 150
#define layerWeights3 2400
#define layerWeights5   48000
#define layerWeights6   1200

#define layerBias1 6
#define layerBias3 16
#define layerBias5 120
#define layerBias6 10

#define offSetL3  layerWeights1+layerBias1
#define offSetL5   offSetL3 + layerWeights3 + layerBias3
#define offSetL6   offSetL5 + layerWeights5 + layerBias5
	bool weightsValid = false;



void CNN_Accel (AXI_VAL INPUT_STREAM[WEIGHTSIZE + INPUTSIZE], AXI_VAL OUTPUT_STREAM[10]);
void standalone_conv (float input_image[1][32][32], float out[10] );
void accelerator(AXI_VAL inputstream[1024], AXI_VAL outputstream[10]);
void loadweights(AXI_VAL weights[51092], AXI_VAL output[10]);

template <typename T, int U, int TI, int TD>
T pop_stream(ap_axiu <sizeof(T)*8,U,TI,TD> const &e)
{
	#pragma HLS INLINE
          
	//assert(sizeof(T) == sizeof(int));
	union
	{
		int ival;
		T oval;
	} converter;
	converter.ival = e.data;
	T ret = converter.oval;

	volatile ap_uint<sizeof(T)> strb = e.strb;
	volatile ap_uint<sizeof(T)> keep = e.keep;
	volatile ap_uint<U> user = e.user;
	volatile ap_uint<1> last = e.last;
	volatile ap_uint<TI> id = e.id;
	volatile ap_uint<TD> dest = e.dest;

	return ret;
}

template <typename T, int U, int TI, int TD> ap_axiu <sizeof(T)*8,U,TI,TD> push_stream(T const &v, bool last = false)
{
	#pragma HLS INLINE
	ap_axiu<sizeof(T)*8,U,TI,TD> e;

	//assert(sizeof(T) == sizeof(int));
	union
	{
		int oval;
		T ival;
	} converter;
	converter.ival = v;
	e.data = converter.oval;

	// set it to sizeof(T) ones
	e.strb = -1;
	e.keep = 15; //e.strb;
	e.user = 0;
	e.last = last ? 1 : 0;
	e.id = 0;
	e.dest = 0;
	return e;
}

//processing element
/*
template<typename T>
void pe(T input, T filter, T &output){

	T mul_result;
	static T psum;

		mul_result = input * filter;
		psum += mul_result;
		output = psum;
	
	return;
}*/
/*

template<typename T,int in_num, int in_DIM, int out_DIM, int k_DIM, int out_num>
void arrayConv(hls::stream<T> &kernel, hls::stream<T> &input, hls::stream<T> &output, int sel){
	T input_var;
	T kernel_var;

	T mul_result;
	static T psum;
	if(sel == 1){
		mul_result = input_var * kernel_var;
		psum += mul_result;
		outMap = psum;
	}
	
	
	return;
}*/
/*
//convolutional core
#define MAX_IMG_COLS 28
#define MAX_IMG_ROWS 28
#define K 5
template<typename T,int K>
static void convolution_strm(int width, int height, hls::stream<T> &kernel, hls::stream<T> &input, 
	hls::stream<T> &output, const T *hcoeff, const T *vcoeff){

	float in_var;
	float filter_var;
	// hls::stream<T> hconv("hconv");
	// hls::stream<T> vconv("vconv"); 	//what's the meaning of this?
	// These assertions let HLS know the upper bounds of loops
	assert(height < MAX_IMG_ROWS);
	assert(width < MAX_IMG_COLS);
	assert(vconv_xlim < MAX_IMG_COLS - (K - 1));
	// Horizontal convolution
	for(int col = 0; col < height; col++) {
		for(int row = 0; row < width; row++) {
			for(int i = 0; i < K; i++) {
				 hls::stream 
	}
	}
	}
	// Vertical convolution
	VConvH:for(int col = 0; col < height; col++) {
	VConvW:for(int row = 0; row < vconv_xlim; row++) {
	VConv:for(int i = 0; i < K; i++) {
	}
	}
	Border:for (int i = 0; i < height; i++) {
	for (int j = 0; j < width; j++) {
	}

}
*/
//pe array

template <typename T, int in_num, int in_DIM, int out_DIM, int k_DIM, int  out_num> \
void convolutionLayer(T input[in_num][in_DIM][in_DIM], T weights[k_DIM* k_DIM* out_num], T bias[out_num], T output[out_num][out_DIM][out_DIM])
{
	const int k2_DIM=k_DIM*k_DIM;
	const int FACTOR=out_DIM/2;
	// const int pe_col=in_DIM-k_DIM+1; //pe_col = out_DIM
	int pe_ROW;
//	int flag;


	pe_ROW = out_num;
 	float ep;
	float em;
	//float sum;
	int stride =1 ;
	T kernel[out_num][in_num][k_DIM][k_DIM];
	// T kernel_fifo[out_num][in_num][k2_DIM];
//	#pragma HLS STREAM variable=kernel depth=5 dim=4

	T input_fifo[in_DIM];
	// T *inp;		//pointer of input_fifo
	T psum[pe_ROW][out_DIM];
	T mul_result[pe_ROW][out_DIM];
	// T output_temp[out_num][out_DIM][out_DIM];

//	assert(flag<16);
//	assert(pe_ROW < 51);

#pragma HLS DATAFLOW

//#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=3 partition

///*#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=0 partition*/
//#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=4 partition

/*#pragma HLS ARRAY_PARTITION variable=input block factor=5 dim=3 partition*/
/*
#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=1 partition
#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=2 partition
#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=3 partition
*/

	//Changing the data structure for weights and biases.
	Kernel1:for(int to=0; to<out_num; to++){
#pragma HLS LOOP_MERGE

	//	#pragma HLS DATAFLOW
		#pragma HLS PIPELINE
		Kernel2:for (int ti = 0; ti < in_num; ti++){
			Kernel3:for(int i=0; i<k_DIM;i++){
				Kernel4:for(int j = 0; j<k_DIM; j++)
					#pragma HLS loop_flatten
					kernel[to][ti][i][j] = weights[j+i*k_DIM+ti*k2_DIM+to*k2_DIM*in_num];
				}
		}
	}


/*********************************THE BELOW CODE IS TESTED*************************************/

	output1:for(int i=0; i< out_num; i++)
		#pragma HLS PIPELINE

#pragma HLS LOOP_MERGE
output2:for(int j=0; j< out_DIM; j++)
			output3:for(int k=0; k<out_DIM; k++)
			{
				#pragma HLS loop_flatten
				output[i][j][k] = 0;
//				output_temp[i][j][k] = 0;
			}

	/*************pearray TESTED**********************/
//	for(int tflag=0;tflag<15;tflag++){
//		if(tflag < flag){
//		#pragma HLS DATAFLOW
		con_innum:for(int ti=0;ti<in_num;ti++){
//#pragma HLS DATAFLOW
			con_outdimrow:for(int tir=0;tir<out_DIM;tir++){//count for the row of input map
				//initialize of psum
				#pragma HLS PIPELINE
				con_pe_outnumrow:for(int to=0;to<pe_ROW;to++){
					#pragma HLS UNROLL
					con_pe_outnumcol:for(int tpe_col=0;tpe_col<out_DIM;tpe_col++)
						psum[to][tpe_col] = 0;
				}
				//out_num*out_DIM pe array
				con_kernelrow:for(int tr=0;tr<k_DIM;tr++){//count for kernel row
					con_input_col:for(int tf=0;tf<in_DIM;tf++){//fill up input fifo
//						#pragma HLS UNROLL
						input_fifo[tf] = input[ti][tr][tf];
					}
					con_outnum:for(int to=0;to<pe_ROW;to++){//finish col conv
						#pragma HLS PIPELINE
						con_outdimcol:for(int tpe_col=0;tpe_col<out_DIM;tpe_col++)//implement col array
						{
							#pragma HLS UNROLL
							con_kernelcol:for(int i=0;i<k_DIM;i++){
								//element of pe
								mul_result[to][tpe_col] = input_fifo[tpe_col+i] * kernel[to][ti][tr][i];
								psum[to][tpe_col] += mul_result[to][tpe_col];
								//end of pe
								output[to][tir][tpe_col] = psum[to][tpe_col];
							}
						}
					}
				}
			}
		}
//		}
//		else
//			break;
//	}



softmax1:for(int to=0; to<out_num; to++)
	//#pragma HLS UNROLL
	softmax2:for(int row=0; row< out_DIM; row ++)
		//#pragma HLS UNROLL
#pragma HLS PIPELINE
		softmax3:for(int col=0; col< out_DIM; col ++)
		{
//			#pragma HLS DATAFLOW
			/*
			#pragma HLS RESOURCE variable=ep core=FExp_fulldsp THIS DOES NOT WORK: WARNING ISSUED IS - Cannot apply functional unit due to incompatible operation sets
			#pragma HLS RESOURCE variable=em core=FExp_fulldsp
			*/
			//#pragma HLS RESOURCE variable=output core = DSP48
			#pragma HLS PIPELINE rewind
			#pragma HLS UNROLL
			output[to][row][col] += bias[to];
			ep = exp(output[to][row][col]);
			em = exp(-output[to][row][col]);
			output[to][row][col] = (ep-em)/(ep+em);

		}
 }

//template <typename T, int in_num, int in_DIM, int out_DIM, int k_DIM, int  out_num> \
//void convolutionLayer(T input[in_num][in_DIM][in_DIM], T weights[k_DIM* k_DIM* out_num], T bias[out_num], T output[out_num][out_DIM][out_DIM])
//{
//	const int k2_DIM=k_DIM*k_DIM;
//	const int FACTOR=out_DIM/2;
//	// const int pe_col=in_DIM-k_DIM+1; //pe_col = out_DIM
////	int pe_ROW;
////	int flag;
//
//
////	pe_ROW = out_num;
// 	float ep;
//	float em;
//	//float sum;
//	int stride =1 ;
//	T kernel[out_num][in_num][k_DIM][k_DIM];
//	// T kernel_fifo[out_num][in_num][k2_DIM];
//	#pragma HLS STREAM variable=kernel depth=5 dim=4
//
//	// T input_fifo[in_DIM];
//	// T *inp;		//pointer of input_fifo
//	T psum[out_num][out_DIM];
//	T mul_result[out_num][out_DIM];
//	// T output_temp[out_num][out_DIM][out_DIM];
//
////	assert(flag<16);
////	assert(pe_ROW < 51);
//
//#pragma HLS DATAFLOW
//
////#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=3 partition
//
/////*#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=0 partition*/
////#pragma HLS ARRAY_PARTITION variable=kernel block factor=5 dim=4 partition
//
///*#pragma HLS ARRAY_PARTITION variable=input block factor=5 dim=3 partition*/
///*
//#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=1 partition
//#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=2 partition
//#pragma HLS ARRAY_PARTITION variable=output cyclic factor=FACTOR dim=3 partition
//*/
//
//	//Changing the data structure for weights and biases.
//	Kernel_loop1:for(int to=0; to<out_num; to++){
//	//	#pragma HLS DATAFLOW
//		Kernel_loop2:for (int ti = 0; ti < in_num; ti++){
//			Kernel_loop3:for(int i=0; i<k_DIM;i++){
//				Kernel_loop4:for(int j = 0; j<k_DIM; j++)
//					#pragma HLS loop_flatten
//					kernel[to][ti][i][j] = weights[j+i*k_DIM+ti*k2_DIM+to*k2_DIM*in_num];
//				}
//		}
//	}
//
//
///*********************************THE BELOW CODE IS TESTED*************************************/
//
//	Output_loop1:for(int i=0; i< out_num; i++)
//		Output_loop2:for(int j=0; j< out_DIM; j++)
//			Output_loop3:for(int k=0; k<out_DIM; k++)
//			{
//				#pragma HLS loop_flatten
//				output[i][j][k] = 0;
////				output_temp[i][j][k] = 0;
//			}
//
//	/*************pearray TESTED**********************/
////	for(int tflag=0;tflag<15;tflag++){
////		if(tflag < flag){
////		#pragma HLS DATAFLOW
//		Conv_innum:for(int ti=0;ti<in_num;ti++){
////#pragma HLS DATAFLOW
//			Conv_outDIMrow:for(int tir=0;tir<out_DIM;tir++){//count for the row of input map
//				//initialize of psum
//				Conv_psuminit_outnum:for(int to=0;to<out_num;to++){
//					#pragma HLS UNROLL
//					Conv_psuminit_outDIMcol:for(int tpe_col=0;tpe_col<out_DIM;tpe_col++)
//						psum[to][tpe_col] = 0;
//				}
//				//out_num*out_DIM pe array
//
//				Conv_kDIMrow:for(int tr=tir;tr<k_DIM;tr++){//count for kernel row
//					/*
//					Conv_inputinit_kDIMcol:for(int tf=0;tf<in_DIM;tf++){//fill up input fifo
////						#pragma HLS UNROLL
//						input_fifo[tf] = input[ti][tr][tf];
//						}*/
//					Conv_outnum:for(int to=0;to<out_num;to++){//finish col conv
//						#pragma HLS PIPELINE
//						Conv_outDIMcol:for(int tpe_col=0;tpe_col<out_DIM;tpe_col++)//implement col array
//						{
//							#pragma HLS UNROLL
////							Conv_kernelrow:for(int i=0;i<k_DIM;i++){
//								Conv_kernelcol:for(int j=0; j<k_DIM; j++){
//									//element of pe
//									mul_result[to][tpe_col] = input[ti][tir+tr][tpe_col+j] * kernel[to][ti][tr][j];
//									psum[to][tpe_col] += mul_result[to][tpe_col];
//									//end of pe
//									output[to][tir][tpe_col] = psum[to][tpe_col];
//
//								}
//
////							}
//						}
//					}
//				}
//			}
//		}
//
////		}
////		else
////			break;
////	}
//
//
//Softmax_outnum:for(int to=0; to<out_num; to++)
//	//#pragma HLS UNROLL
//	Softmax_outDIMrow:for(int row=0; row< out_DIM; row ++)
//		//#pragma HLS UNROLL
//		#pragma HLS PIPELINE
//		Softmax_outrol:for(int col=0; col< out_DIM; col ++)
//		{
////			#pragma HLS DATAFLOW
//			/*
//			#pragma HLS RESOURCE variable=ep core=FExp_fulldsp THIS DOES NOT WORK: WARNING ISSUED IS - Cannot apply functional unit due to incompatible operation sets
//			#pragma HLS RESOURCE variable=em core=FExp_fulldsp
//			*/
//			//#pragma HLS RESOURCE variable=output core = DSP48
//			#pragma HLS PIPELINE rewind
//			#pragma HLS UNROLL
//			output[to][row][col] += bias[to];
//			ep = exp(output[to][row][col]);
//			em = exp(-output[to][row][col]);
//			output[to][row][col] = (ep-em)/(ep+em);
//
//		}
// }


 template <typename T, int in_num, int in_DIM, int stride> \
 void maxPoolLayer(T input[in_num][in_DIM][in_DIM], T output[in_num][in_DIM/2][in_DIM/2])
 {
 	//insert max pool size
	float ep;
	float em;
	float max;

/******************************THE BELOW CODE IS TESTED************************************/

	for(int j=0; j<in_num; j++)
		{
			for(int row=0; row<in_DIM; row=row+stride)
			{
				for(int col=0; col<in_DIM; col=col+stride)
				{
					max = input[j][row][col];
					for(int ir=0; ir<stride; ir++)
					{
						//#pragma HLS UNROLL
						for(int ic=0; ic<stride; ic++)
						{
							if(input[j][row+ir][col+ic]>max)
							{
								max=input[j][row+ir][col+ic];
							}
						}
					}
					ep = exp(max);
					em = exp(-max);
					output[j][row/2][col/2] = (ep-em)/(ep+em);

				}
			}
		}
 }

 template <typename T, int in_num, int out_num>void fullyConnectedLayer(T input[in_num][1][1], T kernel[in_num*out_num], T bias[out_num], T output[out_num])
{


	//convert 2D to 1D
	float ep;
	float em;

	for(int i=0; i<out_num; i++)
	 	output[i]=0;

	for(int i=0; i< out_num; i++){
		//output[i]=0;
 		for(int j=0; j< in_num; j++){
 			output[i] += kernel[j*out_num + i] * input[j][0][0];
			}
		}

	for(int i=0; i<out_num; i++)
	{
		//#pragma HLS UNROLL
		output[i] += bias[i];
		ep = exp(output[i]);
		em = exp(-output[i]);
		output[i] = (ep-em)/(ep+em);
	}
}

 void loadweights(AXI_VAL weights[WEIGHTSIZE], AXI_VAL output[10])
 {

	 float outputstream[10];
	 //#pragma HLS INLINE
	 float weightsFromFile[WEIGHTSIZE];
	 for(int i=0; i<WEIGHTSIZE; i++)
	 {
		 weightsFromFile[i] = pop_stream<float,1,1,1>(weights[i]);
	 }

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
	 	for(int i=0; i<layerBias6; i++)
	 		bias_layer6[i] = weightsFromFile[offSetL6 + layerWeights6 + i];


	 	for(int i=0; i<10; i++)
	 		{
	 		outputstream[i] = 0;
	 		output[i] = push_stream<float,1,1,1>(outputstream[i], i == (10-1));
	 		}



	 	weightsValid = true;

	 	return;

 }

 void standalone_conv (float input_image[1][32][32],  float out[10] )

 {
//#pragma HLS PIPELINE

 	convolutionLayer<float, 1, 32, 28, 5, 6>(input_image, weights_layer1, bias_layer1, layerOutput1);
 	maxPoolLayer<float, 6, 28, 2>(layerOutput1, layerOutput2);
 	convolutionLayer<float, 6, 14, 10, 5, 16>(layerOutput2, weights_layer3, bias_layer3, layerOutput3);
 	maxPoolLayer<float, 16, 10, 2>(layerOutput3, layerOutput4);
 	convolutionLayer<float, 16, 5, 1, 5, 120>(layerOutput4, weights_layer5, bias_layer5, layerOutput5);
 	fullyConnectedLayer<float, 120, 10>(layerOutput5, weights_layer6, bias_layer6, out);

 }

 void accelerator(AXI_VAL inputstream[1024], AXI_VAL outputstream[10])
 {
float input[1][32][32];
float output[10];

for(int i=0; i<32; i++)
	for(int j=0; j<32; j++)
	{
#pragma HLS PIPELINE II=1
		int k = i*32+j;
		input[0][i][j] = pop_stream<float,1,1,1>(inputstream[k]);

	}

standalone_conv(input, output);


for(int i=0; i<10; i++)
{
#pragma HLS PIPELINE II=1
	outputstream[i] = push_stream<float,1,1,1>(output[i], i == (10-1));
}


 }


#endif
