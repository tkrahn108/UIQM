#ifndef UICM_H
#define	UICM_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Uicm{
public:
    float calculate (Mat img, float alphaL, float alphaR); 
    
};


#endif	/* UICM_H */

