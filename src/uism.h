#ifndef UISM_H
#define	UISM_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Uism {
public:
    float calculate(Mat img);

private:
    void findMinMax(Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max);
};

#endif	/* UISM_H */

