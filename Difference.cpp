#include"Difference.h"

Mat equalizeIntensity(const Mat& inputImage)
{
    if (inputImage.channels() >= 3)
    {
        vector<Mat> channels;
        split(inputImage, channels);

        equalizeHist(channels[0], channels[0]);
        equalizeHist(channels[1], channels[1]);
        equalizeHist(channels[2], channels[2]);

        try {
            Mat result;
            merge(channels, result);

            return result;
        }
        catch (Exception e) {
            return channels[0];
        }
    }
    else if (inputImage.channels() == 1) {
        Mat result;
        equalizeHist(inputImage, result);
        return result;
    }
    return Mat();
}

void do1ChnHist(const Mat_<uchar>& img, const Mat_<uchar>& mask, Mat_<double>& h, Mat_<double>& cdf)
{
    for (size_t p = 0; p < img.total(); p++)
    {
        if (mask(p) > 0)
        {
            uchar c = img(p);
            h(c) += 1.0;
        }
    }

    normalize(h, h, 1, 0, NORM_MINMAX);

    cdf(0) = h(0);
    for (int j = 1; j < 256; j++)
    {
        cdf(j) = cdf(j - 1) + h(j);
    }

    normalize(cdf, cdf, 1, 0, NORM_MINMAX);
}

// match histograms of 'src' to that of 'dst', according to both masks
void histMatchRGB(Mat& src, const Mat& src_mask, const Mat& dst, const Mat& dst_mask)
{
    if (src.channels() == 1 && dst.channels() == 1) {
        Mat_<uchar> chns, chns1;

        src.copyTo(chns);
        dst.copyTo(chns1);

        Mat_<double> src_hist = Mat_<double>::zeros(1, 256);
        Mat_<double> dst_hist = Mat_<double>::zeros(1, 256);
        Mat_<double> src_cdf = Mat_<double>::zeros(1, 256);
        Mat_<double> dst_cdf = Mat_<double>::zeros(1, 256);

        do1ChnHist(chns, src_mask, src_hist, src_cdf);
        do1ChnHist(chns1, dst_mask, dst_hist, dst_cdf);

        uchar last = 0;

        Mat_<uchar> lut(1, 256);
        for (int j = 0; j < src_cdf.cols; j++)
        {
            double F1j = src_cdf(j);

            for (uchar k = last; k < dst_cdf.cols; k++)
            {
                double F2k = dst_cdf(k);
                if (abs(F2k - F1j) < HISTMATCH_EPSILON || F2k > F1j)
                {
                    lut(j) = k;
                    last = k;
                    break;
                }
            }
        }

        Mat res;
        LUT(chns, lut, res);

        res.copyTo(src);
    }
    else if (src.channels() == 3 && dst.channels() == 3) {
        vector<Mat_<uchar>> chns, chns1;
        split(src, chns);
        split(dst, chns1);

        for (int i = 0; i < 3; i++)
        {
            Mat_<double> src_hist = Mat_<double>::zeros(1, 256);
            Mat_<double> dst_hist = Mat_<double>::zeros(1, 256);
            Mat_<double> src_cdf = Mat_<double>::zeros(1, 256);
            Mat_<double> dst_cdf = Mat_<double>::zeros(1, 256);

            do1ChnHist(chns[i], src_mask, src_hist, src_cdf);
            do1ChnHist(chns1[i], dst_mask, dst_hist, dst_cdf);

            uchar last = 0;

            Mat_<uchar> lut(1, 256);
            for (int j = 0; j < src_cdf.cols; j++)
            {
                double F1j = src_cdf(j);

                for (uchar k = last; k < dst_cdf.cols; k++)
                {
                    double F2k = dst_cdf(k);
                    if (abs(F2k - F1j) < HISTMATCH_EPSILON || F2k > F1j)
                    {
                        lut(j) = k;
                        last = k;
                        break;
                    }
                }
            }

            LUT(chns[i], lut, chns[i]);
        }

        Mat res;
        merge(chns, res);

        res.copyTo(src);
    }
}

