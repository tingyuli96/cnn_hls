#include <assert.h>
#include <ap_axi_sdata.h>
#include <math.h>
#include "ap_int.h"
//#include "hls_stream.h"
#include "hls_video.h"

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

//convolutional core
/*
template<typename T,int K>
static void convolution_strm(int width, int height, hls::stream<T> &input, hls::stream<T> &output){

}*/
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

	hls::Window<5,5,float> input_buff;

	// T *inp;		//pointer of input_fifo
//	T psum[pe_ROW][out_DIM];
//	T mul_result[pe_ROW][out_DIM];
	// T output_temp[out_num][out_DIM][out_DIM];
//	T output_buff[];
	T output_sm_buff[out_DIM];

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
	k1:for(int to=0; to<out_num; to++)
		k2:for (int ti = 0; ti <in_num; ti++)
			k3:for(int i=0; i<k_DIM; i++)
//#pragma UNROLL#
				k4:for(int j=0; j< k_DIM; j++)
				{
					kernel[to][ti][i][j] = weights[j+i*k_DIM+ti*k_DIM*k_DIM+to*k_DIM*k_DIM*in_num];
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

//#pragma HLS DATAFLOW

	int toc = 0;
	// float input_insert[3];
	con_innum:for(int ti=0;ti<in_num;ti++){
//		#pragma HLS PIPELINE
		//initial of window map
		init_inputcol: for(int i=0; i<k_DIM; i++){
				inti_inputrow: for(int j=0; j<k_DIM; j++){
					#pragma HLS UNROLL
					input_buff.insert(input[ti][i][j], i, j);
				}
			}
		con_outdimrow:for(int tor=0;tor<out_DIM;tor++){
//			#pragma HLS PIPELINE
			input_buff.shift_down();
			for(int iis=0; iis<k_DIM; iis++){
				// #pragma HLS UNROLL
				input_buff.insert(input[ti][tor+k_DIM-1][toc+iis],k_DIM-1, iis);
			}

			if(tor%2 == 0){
				con_outdimcoleven:for(toc=0; toc<out_DIM; toc++){
					if(toc!=0){
						input_buff.shift_right();
						for(int iis=0; iis<k_DIM; iis++){
							// #pragma HLS UNROLL
							input_buff.insert(input[ti][tor+iis][toc+k_DIM-1],iis,k_DIM-1);
						}
					}
					con_kernelroweven: for(int tkr=0; tkr< k_DIM; tkr=tkr++){
						con_kernelcoleven: for(int tkc=0; tkc<k_DIM; tkc++){
							con_outnumeven:for(int to=0; to<out_num; to++){
								// #pragma HLS UNROLL
								output[to][tor][toc] += input_buff.getval(tkr,tkc) * kernel[to][ti][tkc][tkr];
							}
						}
					}
				}
			}
			if(tor%2 == 1){
				con_outdimcolodd: for(toc=out_DIM; toc>0; toc--){
					if(toc != out_DIM){
						input_buff.shift_left();
						for(int iis=0; iis<k_DIM; iis++){
							// #pragma HLS UNROLL
							input_buff.insert(input[ti][tor+iis][toc-1],iis,0);
						}
					}
					con_kernelrowodd: for(int tkr=0; tkr< k_DIM; tkr=tkr++){
						con_kernelcolodd: for(int tkc=0; tkc<k_DIM; tkc++){
							con_outnumodd:for(int to=0; to<out_num; to++){
								// #pragma HLS UNROLL
								output[to][tor][toc] += input_buff.getval(tkr,tkc) * kernel[to][ti][tkc][tkr];
							}
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
	// #pragma HLS PIPELINE
		softmax3:for(int col=0; col< out_DIM; col ++)
		{
//			#pragma HLS DATAFLOW
			/*
			#pragma HLS RESOURCE variable=ep core=FExp_fulldsp THIS DOES NOT WORK: WARNING ISSUED IS - Cannot apply functional unit due to incompatible operation sets
			#pragma HLS RESOURCE variable=em core=FExp_fulldsp
			*/
			//#pragma HLS RESOURCE variable=output core = DSP48
			// #pragma HLS PIPELINE
			#pragma HLS UNROLL
			output_sm_buff[col] = output[to][row][col] + bias[to];
			ep = exp(output_sm_buff[col]);
			em = exp(-output_sm_buff[col]);
			output[to][row][col] = (ep-em)/(ep+em);

		}
 }


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


