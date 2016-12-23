#include "node.h"
#include "variables/cvmat.h"
#include "variables/string.h"
#include "variables/numeric.h"
#include <math.h>

namespace copria {

    /**
     * Calculate the underwater image contrast measure (UIConM)
     */
    class UIConM : public Node {
    public:

        float PLIP_MU, PLIP_GAMMA, PLIP_K, PLIP_LAMBDA, PLIP_BETA;

        UIConM(int id) : Node(id, "Image_UIConM") {
            this->_i = {
                // The input image.
                new CVMatVariable(),
                // The size (number of pixel) per block.
                new NumericVariable<int>(4),
                // PLIP parameter mu.
                new NumericVariable<float>(1026.0),
                // PLIP parameter gamma.
                new NumericVariable<float>(1026.0),
                // PLIP parameter k.
                new NumericVariable<float>(1026.0),
                // PLIP parameter lamda.
                new NumericVariable<float>(1026.0),
                // PLIP parameter beta.
                new NumericVariable<float>(1.0),
            };

            this->_input_is_set = {
                false,
                true,
                true,
                true,
                true,
                true,
                true,
            };

            this->_o = {
                // The UIConM value.
                new NumericVariable<double>(),
            };
        }

        bool execute() {
            cv::Mat img = (((CVMatVariable*) this->i(0))->get());
            int blocksize = (((NumericVariable<int>*) this->i(1))->get());
            PLIP_MU = (((NumericVariable<float>*) this->i(2))->get());
            PLIP_GAMMA = (((NumericVariable<float>*) this->i(3))->get());
            PLIP_K = (((NumericVariable<float>*) this->i(4))->get());
            PLIP_LAMBDA = (((NumericVariable<float>*) this->i(5))->get());
            PLIP_BETA = (((NumericVariable<float>*) this->i(6))->get());

            double result = 0.0;
            double tempResult = 0.0;

            int k1 = img.rows / blocksize;
            int k2 = img.cols / blocksize;
            int min, max;

            for (int i = 1; i <= k1; i++) {
                for (int j = 1; j <= k2; j++) {
                    findMinMaxIntensity(img, (j - 1) * blocksize, j * blocksize - 1, (i - 1) * blocksize, i * blocksize - 1, min, max);
                    if (!(min == max)) {
                        double subtraction = plipSubtraction(plipG(max), plipG(min));
                        double addition = plipAddition(plipG(max), plipG(min));
                        tempResult = subtraction / addition;
                        result += plipMultiplication(plipG(tempResult), plipG(std::log(fabs(tempResult))));
                    }
                }
            }
            double c = 1 / ((double) k1 * (double) k2);
            //    cout << "c: " << c << endl;
            //        cout << "result before multiplication: " << result << endl;
            //    cout << "plipG(result): " << plipG(result) << endl;
            //minus because of negative correlation without it
            result = plipScalarMultiplication(c, plipG(result));
            ((NumericVariable<double>*) this->o(0))->set(result);
            return true;
        }

        /**
         * PLIP operation g(i,j)
         * @param intensity Original image intensity
         * @return PLIP modified intensity value
         */
        double plipG(double intensity) {
            return (PLIP_MU - intensity);
        }

        /**
         * PLIP addition of two gray tone functions g_1 and g_2
         * @param g1 Gray tone function 1
         * @param g2 Gray tone function 2
         * @return PLIP modified sum
         */
        double plipAddition(double g1, double g2) {
            return (g1 + g2 - (g1 * g2) / PLIP_GAMMA);
        }

        /**
         * PLIP subtraction of two gray tone functions g_1 and g_2
         * @param g1 Gray tone function 1
         * @param g2 Gray tone function 2
         * @return PLIP modified difference
         */
        double plipSubtraction(double g1, double g2) {
            return (PLIP_K * (g1 - g2) / (PLIP_K - g2));
        }

        /**
         * PLIP multiplication of two gray tone functions g_1 and g_2
         * @param g1 Gray tone function 1
         * @param g2 Gray tone function 2
         * @return PLIP modified multiplication 
         */
        double plipScalarMultiplication(double c, double g) {
            return (PLIP_GAMMA - PLIP_GAMMA * pow(1 - g / PLIP_GAMMA, c));
        }

        double plipMultiplication(double g1, double g2) {
            return plipPhiInverse(plipPhi(plipG(g1)) * plipPhi(plipG(g2)));
        }

        double plipPhi(double g) {
            double result = 0.0;
            result = -PLIP_LAMBDA * pow(std::log(1 - g / PLIP_LAMBDA), PLIP_BETA);
            return result;
        }

        double plipPhiInverse(double g) {
            double result = 0.0;
            result = PLIP_LAMBDA * (1 - pow(exp(-g / PLIP_LAMBDA), 1 / PLIP_BETA));
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
        void findMinMaxIntensity(cv::Mat img, int xmin, int xmax, int ymin, int ymax, int& min, int& max) {
            int tempMax = 0;
            int tempMin = INT_MAX;
            int tempValue = 0;
            cv::Vec3b intensity;

            for (int i = xmin; i <= xmax; i++) {
                for (int j = ymin; j <= ymax; j++) {
                    intensity = img.at<cv::Vec3b>(j, i);
                    tempValue = intensity.val[0] / 3 + intensity.val[1] / 3 + intensity.val[2] / 3;
                    if (tempValue < tempMin) {
                        tempMin = tempValue;
                    }
                    if (tempValue > tempMax) {
                        tempMax = tempValue;
                    }
                }
            }
            min = tempMin;
            max = tempMax;
        }
    };

    extern "C" {

        UIConM* createUIConM(int id) {
            return new UIConM(id);
        }
    }
}
