#include <iostream>
#include <opencv2/opencv.hpp>
#include "show_video.h"

using namespace std;
using namespace cv;

int showImage(String path) {
    cv::Mat img = cv::imread(path, -1);
    if (img.empty()) {
        cout << "can't open file:" << path << endl;
        return -1;
    }
    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Image", img);
    cv::waitKey(0);
    cv::destroyWindow("Image");
    return 0;
}

int main(int argc, char *argv[]) {
    CommandLineParser parser(
        argc, argv,
        { "{help h usage ? |   | show help infos}"
        "{showImage|   | show image}"
        "{showVideo|   | show video}"
        "{@input0||first input}"
        });
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    if (parser.has("showImage")) {
        String imageFile = parser.get<String>(0);
        if (!parser.check())
        {
            parser.printErrors();
            return -1;
        }
        return showImage(imageFile);
    }
    if (parser.has("showVideo")) {
        String filePath = parser.get<String>(0);
        if (!parser.check())
        {
            parser.printErrors();
            return -1;
        }
        return showVideo(filePath);
    }
    return 0;
}