#include "uiconm.h"
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;
using namespace cv;

//PLIP parameters
#define PLIP_MU 1026.0F
#define PLIP_GAMMA 1026.0F
#define PLIP_K 1026.0F
#define PLIP_LAMBDA 1026.0F
#define PLIP_BETA 1.0F

//height and width for each quadratic block
#define BLOCKSIZE 4

/**
 * Calculates the UIConM (underwater image contrast measure) value according Panetta et al. 
 * @param img Image for which the value should be calculated
 * @return UIConM value
 */
double Uiconm::calculate(Mat img) {
    double result = 0.0;
    double tempResult = 0.0;

    int k1 = img.rows / BLOCKSIZE;
    int k2 = img.cols / BLOCKSIZE;
//    cout << "k1: " << k1 << endl;
//    cout << "k2: " << k2 << endl;
    //    double omin, omax;
    //    minMaxLoc(img, &omin, &omax);

    //    cout << "overallMin: " << omin << endl;
    //    cout << "overallMax: " << omax << endl;

    int min, max;

    for (int i = 1; i <= k1; i++) {
        for (int j = 1; j <= k2; j++) {
            findMinMaxIntensity(img, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
//            cout << "min: " << min << endl;
//            cout << "max: " << max << endl;
            if (!(min == max)) {
                double subtraction = plipSubtraction(plipG(max), plipG(min));
                double addition = plipAddition(plipG(max), plipG(min));
                tempResult = subtraction / addition;
//                cout << "subtraction: " << subtraction << endl;
//                cout << "addition: " << addition << endl;
//                cout << "tempResult: " << tempResult << endl; 
//                cout << "log(fabs(tempResult)): " << log(fabs(tempResult)) << endl;
//                cout << "plipMultiplication(plipG(tempResult),plipG(log(fabs(tempResult))))" << plipMultiplication(plipG(tempResult),plipG(log(fabs(tempResult)))) << endl << endl; 
                result += plipMultiplication(plipG(tempResult),plipG(log(fabs(tempResult))));      
            }
        }
    }
    double c = 1 / ((double) k1 * (double) k2);
//    cout << "c: " << c << endl;
    //        cout << "result before multiplication: " << result << endl;
//    cout << "plipG(result): " << plipG(result) << endl;
    //minus because of negative correlation without it
    result = plipScalarMultiplication(c, plipG(result));
    cout << "UIConM: " << result << endl;
    return result;
}

/**
 * PLIP operation g(i,j)
 * @param intensity Original image intensity
 * @return PLIP modified intensity value
 */
double Uiconm::plipG(double intensity) {
    return (PLIP_MU - intensity);
}

/**
 * PLIP addition of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified sum
 */
double Uiconm::plipAddition(double g1, double g2) {
    return (g1 + g2 - (g1 * g2) / PLIP_GAMMA);
}

/**
 * PLIP subtraction of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified difference
 */
double Uiconm::plipSubtraction(double g1, double g2) {
    return (PLIP_K * (g1 - g2) / (PLIP_K - g2));
}

/**
 * PLIP multiplication of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified multiplication 
 */
double Uiconm::plipScalarMultiplication(double c, double g) {
    return (PLIP_GAMMA - PLIP_GAMMA * pow(1 - g / PLIP_GAMMA, c));
}

double Uiconm::plipMultiplication(double g1, double g2){
    return plipPhiInverse(plipPhi(plipG(g1)) * plipPhi(plipG(g2)));
}

double Uiconm::plipPhi(double g){
    double result = 0.0;
    result = -PLIP_LAMBDA * pow(log(1 - g/PLIP_LAMBDA), PLIP_BETA); 
    return result;
}

double Uiconm::plipPhiInverse(double g){
    double result = 0.0;
    result = PLIP_LAMBDA * (1 - pow(exp(-g/PLIP_LAMBDA),1/PLIP_BETA));
    return result;
}

/**
 * Calculates the minimum and maximum intensity values within a specified area 
 * within the given image. For calculation of the intensity value the average
 * of all channel values is used: 1/3 * r + 1/3 * g + 1/3 * b.
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
void Uiconm::findMinMaxIntensity(Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max) {
    int tempMax = 0;
    int tempMin = INT_MAX;
    int tempValue = 0;
    Vec3b intensity;

    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            intensity = img.at<Vec3b>(j, i);
            //            uchar blue = intensity.val[0];
            //            uchar green = intensity.val[1];
            //            uchar red = intensity.val[2];
            //            cout << "Intensity: " << intensity << endl;
            tempValue = intensity.val[0] / 3 + intensity.val[1] / 3 + intensity.val[2] / 3;
            //                        cout << tempValue << ", ";
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
