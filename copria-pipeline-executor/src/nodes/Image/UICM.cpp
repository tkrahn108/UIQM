#include "node.h"
#include "variables/cvmat.h"
#include "variables/string.h"
#include "variables/numeric.h"
#include <list>
#include <math.h>
#include <cmath>

namespace copria {

    /**
     * Calculate the underwater image colorfulness measure (UICM)
     */
    class UICM : public Node {
    public:

        UICM(int id) : Node(id, "Image_UICM") {
            this->_i = {
                // The input image.
                new CVMatVariable(),
                // The alpha_L value (lower bound) for the asymmetric alpha-trimmed mean.
                new NumericVariable<float>(0.1),
                // The alpha_R value (upper bound) for the asymmetric alpha-trimmed mean.
                new NumericVariable<float>(0.1),
            };

            this->_input_is_set = {
                false,
                true,
                true,
            };

            this->_o = {
                // The UICM value.
                new NumericVariable<double>(),
            };
        }

        bool execute() {
            cv::Mat img = (((CVMatVariable*) this->i(0))->get());
	    float alphaL = (((NumericVariable<float>*) this->i(1))->get());
	    float alphaR = (((NumericVariable<float>*) this->i(2))->get());

            int nRows = img.rows;
            int nCols = img.cols;
            int K = nRows * nCols;

            //calculate RG and YB values for the whole image
            std::list<int> rgList;
            std::list<int> ybList;

            int r = 0, g = 0, b = 0;
            for (int i = 0; i < nRows; i++) {
                for (int j = 0; j < nCols; j++) {
                    b = img.at<cv::Vec3b>(i, j)[0];
                    g = img.at<cv::Vec3b>(i, j)[1];
                    r = img.at<cv::Vec3b>(i, j)[2];
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

            count = 0;
            for (std::list<int>::iterator it = ybList.begin(); it != ybList.end(); ++it) {
                if (count >= TalphaL && count < upperBound) {
                    meanYB += *it;
                }
                count++;
            }
            meanYB /= (float) (K - TalphaL - TalphaR);

            //calculate the variance
            float varianceRG = 0.0F;
            float varianceYB = 0.0F;

            //it is unclear whether the variance is also only calculated between T_alpha_L and T_alpha_R
            for (std::list<int>::iterator it = rgList.begin(); it != rgList.end(); ++it) {
                varianceRG += pow(*it - meanRG, 2);
            }
            varianceRG /= K;

            for (std::list<int>::iterator it = ybList.begin(); it != ybList.end(); ++it) {
                varianceYB += pow(*it - meanYB, 2);
            }
            varianceYB /= K;

            //calculate UICM
            double result = -0.0268 * sqrt(pow(meanRG, 2) + pow(meanYB, 2)) + 0.1586 * sqrt(varianceRG + varianceYB);
            ((NumericVariable<double>*) this->o(0))->set(result);
            return true;
        }
    };

    extern "C" {

        UICM* createUICM(int id) {
            return new UICM(id);
        }
    }
}
