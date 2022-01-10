#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>


class Cropping {
private:
	int width, height, y, x;
	double overlapCoef;
public:
	Cropping(int width, int height, double overlapCoef);
	cv::Mat getNextCrop(cv::Mat& original, cv::Point& shift);
};