Mat Difference(Mat& first, Mat& second, bool hist_norm = false, bool smoothing = false, int smooth_size = 5) {
    if (first.empty() || second.empty()) return Mat();

    if (first.channels() == 1 && second.channels() == 1) {
        int max_cols = max(first.cols, second.cols);
        int max_rows = max(first.rows, second.rows);

        int min_cols = min(first.cols, second.cols);
        int min_rows = min(first.rows, second.rows);

        if (hist_norm) {
            Mat mask = Mat(first.size(), CV_8U, Scalar(255));
            histMatchRGB(second, mask, first, mask);
        }

        Mat diffImage = Mat::zeros(max_rows, max_cols, CV_8UC1);

        for (int j = 0; j < min_rows; ++j) {
            for (int i = 0; i < min_cols; ++i) {
                uchar pix_first = first.at<uchar>(j, i);
                uchar pix_second = second.at<uchar>(j, i);

                if (pix_first == 0 || pix_second == 0) continue;

                uchar d;
                d = abs(pix_first - pix_second);
                diffImage.at<uchar>(j, i) = d;
            }
        }

        if (smoothing) {
            medianBlur(diffImage, diffImage, smooth_size);
            //GaussianBlur(diffImage, diffImage, Size(smooth_size, smooth_size), 0, 0);
        }

        return diffImage;

    }
    else if (first.channels() == 3 && second.channels() == 3) {
        int max_cols = max(first.cols, second.cols);
        int max_rows = max(first.rows, second.rows);

        int min_cols = min(first.cols, second.cols);
        int min_rows = min(first.rows, second.rows);

        if (hist_norm) {
            Mat mask = Mat(first.size(), CV_8U, Scalar(255));
            histMatchRGB(second, mask, first, mask);
        }

        Mat diffImage(max_rows, max_cols, CV_8UC3, Scalar(0, 0, 0));


        for (int j = 0; j < min_rows; ++j) {
            for (int i = 0; i < min_cols; ++i) {
                Vec3b pix_first = first.at<Vec3b>(j, i);
                Vec3b pix_second = second.at<Vec3b>(j, i);

                if ((pix_first[0] == 0 && pix_first[1] == 0 && pix_first[2] == 0) || (pix_second[0] == 0 && pix_second[1] == 0 && pix_second[2] == 0)) continue;

                Vec3b d;
                d[0] = abs(pix_first[0] - pix_second[0]);
                d[1] = abs(pix_first[1] - pix_second[1]);
                d[2] = abs(pix_first[2] - pix_second[2]);
                diffImage.at<Vec3b>(j, i) = d;
            }
        }

        if (smoothing) {
            medianBlur(diffImage, diffImage, smooth_size);
            //GaussianBlur(diffImage, diffImage, Size(smooth_size, smooth_size), 0, 0);
        }

        return diffImage;
    }

    return Mat();
}



int DifferenceCorrelation(const Mat& first, Mat& second, int kernel_size = 10) {
    int max_cols = max(first.cols, second.cols);
    int max_rows = max(first.rows, second.rows);

    int min_cols = min(first.cols, second.cols);
    int min_rows = min(first.rows, second.rows);

    for (int j = 1; j < min_rows - kernel_size; j += kernel_size) {
        for (int i = 1; i < min_cols - kernel_size; i += kernel_size) {

            Mat result_part;
            Mat part_of_first = first(Rect(i, j, kernel_size, kernel_size));
            Mat part_of_second = second(Rect(i, j, kernel_size, kernel_size));

            Mat dif_image = Difference(part_of_first, part_of_second, false, false, 5);

            int min_dist = 0;
            int x_c = i;
            int y_c = j;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    min_dist += dif_image.at<uchar>(l, k);
                }
            }

            if (min_dist == 0) continue;

            part_of_second.copyTo(result_part);

            int tmp_sum;

            //Сдвиг вверх
            part_of_second = second(Rect(i, j + 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вниз
            part_of_second = second(Rect(i, j - 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вправо
            part_of_second = second(Rect(i + 1, j, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }


            //Сдвиг влево
            part_of_second = second(Rect(i - 1, j, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вверх-вправо
            part_of_second = second(Rect(i + 1, j + 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вверх-влево
            part_of_second = second(Rect(i - 1, j + 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вниз-влево
            part_of_second = second(Rect(i - 1, j - 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }

            //Сдвиг вниз-вправо
            part_of_second = second(Rect(i + 1, j - 1, kernel_size, kernel_size));
            dif_image = Difference(part_of_first, part_of_second, false, false, 5);
            tmp_sum = 0;
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    tmp_sum += dif_image.at<uchar>(l, k);
                }
            }
            if (tmp_sum < min_dist) {
                part_of_second.copyTo(result_part);
                min_dist = tmp_sum;
            }


            //Присвоение
            for (int k = 0; k < kernel_size; k++) {
                for (int l = 0; l < kernel_size; l++) {
                    second.at<uchar>(j + k, i + l) = result_part.at<uchar>(k, l);
                }
            }
        }
    }

    return 0;
}


//cv::Mat diffImage;
//cv::absdiff(first, second, diffImage);
//
//cv::Mat foregroundMask = cv::Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);
//
//float threshold = 30.0f;
//float dist;
//
//for (int j = 0; j < diffImage.rows; ++j)
//    for (int i = 0; i < diffImage.cols; ++i)
//    {
//        cv::Vec3b pix = diffImage.at<cv::Vec3b>(j, i);
//
//        dist = (pix[0] * pix[0] + pix[1] * pix[1] + pix[2] * pix[2]);
//        dist = sqrt(dist);
//
//        if (dist > threshold)
//        {
//            foregroundMask.at<unsigned char>(j, i) = 255;
//        }
//    }
//return diffImage;