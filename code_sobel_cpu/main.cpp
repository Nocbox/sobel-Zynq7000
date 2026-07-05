#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define THRESHOLD 110

int main()
{
    //--------------------------------------------------
    // Read image
    //--------------------------------------------------
    Mat img = imread("test.jpg", IMREAD_GRAYSCALE);

    if (img.empty())
    {
        cout << "Cannot open image!" << endl;
        return -1;
    }

    resize(img, img, Size(640, 480));

    int width = img.cols;
    int height = img.rows;

    Mat output(height, width, CV_8UC1, Scalar(0));

    //--------------------------------------------------
    // Start timer
    //--------------------------------------------------
    auto start = chrono::high_resolution_clock::now();

    //--------------------------------------------------
    // Sobel Edge Detection
    //--------------------------------------------------
    for (int r = 2; r < height; r++)
    {
        for (int c = 2; c < width; c++)
        {
            int gx =
                -img.at<uchar>(r - 2, c - 2)
                + img.at<uchar>(r - 2, c)

                -2 * img.at<uchar>(r - 1, c - 2)
                +2 * img.at<uchar>(r - 1, c)

                -img.at<uchar>(r, c - 2)
                +img.at<uchar>(r, c);

            int gy =
                 img.at<uchar>(r - 2, c - 2)
                +2 * img.at<uchar>(r - 2, c - 1)
                +img.at<uchar>(r - 2, c)

                -img.at<uchar>(r, c - 2)
                -2 * img.at<uchar>(r, c - 1)
                -img.at<uchar>(r, c);

            int mag = abs(gx) + abs(gy);

            if (mag > 255)
                mag = 255;

            output.at<uchar>(r, c) = (mag >= THRESHOLD) ? 255 : 0;
        }
    }

    //--------------------------------------------------
    // Stop timer
    //--------------------------------------------------
    auto stop = chrono::high_resolution_clock::now();

    double execution_time =
        chrono::duration<double, milli>(stop - start).count();

    //--------------------------------------------------
    // Save output image
    //--------------------------------------------------
    imwrite("sobel_cpu.png", output);

    //--------------------------------------------------
    // Print results
    //--------------------------------------------------
    cout << "=====================================" << endl;
    cout << "Sobel Edge Detection (CPU - C++)" << endl;
    cout << "Image Size     : " << width << " x " << height << endl;
    cout << "Threshold      : " << THRESHOLD << endl;
    cout << "Execution Time : " << execution_time << " ms" << endl;
    cout << "FPS            : " << 1000.0 / execution_time << endl;
    cout << "Output Image   : sobel_cpu.png" << endl;
    cout << "=====================================" << endl;

    return 0;
}