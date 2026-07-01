#ifndef SOBEL_H
#define SOBEL_H

#include <iostream>
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

#define MAX_WIDTH 640
#define MAX_HEIGHT 480
#define THRESHOLD 110

typedef ap_axiu<8,1,1,1> axis_t;

void sobel_accel(
    hls::stream<axis_t> &in_stream,
    hls::stream<axis_t> &out_stream,
    int width,
    int height
);

#endif