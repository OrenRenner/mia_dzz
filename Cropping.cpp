#include"Cropping.h"

using namespace cv;
using namespace std;


Cropping::Cropping(int width, int height, double overlapCoef) {
	if (overlapCoef > 1.0 or overlapCoef < 0.0) {
		throw std::invalid_argument("Please set overlapCoef in [0, 1] interval");
	}

	this->height = height;
	this->width = width;
	this->overlapCoef = overlapCoef;
	this->x = 0;
	this->y = 0;
}

Mat Cropping::getNextCrop(cv::Mat& original, cv::Point& shift) {
	if (this->y > original.rows - this->height || this->height > original.rows || this->width > original.cols) {
		Mat mat;
		return mat;
	}

	shift.x = this->x;
	shift.y = this->y;

	Rect grid_rect(x, y, this->width, this->height);
	this->x += this->width;

	if (this->x - this->overlapCoef * this->width > 0) {
		if (this->width - this->overlapCoef * this->width < 1) this->x += -this->width + 1;
		else this->x -= (int)round(this->overlapCoef * this->width);
	}

	if (this->x > original.cols - this->width) {
		this->x = 0;
		this->y += this->height;
		if (this->y - this->overlapCoef * this->height > 0) {
			if (this->height - this->overlapCoef * this->height < 1) this->y += -this->height + 1;
			else this->y -= (int)round(this->overlapCoef * this->height);
		}
	}

	return original(grid_rect);
}

