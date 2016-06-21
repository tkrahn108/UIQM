#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>

#include "uicm.h"
#include "uism.h"
#include "uiconm.h"

//boundaries for calculation of the means in the UICM term
#define ALPHAR 0.1F
#define ALPHAL 0.1F

using namespace cv;
using namespace std;

/**
 * Checks whether a given string has a specific ending.
 * 
 * @param fullString The string that will be checked
 * @param ending The ending that will be looked for
 * @return True if the ending appears at the end of the provided string, otherwise false
 */
bool hasEnding(const string &fullString, const string &ending);


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: UIQM <Image_Directory>\n");
        return -1;
    }
    
    //declares three object for calculation of the three terms
    Uicm uicm;
    Uism uism;
    Uiconm uiconm;
    
    // Loads all image files from the given directory.
    DIR *dir;
    struct dirent *ent;

    printf("Image path: %s\n", argv[1]);
    if ((dir = opendir(argv[1])) != NULL) {
        string imgDir = argv[1];
        if (!hasEnding(imgDir, "/")) imgDir += "/";
        while ((ent = readdir(dir)) != NULL) {
            Mat image;
            string imgPath(imgDir + string(ent->d_name));
            if (hasEnding(imgPath, ".jpg") || hasEnding(imgPath, ".JPG") ||\
                hasEnding(imgPath, ".png") || hasEnding(imgPath, ".PNG") ||\
                hasEnding(imgPath, ".jpeg") || hasEnding(imgPath, ".JPEG")) {
                image = imread(imgPath, 1);

                if (!image.data) {
                    printf("No image data in file %s \n", imgPath.c_str());
                } else {
                    printf("Successfully loaded image:  %s \n", imgPath.c_str());
                    float sum = 0;
                    sum += uicm.calculate(image, ALPHAL, ALPHAR);
//                    sum += uism.calculate(image);
                    sum += uiconm.calculate(image);
                    cout << "The value for image " <<  imgPath.c_str() << " is " << sum << endl << endl;
                    
                }
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        printf("Error: Could not open directory");
        perror("");
        return 1;
    }


    return 0;
}


bool hasEnding(const string &fullString, const string &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    return false;
}

