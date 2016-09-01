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

//height and width for each quadratic block
#define BLOCKSIZE 4

/**
 * Calculates the UIConM (underwater image contrast measure) value according Panetta et al. 
 * @param img Image for which the value should be calculated
 * @return UIConM value
 */
float Uiconm::calculate(Mat img) {
    float result = 0.0F;
    float tempResult = 0.0F;

    int k1 = img.rows / BLOCKSIZE;
    int k2 = img.cols / BLOCKSIZE;

    int min, max;

    for (int i = 1; i <= k1; i++) {
        for (int j = 1; j <= k2; j++) {
            findMinMaxIntensity(img, (j - 1) * BLOCKSIZE, j * BLOCKSIZE - 1, (i - 1) * BLOCKSIZE, i * BLOCKSIZE - 1, min, max);
            if (!(min == 0)) {
                tempResult = plipSubtraction(plipG(max), plipG(min)) / plipAddition(plipG(max), plipG(min));
//                cout << "Temp result: " << tempResult << endl;
                result += tempResult * log(fabs(tempResult));
//                cout << "Result: " << result << endl << endl ;
            }
        }
    }
    float c = 1 / ((float) k1 * (float) k2);
    cout << "c: " << c << endl;
    cout << "result before multiplication: " << result << endl;
    cout << "result plipG: " << plipG(result) << endl;
    result = plipMultiplication(c, plipG(result));
    cout << "UIConM: " << result << endl;
    return result;
}

/**
 * PLIP operation g(i,j)
 * @param intensity Original image intensity
 * @return PLIP modified intensity value
 */
float Uiconm::plipG(float intensity) {
    return (PLIP_MU - intensity);
}

/**
 * PLIP addition of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified sum
 */
float Uiconm::plipAddition(float g1, float g2) {
    return (g1 + g2 - (g1 * g2) / PLIP_GAMMA);
}

/**
 * PLIP subtraction of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified difference
 */
float Uiconm::plipSubtraction(float g1, float g2) {
    return (PLIP_K * (g1 - g2) / (PLIP_K - g2));
}

/**
 * PLIP multiplication of two gray tone functions g_1 and g_2
 * @param g1 Gray tone function 1
 * @param g2 Gray tone function 2
 * @return PLIP modified multiplication 
 */
float Uiconm::plipMultiplication(float c, float g) {
    return (PLIP_GAMMA - PLIP_GAMMA * pow(1 - g / PLIP_GAMMA, c));
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
    Scalar intensity;

    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            intensity = img.at<uchar>(j, i);
            tempValue = intensity.val[0] / 3 + intensity.val[1] / 3 + intensity.val[2] / 3;
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
