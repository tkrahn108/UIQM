#include "node.h"
#include "variables/cvmat.h"
#include "variables/string.h"
#include "variables/numeric.h"

//coefficents for the addition of channel values; this are default values
#define LAMBDA_R 0.299F
#define LAMBDA_G 0.587F
#define LAMBDA_B 0.114F

namespace copria {

    /**
     * Calculate the underwater image sharpness measure.
     */
    class UISM : public Node {
    public:

        UISM(int id) : Node(id, "Image_UISM") {
            this->_i = {
                // The input image.
                new CVMatVariable(),
                // The size (number of pixel) per block.
                new NumericVariable<int>(4),
            };

            this->_input_is_set = {
                false,
                true,
            };

            this->_o = {
                // The UISM value.
                new NumericVariable<double>(),
            };
        }

        bool execute() {
            cv::Mat img = (((CVMatVariable*) this->i(0))->get());
            int blocksize = (((NumericVariable<int>*) this->i(1))->get());
            cv::Mat rChannelImg, gChannelImg, bChannelImg;
            std::vector<cv::Mat> channels(3);
            split(img, channels);

            bChannelImg = channels[0];
            gChannelImg = channels[1];
            rChannelImg = channels[2];


            // Generate grad_x and grad_y
            cv::Mat grad_x, grad_y;
            cv::Mat abs_grad_x, abs_grad_y;
            cv::Mat rSobelImg, gSobelImg, bSobelImg;
            //scale factor
            int scale = 1;
            int delta = 0;
            //output image depth
            int ddepth = CV_16S;

            //Gradient X for r-channel
            Sobel(rChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_x, abs_grad_x);
            //Gradient Y for r-channel
            Sobel(rChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_y, abs_grad_y);
            abs_grad_x.convertTo(abs_grad_x, CV_32F);
            abs_grad_y.convertTo(abs_grad_y, CV_32F);
            //magnitude = sqrt(abs_grad_x^2 + abs_grad_y^2);
            magnitude(abs_grad_x, abs_grad_y, rSobelImg);

            //Gradient X for g-channel
            Sobel(gChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_x, abs_grad_x);
            //Gradient Y for g-channel
            Sobel(gChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_y, abs_grad_y);
            abs_grad_x.convertTo(abs_grad_x, CV_32F);
            abs_grad_y.convertTo(abs_grad_y, CV_32F);
            magnitude(abs_grad_x, abs_grad_y, gSobelImg);

            //Gradient X for b-channel
            Sobel(bChannelImg, grad_x, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_x, abs_grad_x);
            //Gradient Y for b-channel
            Sobel(bChannelImg, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_y, abs_grad_y);
            abs_grad_x.convertTo(abs_grad_x, CV_32F);
            abs_grad_y.convertTo(abs_grad_y, CV_32F);
            magnitude(abs_grad_x, abs_grad_y, bSobelImg);

            //grayscale edge maps which results from multiplication with the original image
            cv::Mat rGrayscaleEdge, gGrayscaleEdge, bGrayscaleEdge;

            double minVal = 0;
            double maxVal = 0;

            rChannelImg.convertTo(rChannelImg, CV_32F);
            gChannelImg.convertTo(gChannelImg, CV_32F);
            bChannelImg.convertTo(bChannelImg, CV_32F);

            rGrayscaleEdge = rChannelImg.mul(rSobelImg);
            gGrayscaleEdge = gChannelImg.mul(gSobelImg);
            bGrayscaleEdge = bChannelImg.mul(bSobelImg);

            cv::minMaxLoc(rGrayscaleEdge, &minVal, &maxVal);
            rGrayscaleEdge.convertTo(rGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);
            cv::minMaxLoc(gGrayscaleEdge, &minVal, &maxVal);
            gGrayscaleEdge.convertTo(gGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);
            cv::minMaxLoc(bGrayscaleEdge, &minVal, &maxVal);
            bGrayscaleEdge.convertTo(bGrayscaleEdge, CV_8U, 255.0 / (maxVal - minVal), -minVal);

            //EME ...
            int k1 = img.rows / blocksize;
            int k2 = img.cols / blocksize;
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            double logarithmusValue;

            int min, max;

            for (int i = 1; i <= k1; i++) {
                for (int j = 1; j <= k2; j++) {
                    //... for red-channel
                    findMinMax(rGrayscaleEdge, (j - 1) * blocksize, j * blocksize - 1, (i - 1) * blocksize, i * blocksize - 1, min, max);
                    if (!(min == 0 || max == 0)) {
                        logarithmusValue = ((double) max) / ((double) min);
                        sumR += std::log(logarithmusValue);
                    } else {
                        sumR += 1;
                    }
                    //... for green channel
                    findMinMax(gGrayscaleEdge, (j - 1) * blocksize, j * blocksize - 1, (i - 1) * blocksize, i * blocksize - 1, min, max);
                    if (!(min == 0 || max == 0)) {
                        logarithmusValue = ((double) max) / ((double) min);
                        sumG += std::log(logarithmusValue);
                    } else {
                        sumG += 1;
                    }
                    //... for blue channel
                    findMinMax(bGrayscaleEdge, (j - 1) * blocksize, j * blocksize - 1, (i - 1) * blocksize, i * blocksize - 1, min, max);
                    if (!(min == 0 || max == 0)) {
                        logarithmusValue = ((double) max) / ((double) min);
                        sumB += std::log(logarithmusValue);
                    } else {
                        sumB += 1;
                    }
                }
            }
            sumR *= 2.0 / (k1 * k2);
            sumG *= 2.0 / (k1 * k2);
            sumB *= 2.0 / (k1 * k2);

            double result = LAMBDA_R * sumR + LAMBDA_G * sumG + LAMBDA_B * sumB;
            ((NumericVariable<double>*) this->o(0))->set(result);
            return true;
        }

        void findMinMax(cv::Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max) {
            int tempMax = 0;
            int tempMin = INT_MAX;
            int tempValue = 0;
            cv::Scalar intensity;

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


    };

    extern "C" {

        UISM* createUISM(int id) {
            return new UISM(id);
        }
    }
}
