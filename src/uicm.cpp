#include "uicm.h"
#include <opencv2/opencv.hpp>
#include <list>
#include <math.h>
#include <cmath>

using namespace std;
using namespace cv;

float Uicm::calculate(Mat img, float alphaL, float alphaR) {
    int nRows = img.rows;
    int nCols = img.cols;
    int K = nRows * nCols;
//    cout << "nRows: " << nRows << endl;
//    cout << "nCols: " << nCols << endl;
    
    //calculate RG and YB values for the whole image
    std::list<int> rgList;
    std::list<int> ybList;

    int r = 0, g = 0, b = 0;
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            b = img.at<Vec3b>(i, j)[0];
            g = img.at<Vec3b>(i, j)[1];
            r = img.at<Vec3b>(i, j)[2];
            rgList.push_back(r - g);
            ybList.push_back((int) ((r + g) / 2 - b));
        }
    }

    /* it would be better to sort the list when we add new elements (insertion sort),
     * as this way to sort is very inefficient. Or don't we need it any more?
     */
    rgList.sort();
    ybList.sort();

    
    //calculate the mean values for RG and YB
    int TalphaL = (int) (ceil(K * alphaL));
    int TalphaR = (int) (floor(K * alphaR));

//    cout << "TalphaL: " << TalphaL << endl;
//    cout << "TalphaR: " << TalphaR << endl;

    float meanRG = 0.0F;
    float meanYB = 0.0F;

    int count = 0;
    int upperBound = K - TalphaR;

    for (std::list<int>::iterator it = rgList.begin(); it != rgList.end(); ++it) {
        if (count >= TalphaL && count < upperBound) {
            meanRG += *it;
        }
        count++;
    }
    meanRG /= (float) (K - TalphaL - TalphaR);
//    cout << "meanRG: " << meanRG << endl;

    count = 0;
    for (std::list<int>::iterator it = ybList.begin(); it != ybList.end(); ++it) {
        if (count >= TalphaL && count < upperBound) {
            meanYB += *it;
        }
        count++;
    }
    meanYB /= (float) (K - TalphaL - TalphaR);
//    cout << "meanYB: " << meanYB << endl;


    //calculate the variance
    float varianceRG = 0.0F;
    float varianceYB = 0.0F;

    //it is unclear whether the variance is also only calculated between T_alpha_L and T_alpha_R
    for (std::list<int>::iterator it = rgList.begin(); it != rgList.end(); ++it) {
        varianceRG += pow(*it - meanRG, 2);
    }
    varianceRG /= K;
//    cout << "Variance RG: " << varianceRG << endl;

    for (std::list<int>::iterator it = ybList.begin(); it != ybList.end(); ++it) {
        varianceYB += pow(*it - meanYB, 2);
    }
    varianceYB /= K;
//    cout << "Variance YB: " << varianceYB << endl;

    //calculate UICM
    float result = -0.0268 * sqrt(pow(meanRG, 2) + pow(meanYB, 2)) + 0.1586 * sqrt(varianceRG + varianceYB);
//    cout << "UICM: " << result << endl;
    return result;
}