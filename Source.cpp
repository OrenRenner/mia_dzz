#include"Difference.h"
#include<math.h> 
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Please input arguments: programm.exe first_image_path second_image_path save_path";
        return -1;
    }

    string first_image = argv[1];
    string second_image = argv[2];
    string save_path = argv[3];

    if (save_path.back() != '/') save_path += '/';

    cout << "Programm, starting..." << endl;

    // Принимаем 2 изображения
    Mat first = imread(first_image, 0);
    Mat second = imread(second_image, 0);

    // Разница изображения
    Mat srcImage = Difference(first, second, false, false, 5);
    if (!srcImage.data)
    {
        cout << "read in picture error!" << endl;
        return 0;
    }
    imwrite(save_path + "/difference.jpg", srcImage);

    cout << "Please, waiting..." << endl;

    if (argc > 4) {
        string arg_min = argv[4];
        string arg_max = argv[5];

        // Пороги гистограммы
        Mat dstImage = srcImage.clone();
        int rowsNum = dstImage.rows;
        int colsNum = dstImage.cols;
        //Image continuity judgment
        if (dstImage.isContinuous())
        {
            colsNum *= rowsNum;
            rowsNum = 1;
        }

        //Image pointer operation
        uchar* pDataMat;
        int controlMin = std::stoi(arg_min);
        int last_index = std::stoi(arg_max);
       

        //Calculate the gray level of image
        for (int j = 0; j < rowsNum; j++)
        {
            pDataMat = dstImage.ptr<uchar>(j);
            for (int i = 0; i < colsNum; i++)
            {
                //The first method, binary mapping
                if (pDataMat[i] > controlMin) {
                    //pDataMat[i] += 255 - last_index;
                }
                else
                    pDataMat[i] = 0;
            }
        }
        imwrite(save_path + "/threshold.jpg", dstImage);

        //Calculate the gray level of image
        for (int j = 0; j < rowsNum; j++)
        {
            pDataMat = dstImage.ptr<uchar>(j);
            for (int i = 0; i < colsNum; i++)
            {
                //The first method, binary mapping
                if (pDataMat[i] > controlMin) {
                    pDataMat[i] += 255 - last_index;
                }
                else
                    pDataMat[i] = 0;
            }
        }
        imwrite(save_path + "/threshold_mask.jpg", dstImage);
    }
    else {
        // Пороги гистограммы
        Mat dstImage = srcImage.clone();
        int rowsNum = dstImage.rows;
        int colsNum = dstImage.cols;
        //Image continuity judgment
        if (dstImage.isContinuous())
        {
            colsNum *= rowsNum;
            rowsNum = 1;
        }

        //Image pointer operation
        uchar* pDataMat;
        int controlMin = 0;
        int controlMax = 255;

        // Для нахождения моды яркости пикселей
        vector<int> pixels;
        for (size_t i = 0; i < 256; i++)
        {
            pixels.push_back(0);
        }

        float sum_of_pix = 0, count_of_pix = 0;
        for (int j = 0; j < rowsNum; j++)
        {
            pDataMat = dstImage.ptr<uchar>(j);
            for (int i = 0; i < colsNum; i++)
            {
                if (pDataMat[i] != 0) {
                    sum_of_pix += 1.0 * (int)pDataMat[i];
                    count_of_pix++;
                }

                pixels[(int)pDataMat[i]] ++;
            }
        }

        int max_index = 1;
        int max_value = pixels[1];
        int last_index;
        for (size_t i = 2; i < 256; i++)
        {
            if (max_value < pixels[i]) {
                max_index = i;
                max_value = pixels[i];
            }
        }
        controlMin = 2 * max_index - 1;
        for (size_t i = max_index; i < 256; i++)
        {
            if (pixels[i] < 10) {
                last_index = i;
                break;
            }
        }

        //Calculate the gray level of image
        for (int j = 0; j < rowsNum; j++)
        {
            pDataMat = dstImage.ptr<uchar>(j);
            for (int i = 0; i < colsNum; i++)
            {
                //The first method, binary mapping
                if (pDataMat[i] > controlMin) {
                    //pDataMat[i] += 255 - last_index;
                }
                else
                    pDataMat[i] = 0;
            }
        }
        imwrite(save_path + "/threshold.jpg", dstImage);

        //Calculate the gray level of image
        for (int j = 0; j < rowsNum; j++)
        {
            pDataMat = dstImage.ptr<uchar>(j);
            for (int i = 0; i < colsNum; i++)
            {
                //The first method, binary mapping
                if (pDataMat[i] > controlMin) {
                    pDataMat[i] += 255 - last_index;
                }
                else
                    pDataMat[i] = 0;
            }
        }
        imwrite(save_path + "/threshold_mask.jpg", dstImage);
    }
    

    cout << "Completed successfully!" << endl;

    return 0;
}