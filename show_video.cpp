#include "show_video.h"
#include<iostream>

using namespace std;
using namespace cv;

struct PositionData
{
    int slider_position = 0;
    int run = 1, dontset = 0;
    cv::VideoCapture* pCap;
};

void onTrackbarSlide(int pos, void * userData) {
    PositionData* pData = static_cast<PositionData*>(userData);
    pData->pCap->set(cv::CAP_PROP_POS_FRAMES, pos);
    if (!pData->dontset)
        pData->run = 1;
    pData->dontset = 0;
}

int showVideo(cv::String path)
{
    cv::namedWindow("video", cv::WINDOW_AUTOSIZE);
    cv::VideoCapture capture(path);
    int frames = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
    int tmpw = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int tmph = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    float fps = (float)capture.get(cv::CAP_PROP_FPS);
    cout << "Video has " << frames << " frames of dimensions("
        << tmpw << ", " << tmph << ")." << "fps:" << fps << endl;
    struct PositionData userData;
    userData.pCap = &capture;
    cv::createTrackbar("Position", "video", &userData.slider_position, frames,
        onTrackbarSlide, &userData);
    cv::Mat frame;
    for (;;) {
        if (userData.run != 0) {
            capture >> frame;
            if (frame.empty())
                break;
            int current_pos = (int)capture.get(cv::CAP_PROP_POS_FRAMES);
            userData.dontset = 1;
            cv::setTrackbarPos("Position", "video", current_pos);
            cv::imshow("video", frame);
            userData.run -= 1;
        }
        char c = (char)cv::waitKey(1000 / fps);
        if (c == 's') // single step
        {
            userData.run = 1; cout << "Single step, run = " << userData.run << endl;
        }
        if (c == 'r') // run mode
        {
            userData.run = -1; cout << "Run mode, run = " << userData.run << endl;
        }
        if (c == 27)
            break;
    }
    cv::destroyWindow("video");
    capture.release();
    return 0;
}
