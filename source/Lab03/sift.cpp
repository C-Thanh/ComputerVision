#include "sift.hpp"

SIFTOperator::SIFTOperator() {}
SIFTOperator::~SIFTOperator() {}

int SIFTOperator::ExtractKeyPoints(const Mat& srcImg, std::vector<KeyPoint>& keypoints, Mat& descriptor) {
	if (srcImg.empty())
		return 0;

	//Extract the keypoints and their descriptor
	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	detector->detectAndCompute(srcImg, noArray(), keypoints, descriptor);


	return 1;
}

int SIFTOperator::FindClosestMatches(const Mat& descriptorTemplate, const Mat& descriptorScene, std::vector<DMatch>& goodMatches) {
	if (descriptorTemplate.empty() || descriptorScene.empty())
		return 0;
	
	//Create a FLAN Based matcher
	FlannBasedMatcher matcher;
	std::vector<std::vector<cv::DMatch>> knnMatches;
	//Apply KNN match
	matcher.knnMatch(descriptorTemplate, descriptorScene, knnMatches, 2);
	const float ratio_thresh = 0.75f;
	//find good matches
	for (size_t i = 0; i < knnMatches.size(); i++)
	{
		if (knnMatches[i][0].distance < ratio_thresh * knnMatches[i][1].distance)
		{
			goodMatches.push_back(knnMatches[i][0]);
		}
	}
	return 1;
}

int SIFTOperator::FindHomography(const Mat& templateImg, const Mat& sceneImg, Mat& homography) {
	if (templateImg.empty() || sceneImg.empty())
		return 0;

	std::vector<KeyPoint> tempKeyPoints, sceneKeyPoints;
	Mat tempDescriptor, sceneDescriptor;

	//Extract the corresponding keypoints and their descriptor
	if (!ExtractKeyPoints(templateImg, tempKeyPoints, tempDescriptor) || !ExtractKeyPoints(sceneImg, sceneKeyPoints, sceneDescriptor)) {
		std::cerr << "Failed to extract key points and descriptor!" << std::endl;
		return 0;
	}

	std::vector<DMatch> goodMatches;
	//Find the matches
	if (!FindClosestMatches(tempDescriptor, sceneDescriptor, goodMatches)) {
		std::cerr << "Failed to find closest matches!" << std::endl;
		return 0;
	}
	//Aggregates the point coordinates
	std::vector<cv::Point2f> points1, points2;
	for (size_t i = 0; i < goodMatches.size(); i++) {
		points1.push_back(tempKeyPoints[goodMatches[i].queryIdx].pt);
		points2.push_back(sceneKeyPoints[goodMatches[i].trainIdx].pt);
	}

	//Find the homography matrix 
	double ransacReprojThreshold = 3.0;
	homography = findHomography(Mat(points1), Mat(points2), RANSAC, ransacReprojThreshold);
	return 1;
}

int SIFTOperator::ObjectDetection(const Mat& templateImg, const Mat& sceneImg, Mat& imgMatches) {
	Mat grayTemplate, grayScene;

	//Convert image to grayscale
	cvtColor(templateImg, grayTemplate, COLOR_BGR2GRAY);
	cvtColor(sceneImg, grayScene, COLOR_BGR2GRAY);

	std::vector<KeyPoint> tempKeyPoints, sceneKeyPoints;
	Mat tempDescriptor, sceneDescriptor;

	if (!ExtractKeyPoints(grayTemplate, tempKeyPoints, tempDescriptor) || !ExtractKeyPoints(grayScene, sceneKeyPoints, sceneDescriptor)) {
		std::cerr << "Failed to extract key points and descriptor!" << std::endl;
		return 0;
	}

	std::vector<DMatch> goodMatches;

	if (!FindClosestMatches(tempDescriptor, sceneDescriptor, goodMatches)) {
		std::cerr << "Failed to find closest matches!" << std::endl;
		return 0;
	}

	Mat homography;
	if (!FindHomography(grayTemplate, grayScene, homography))
		return 0;

	//Get the template image's corners
	std::vector<cv::Point2f> tempCorners(4);
	tempCorners[0] = cv::Point(0, 0);
	tempCorners[1] = cv::Point(grayTemplate.cols - 1, 0);
	tempCorners[2] = cv::Point(grayTemplate.cols - 1, grayTemplate.rows - 1);
	tempCorners[3] = cv::Point(0, grayTemplate.rows - 1);

	//Transform to object's corners by homorgraphy matrix
	std::vector<cv::Point2f> sceneCorners(4);
	perspectiveTransform(tempCorners, sceneCorners, homography);

	//Draw the bounding box
	line(sceneImg, sceneCorners[0], sceneCorners[1], Scalar(0, 0, 255), 4);
	line(sceneImg, sceneCorners[1], sceneCorners[2], Scalar(0, 0, 255), 4);
	line(sceneImg, sceneCorners[2], sceneCorners[3], Scalar(0, 0, 255), 4);
	line(sceneImg, sceneCorners[3], sceneCorners[0], Scalar(0, 0, 255), 4);

	//Draw the good matches
	drawMatches(templateImg, tempKeyPoints, sceneImg, sceneKeyPoints, goodMatches, imgMatches, Scalar::all(-1),
			Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	return 1;
}
