// ±‡“Î÷∏¡Ó
// g++ -o test test.cpp `pkg-config --cflags --libs opencv`

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    VideoCapture capture(2);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    capture.set(CV_CAP_PROP_FPS, 20); //÷° ˝
    Mat frame;
 
    while (true)
    {
        capture >> frame;
        imshow("test", frame);
        waitKey(10);
    }

    return 0;
}
