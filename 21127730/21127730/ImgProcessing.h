#pragma once
#include "windows.h "
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

#define pi 3.14159

class ImgProcessing {
public:
	ImgProcessing();
	int ConvertToGrayscale(const Mat& srcImg, Mat& dstImg);
	int ChangeBrightness(const Mat& srcImg, Mat& dstImg, int k);
	int ChangeContrast(const Mat& srcImg, Mat& dstImg, float k);
	int AvgFilter(const Mat& srcImg, Mat& dstImg, int k);
	int MedianFilter(const Mat& srcImg, Mat& dstImg, int k);
	int GaussianFilter(const Mat& srcImg, Mat& dstImg, int k);
	int SobelEdgeDetect(const Mat& srcImg, Mat& dstImg);
	int LaplaceEdgeDetect(const Mat& srcImg, Mat& dstImg);
	~ImgProcessing();
};
