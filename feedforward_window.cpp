
#include <stdio.h>
#include <stdlib.h>

#define WEIGHTSIZE 51902
#define INPUTSIZE 1024
#define OUTPUTSIZE 10

#include "convolution_window.h"

void CNN_Accel_WD (AXI_VAL INPUT_STREAM[WEIGHTSIZE], AXI_VAL OUTPUT_STREAM[OUTPUTSIZE])
{
#pragma HLS INTERFACE ap_ctrl_none port=return
	//#pragma HLS INTERFACE ap_ctrl_none port=return
	//#pragma HLS INTERFACE s_axilite port=return     bundle=CONTROL_BUS
	#pragma HLS INTERFACE axis      port=OUTPUT_STREAM
	#pragma HLS INTERFACE axis      port=INPUT_STREAM

	if(!weightsValid)
	{
		loadweights(INPUT_STREAM, OUTPUT_STREAM);
		return;
	}
	else
	{
		accelerator(INPUT_STREAM, OUTPUT_STREAM);
		return;
	}


}

