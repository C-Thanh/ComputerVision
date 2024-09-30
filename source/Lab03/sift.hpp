#pragma once
#include "windows.h "
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class SIFTOperator
{
public:
	SIFTOperator();
	~SIFTOperator();
	int ExtractKeyPoints(const Mat& srcImg, std::vector<KeyPoint>& keypoints, Mat& descriptor);
	int FindClosestMatches(const Mat& decriptorTemplate, const Mat& descriptorScene, std::vector<DMatch>& goodMatches);
	int FindHomography(const Mat& templateImg, const Mat& sceneImg, Mat& homography);
	int ObjectDetection(const Mat& templateImg, const Mat& sceneImg, Mat& imgMatches);
};