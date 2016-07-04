#include "uism.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//coefficents for the addition of channel values; this are default values
#define LAMBDA_R 0.299F
#define LAMBDA_G 0.587F
#define LAMBDA_B 0.114F
//height and width for each quadratic block
#define BLOCKSIZE 4

/**
 * Calculates the UISM (underwater image sharpness measure) value according Panetta et al. 
 * @param img Image for which the value should be calculated
 * @return UISM value
 */
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
    //Gradient Y for r-channel
    Sobel(rChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, rSobelImg);

    //Gradient X for g-channel
    Sobel(gChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient Y for r-channel
    Sobel(gChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gSobelImg);

    //Gradient X for b-channel
    Sobel(bChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient Y for r-channel
    Sobel(bChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, bSobelImg);

    //grayscale edge maps which results from multiplication with the original image
    Mat rGrayscaleEdge, gGrayscaleEdge, bGrayscaleEdge;

    //i don't think that this multiplication is correct?!
    rGrayscaleEdge = rChannelImg.mul(rSobelImg);
    gGrayscaleEdge = gChannelImg.mul(gSobelImg);
    bGrayscaleEdge = bChannelImg.mul(bSobelImg);

//    namedWindow("TestR", WINDOW_NORMAL);
//    imshow("TestR", rGrayscaleEdge);
//    namedWindow("TestG", WINDOW_NORMAL);
//    imshow("TestG", gGrayscaleEdge);
//    namedWindow("TestB", WINDOW_NORMAL);
//    imshow("TestB", bGrayscaleEdge);
//    waitKey(0);

    //EME ...
    int k1 = img.rows / BLOCKSIZE;
    int k2 = img.cols / BLOCKSIZE;
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;
    double logarithmusValue;

    int min, max;

    for (int i = 1; i <= k1; i++) {
        for (int j = 1; j <= k2; j++) {
            //... for red-channel
            findMinMax(rGrayscaleEdge, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            //which logarithmus function do i have to use? log or log10? What to do if max and/or are zero?
//            if (min == 0) {
//                min = 1;
//            }
//            if (max == 0) {
//                max = 1;
//            }
            if (!(min == 0 || max == 0))
            {
            logarithmusValue = ((double) max) / ((double) min);
            sumR += log(logarithmusValue);
            }
            //... for green channel
            findMinMax(gGrayscaleEdge, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            //which logarithmus function do i have to use? log or log10? What to do if max and/or are zero?
            if (!(min == 0 || max == 0)){
            logarithmusValue = ((double) max) / ((double) min);
            sumG += log(logarithmusValue);
            }
            
            //... for blue channel
            findMinMax(bGrayscaleEdge, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            //which logarithmus function do i have to use? log or log10? What to do if max and/or are zero?
            if (!(min == 0 || max == 0)){
            logarithmusValue = ((double) max) / ((double) min);
            sumB += log(logarithmusValue);
            }
            //            cout << "i: " << i << endl;
            //            cout << "j: " << j << endl;
            //            cout << "min: " << min << endl;
            //            cout << "max: " << max << endl;
            //            cout << "logarithmus Value: " << log(logarithmusValue) << endl;
            //            cout << "sumR: " << sumR << endl;
        }
    }
    sumR *= 2.0 / (k1 * k2);
    sumG *= 2.0 / (k1 * k2);
    sumB *= 2.0 / (k1 * k2);
//    cout << "sumR: " << sumR << endl;
//    cout << "sumG: " << sumG << endl;
//    cout << "sumB: " << sumB << endl;

    float result = LAMBDA_R * sumR + LAMBDA_G * sumG + LAMBDA_B * sumB;
    cout << "UISM: " << result << endl;
    return result;
}

/**
 * Calculates the minimum and maximum intensity values within a specified area 
 * within the given image.
 * @param img Image 
 * @param xmin Lower x-value for the area
 * @param xmax Upper x-value for the area
 * @param ymin Lower y-value for the area
 * @param ymax Upper y-value for the area
 * @param min Minimum intensity value within the area with the edge points 
 * (xmin,ymin) and (xmax, ymax) of the image
 * @param max Maximum intensity value within the area with the edge points 
 * (xmin,ymin) and (xmax, ymax) of the image
 */
void Uism::findMinMax(Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max) {
    int tempMax = 0;
    int tempMin = INT_MAX;
    int tempValue = 0;
    Scalar intensity;

    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            intensity = img.at<uchar>(j, i);
            tempValue = intensity.val[0];
            //            cout << tempValue << ", ";
            if (tempValue < tempMin) {
                tempMin = tempValue;
            }
            if (tempValue > tempMax) {
                tempMax = tempValue;
            }
        }
        //        cout << endl;
    }
    min = tempMin;
    max = tempMax;
}