#ifndef UICONM_H
#define	UICONM_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Uiconm {
public:
    double calculate(Mat img);

private:
    double plipG(double intensity);
    double plipAddition(double g1, double g2);
    double plipSubtraction(double g1, double g2);
    double plipScalarMultiplication(double c, double g);
    double plipMultiplication(double g1, double g2);
    double plipPhi(double g);
    double plipPhiInverse(double g);
    void findMinMaxIntensity(Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max);
};


#endif	/* UICONM_H */

