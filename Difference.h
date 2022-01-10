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


#define HISTMATCH_EPSILON 0.000001

using namespace cv;
using namespace std;

Mat Difference(Mat& first, Mat& second, bool hist_norm, bool smoothing, int smooth_size);
Mat equalizeIntensity(const Mat& inputImage);
void histMatchRGB(Mat& src, const Mat& src_mask, const Mat& dst, const Mat& dst_mask);
int DifferenceCorrelation(const Mat& first, Mat& second, int kernel_size);