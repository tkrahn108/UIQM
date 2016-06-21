#include "uism.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define LAMBDA_R 0.299F
#define LAMBDA_G 0.587F
#define LAMBDA_B 0.114F
#define BLOCKSIZE 4

float Uism::calculate(Mat img) {
    //split original image in its three channels
    Mat rChannelImg, gChannelImg, bChannelImg;

    vector<Mat> channels(3);
    split(img, channels);

    bChannelImg = channels[0];
    gChannelImg = channels[1];
    rChannelImg = channels[2];


    // Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Mat rSobelImg, gSobelImg, bSobelImg;
    //scale factor
    int scale = 1;
    int delta = 0;
    //output image depth
    int ddepth = CV_16S;

    //Gradient X for r-channel
    Sobel(rChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient X for r-channel
    Sobel(rChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, rSobelImg);

    //Gradient X for g-channel
    Sobel(gChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient X for r-channel
    Sobel(gChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gSobelImg);

    //Gradient X for b-channel
    Sobel(bChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient X for r-channel
    Sobel(bChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, bSobelImg);
    
    //multiply with original image
    Mat rGrayscaleEdge, gGrayscaleEdge, bGrayscaleEdge;

    //this doesn't work
    rGrayscaleEdge = img.mul(rSobelImg);
    

    waitKey(0);

    return 3.0f;
}
