#include "sobel.h"

void sobel_accel(
    hls::stream<axis_t> &in_stream,
    hls::stream<axis_t> &out_stream,
    int width,
    int height
)
{
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream

#pragma HLS INTERFACE s_axilite port=width
#pragma HLS INTERFACE s_axilite port=height
#pragma HLS INTERFACE s_axilite port=return
    static ap_uint<8> line_buffer[2][MAX_WIDTH] ;
    ap_uint<8> window[3][3] = {0};

ROW_LOOP:
    for (int r = 0; r < height; r++)
    {
#pragma HLS LOOP_TRIPCOUNT min=480 max=480
    COL_LOOP:
        for (int c = 0; c < width; c++)
        {
#pragma HLS LOOP_TRIPCOUNT min=640 max=640
            axis_t inpix = in_stream.read();
            ap_uint<8> pixel = inpix.data;

            window[0][0] = window[0][1];
            window[0][1] = window[0][2];

            window[1][0] = window[1][1];
            window[1][1] = window[1][2];

            window[2][0] = window[2][1];
            window[2][1] = window[2][2];

            window[0][2] = line_buffer[0][c];
            window[1][2] = line_buffer[1][c];
            window[2][2] = pixel;

            line_buffer[0][c] = line_buffer[1][c];
            line_buffer[1][c] = pixel;

            axis_t outpix;

            if(r < 2 || c < 2)
            {
                outpix.data = 0;
            }
            else
            {
                int gx =
                    -window[0][0]
                    +window[0][2]
                    -2*window[1][0]
                    +2*window[1][2]
                    -window[2][0]
                    +window[2][2];

                int gy =
                    window[0][0]
                    +2*window[0][1]
                    +window[0][2]
                    -window[2][0]
                    -2*window[2][1]
                    -window[2][2];
                int mag =
                    (gx<0?-gx:gx) +
                    (gy<0?-gy:gy);
                if(mag > 255)
                    mag = 255;
                outpix.data = (mag >= THRESHOLD) ? 255 : 0;
                
                

                //outpix.data = (ap_uint<8>)mag;  
            }
            outpix.keep = -1;
            outpix.strb = -1;

            if(r==(height-1) && c==(width-1))
                outpix.last = 1;
            else
                outpix.last = 0;

            out_stream.write(outpix);
        }
            
    }
        

}