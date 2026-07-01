#include <iostream>

#include "sobel.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    //----------------------------------------------------------
    // Read image
    //----------------------------------------------------------

    Mat rgb = imread("D:/prj_HW_SW/test_img/pepper3.jpg");

    if(rgb.empty())
    {
        cout << "Cannot open image!" << endl;
        return -1;
    }

    //----------------------------------------------------------
    // Convert to gray
    //----------------------------------------------------------

    Mat gray;

    cvtColor(
        rgb,
        gray,
        COLOR_BGR2GRAY
    );

    int width  = gray.cols;
    int height = gray.rows;

    cout << "Width  = " << width  << endl;
    cout << "Height = " << height << endl;

    //----------------------------------------------------------
    // AXI Streams
    //----------------------------------------------------------

    hls::stream<axis_t> in_stream;
    hls::stream<axis_t> out_stream;

    //----------------------------------------------------------
    // Write image to input stream
    //----------------------------------------------------------

    for(int r=0;r<height;r++)
    {
        for(int c=0;c<width;c++)
        {
            axis_t pixel;

            pixel.data = gray.at<uchar>(r,c);

            pixel.keep = -1;
            pixel.strb = -1;

            if(r==height-1 && c==width-1)
                pixel.last = 1;
            else
                pixel.last = 0;

            in_stream.write(pixel);
        }
    }

    //----------------------------------------------------------
    // Run Sobel IP
    //----------------------------------------------------------

    sobel_accel(
        in_stream,
        out_stream,
        width,
        height
    );

    //----------------------------------------------------------
    // Read result stream
    //----------------------------------------------------------

    Mat edge(
        height,
        width,
        CV_8UC1
    );

    for(int r=0;r<height;r++)
    {
        for(int c=0;c<width;c++)
        {
            axis_t pixel =
                out_stream.read();

            edge.at<uchar>(r,c) =
                (unsigned char)pixel.data;
        }
    }

    //----------------------------------------------------------
    // Save result
    //----------------------------------------------------------

    imwrite(
        "sobel_output.jpg",
        edge
    );

    cout << "Done!" << endl;

    return 0;
}