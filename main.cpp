#include <iostream>
#include <opencv2/opencv.hpp>
#include "show_video.h"
#include "graph_segmentation.h"

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

int example() {
    cv::Mat img_rgb;
    cv::namedWindow("Example Canny", cv::WINDOW_AUTOSIZE);
    img_rgb = cv::imread("./2.jpg");
    cv::pyrDown(img_rgb, img_rgb);
    cv::imshow("Example Canny", img_rgb);
    /// Separate the image in 3 places ( B, G and R )
    vector<Mat> bgr_planes;
    split(img_rgb, bgr_planes);
    /// Establish the number of bins
    int histSize = 256;
    /// Set the ranges ( for B,G,R) )
    const int channels = 0;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;
    Mat b_hist, g_hist, r_hist;
    /// Compute the histograms:
    cv::calcHist(&bgr_planes[0], 1, &channels, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, &channels, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, &channels, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    /// Draw for each channel
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
            Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
            Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
            Scalar(0, 0, 255), 2, 8, 0);
    }
    /// Display
    namedWindow("calcHist Demo", cv::WINDOW_AUTOSIZE);
    imshow("calcHist Demo", histImage);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

int segmentImage(float sigma, float k, float min, String& inputfile, String& output) {
    cv::Mat image = imread(inputfile);
    if (image.empty())
    {
        cout << "can't read input file:" << inputfile << endl;
        return -1;
    }
    image.convertTo(image, CV_32FC3);
    int kSize = 2 * (int)ceil(sigma * 2) + 1;
    GaussianBlur(image, image, Size(kSize, kSize), sigma);
    Ptr<GraphSegmentationMagic> magic = new GraphSegmentationMagicThreshold(k);
    Ptr<GraphSegmentationDistance> distance = new GraphSegmentationEuclideanRGB();
    GraphSegmentation segmentation;
    segmentation.setMagic(magic);
    segmentation.setDistance(distance);
    segmentation.buildGraph(image);
    segmentation.oversegmentGraph();
    segmentation.enforceMinimumSegmentSize(min);
    cv::Mat labels = segmentation.deriveLabels();
    double maxL;
    minMaxIdx(labels, NULL, &maxL);
    float ratio = 255 / maxL;
    labels.convertTo(labels, CV_8UC1, ratio);
    cv::Mat outImage(image.size(), CV_8UC3);
    applyColorMap(labels, outImage, COLORMAP_JET);
    cv::imwrite(output, outImage);
    return 0;
}

int main(int argc, char *argv[]) {
    CommandLineParser parser(
        argc, argv,
        { "{help h usage ? |   | show help infos}"
        "{showImage|   | show image}"
        "{showVideo|   | show video}"
        "{imgSeg||do image segmentation}"
        "{sigma|0.5|(imgSeg)Used to smooth the input image before segmenting it}"
        "{k|500|(imgSeg)Value for the threshold function}"
        "{min|20|(imgSeg)Minimum component size enforced by post-processing}"
        "{output|output.png|Output image}"
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
    if (parser.has("imgSeg"))
    {
        float sigma = parser.get<float>("sigma");
        float k = parser.get<float>("k");
        float min = parser.get<float>("min");
        String output = parser.get<String>("output");
        String inputfile = parser.get<String>(0);
        if (!parser.check())
        {
            cout << "cvtool --imgSeg [--sigma=] [--k=] [--min=] [--output=] input" << endl;
            return -1;
        }
        return segmentImage(sigma, k, min, inputfile, output);
    }
    return 0;
}