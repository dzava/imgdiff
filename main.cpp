#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <getopt.h>
#include <json/json.h>
#include <errno.h>

using namespace std;
using namespace cv;

Mat resizeImage(Mat img, int width, int height);

Json::Value rectToJson(Rect rect);

Scalar hexToBGR(const char *value);

void parseOptions(int argc, char **argv);

struct Config {
    Scalar color = {0, 255, 0};

    char image1[PATH_MAX];
    char image2[PATH_MAX];
    char output[PATH_MAX];
    int outputBase = 2;
    bool json = true;

    bool writeDiffImage() {
        return strlen(output) > 0;
    }
};

Config config;

int main(int argc, char *argv[]) {

    parseOptions(argc, argv);

    Mat image1 = imread(config.image1);
    Mat image2 = imread(config.image2);

    if (image1.data == nullptr) {
        cerr << "Cannot open '" << config.image1 << "'" << endl;

        return 10;
    }
    if (image2.data == nullptr) {
        cerr << "Cannot open '" << config.image2 << "'" << endl;

        return 20;
    }

    image1 = resizeImage(image1, max(image1.size().width, image2.size().width),
                         max(image1.size().height, image2.size().height));
    image2 = resizeImage(image2, max(image1.size().width, image2.size().width),
                         max(image1.size().height, image2.size().height));

    Mat image1_gray;
    Mat image2_gray;

    cvtColor(image1, image1_gray, CV_BGR2GRAY);
    cvtColor(image2, image2_gray, CV_BGR2GRAY);


    Mat difference_image;
    Mat threshold_image;

    absdiff(image1_gray, image2_gray, difference_image);
    double threshold = cv::threshold(difference_image, threshold_image, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);


    vector <vector<Point>> contours;
    threshold_image = 255 - threshold_image;
    findContours(threshold_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    Mat output = (config.outputBase == 1 ? image1 : image2).clone();
    Json::Value differences = Json::arrayValue;

    for (int i = 0; i < contours.size(); i++) {
        Rect bbox = boundingRect(contours[i]);

        rectangle(output, bbox, config.color);
        differences[i] = rectToJson(bbox);
    }

    Json::Value root;

    root["image1"] = config.image1;
    root["image2"] = config.image2;
    root["threshold"] = threshold;
    root["differences"] = differences;

    if (config.writeDiffImage()) {
        if (!imwrite(config.output, output)) {
            cerr << "Could not save '" << config.output << "'" << endl;

            return 30;
        };

        root["output"] = config.output;
    }

    if (config.json) {
        cout << root << endl;
    }

    return 0;
}

Mat resizeImage(Mat img, int width, int height) {
    Mat temp_image(Size(width, height), img.type());
    temp_image = Scalar(0);
    img.copyTo(temp_image(Rect(0, 0, img.cols, img.rows)));

    return temp_image;
}

Json::Value rectToJson(Rect rect) {
    Json::Value result;
    result["x"] = rect.x;
    result["y"] = rect.y;
    result["width"] = rect.width;
    result["height"] = rect.height;

    return result;
}

Scalar hexToBGR(const char *value) {
    int r = 0, g = 0, b = 0;
    sscanf(value, "%02x%02x%02x", &r, &g, &b);

    return Scalar{b, g, r};
}

void usage() {
    cout << "Usage: " << program_invocation_short_name << " " <<
         "[-b base_image] [-c highlight_color] [-o output_path] image1 image2" << endl;
    cout << "Options: " << endl <<
         "\t-b [1|2] The image to use as base when generating a difference image" << endl <<
         "\t-c The hex value of the color to use for highlighting differences" << endl <<
         "\t-o The location where the difference image should be saved" << endl <<
         "\t-q Disable json output" << endl;
}

void parseOptions(int argc, char **argv) {

    int opt;
    while ((opt = getopt(argc, argv, "b:c:o:q")) != -1) {
        switch (opt) {
            case 'b':
                config.outputBase = atoi(optarg);
                break;
            case 'c':
                config.color = hexToBGR(optarg);
                break;
            case 'o':
                realpath(optarg, config.output);
                break;
            case 'q':
                config.json = false;
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    if (optind + 1 >= argc) {
        usage();
        exit(EXIT_FAILURE);
    }

    realpath(argv[optind], config.image1);
    realpath(argv[optind + 1], config.image2);
}