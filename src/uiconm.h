#ifndef UICONM_H
#define	UICONM_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Uiconm {
public:
    float calculate(Mat img);

private:
    float plipG(float intensity);
    float plipAddition(float g1, float g2);
    float plipSubtraction(float g1, float g2);
    float plipMultiplication(float c, float g);
    void findMinMaxIntensity(Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max);
};


#endif	/* UICONM_H */

