#include "uism.h"
#include "stdio.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//coefficents for the addition of channel values; this are default values
#define LAMBDA_R 0.299F
#define LAMBDA_G 0.587F
#define LAMBDA_B 0.114F
//#define LAMBDA_R 0.33F
//#define LAMBDA_G 0.33F
//#define LAMBDA_B 0.33F
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
    abs_grad_x.convertTo(abs_grad_x, CV_32F);
    abs_grad_y.convertTo(abs_grad_y, CV_32F);
    //magnitude = sqrt(abs_grad_x^2 + abs_grad_y^2);
    magnitude(abs_grad_x, abs_grad_y, rSobelImg);

    //    namedWindow("GradY", WINDOW_NORMAL);
    //    imshow("GradY", abs_grad_y);
    //    namedWindow("GradX", WINDOW_NORMAL);
    //    imshow("GradX", abs_grad_x);
    //    namedWindow("rSobel", WINDOW_NORMAL);
    //    imshow("rSobel", rSobelImg);
    //    waitKey(0);



    //Gradient X for g-channel
    Sobel(gChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient Y for g-channel
    Sobel(gChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    abs_grad_x.convertTo(abs_grad_x, CV_32F);
    abs_grad_y.convertTo(abs_grad_y, CV_32F);
    magnitude(abs_grad_x, abs_grad_y, gSobelImg);

    //Gradient X for b-channel
    Sobel(bChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    //Gradient Y for b-channel
    Sobel(bChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    abs_grad_x.convertTo(abs_grad_x, CV_32F);
    abs_grad_y.convertTo(abs_grad_y, CV_32F);
    magnitude(abs_grad_x, abs_grad_y, bSobelImg);

    //grayscale edge maps which results from multiplication with the original image
    Mat rGrayscaleEdge, gGrayscaleEdge, bGrayscaleEdge;
    // sobel konvertieren zu double und auf [0,1] normieren, original konvertieren zu double, dann 'rechnen', dann in uint8 konvertieren.

    //     cout << "rChannelImage before Conversion: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rChannelImg.at<uchar>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }
    //
    //    
    double minVal = 0;
    double maxVal = 0;
    //    minMaxLoc(rSobelImg, &minVal, &maxVal);
    //    cout << "Values before Conversion: " << minVal << ", " << maxVal << endl;
    //    
    //    cout << "rSobelImage before Conversion: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rSobelImg.at<uchar>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }


    rChannelImg.convertTo(rChannelImg, CV_32F);
    gChannelImg.convertTo(gChannelImg, CV_32F);
    bChannelImg.convertTo(bChannelImg, CV_32F);

    //    minMaxLoc(rSobelImg, &minVal, &maxVal);
    //    cout << "Values after Conversion: " << minVal << ", " << maxVal << endl;
    //
    //    
    //    cout << "rChannelImage: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rChannelImg.at<float>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }
    //
    //    cout << "rSobelImage: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rSobelImg.at<float>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }

    rGrayscaleEdge = rChannelImg.mul(rSobelImg);
    gGrayscaleEdge = gChannelImg.mul(gSobelImg);
    bGrayscaleEdge = bChannelImg.mul(bSobelImg);

    //    cout << "rGrayscaleEdge: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rGrayscaleEdge.at<float>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }
    //    

    minMaxLoc(rGrayscaleEdge, &minVal, &maxVal);
    rGrayscaleEdge.convertTo(rGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    minMaxLoc(gGrayscaleEdge, &minVal, &maxVal);
    gGrayscaleEdge.convertTo(gGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    minMaxLoc(bGrayscaleEdge, &minVal, &maxVal);
    bGrayscaleEdge.convertTo(bGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);

    //    cout << "rGrayscaleEdge after conversion: " << endl;
    //    for (int i = 0; i < 20; i++) {
    //        for (int j = 0; j < 20; j++) {
    //            Scalar intensity = rGrayscaleEdge.at<uchar>(j, i);
    //            cout << intensity.val[0] << "\t";
    //        }
    //        cout << endl;
    //    }

    // eventuell hier noch mal merge
    //    vector<Mat> channelsMerge;
    //    channelsMerge.push_back(bGrayscaleEdge);
    //    channelsMerge.push_back(gGrayscaleEdge);
    //    channelsMerge.push_back(rGrayscaleEdge);
    //    
    //    Mat mergedImg;
    //    merge(channelsMerge, mergedImg);
    //    
    //    namedWindow("Merge", WINDOW_NORMAL);
    //    imshow("Merge", mergedImg);
    //    waitKey(0);
    //        namedWindow("RGrayscaleEdge", WINDOW_NORMAL);
    //        imshow("RGrayscaleEdge", rGrayscaleEdge);
    //       namedWindow("TestG", WINDOW_NORMAL);
    //       imshow("TestG", gGrayscaleEdge);
    //       namedWindow("TestB", WINDOW_NORMAL);
    //       imshow("TestB", bGrayscaleEdge);
    //        waitKey(0);

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
            //What to do if max and/or are zero?
            //            if (min == 0) {
            //                min = 1;
            //            }
            //            if (max == 0) {
            //                max = 1;
            //            }
            if (!(min == 0 || max == 0)) {
                logarithmusValue = ((double) max) / ((double) min);
                sumR += log(logarithmusValue);
            } else {
                sumR += 1;
            }
            //... for green channel
            findMinMax(gGrayscaleEdge, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            // What to do if max and/or are zero?
            if (!(min == 0 || max == 0)) {
                logarithmusValue = ((double) max) / ((double) min);
                sumG += log(logarithmusValue);
            } else {
                sumG += 1;
            }
            //... for blue channel
            findMinMax(bGrayscaleEdge, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            // What to do if max and/or are zero?
            if (!(min == 0 || max == 0)) {
                logarithmusValue = ((double) max) / ((double) min);
                sumB += log(logarithmusValue);
            } else {
                sumB += 1;
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
    //    cout << result << endl;
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