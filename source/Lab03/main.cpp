#include "sift.hpp"

int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Error: Invalid command." << std::endl;
		return 0;
	}

	std::string mode = argv[1];
	std::string inputTempFilePath = argv[2];
	std::string inputSceneFilePath = argv[3];
	std::string outputFilePath = argv[4];

	const Mat templateImg = imread(inputTempFilePath);
	const Mat sceneImg = imread(inputSceneFilePath);
	if (templateImg.empty() || sceneImg.empty()) {
		std::cerr << "Error: Invalid input image!" << std::endl;
		return 2;
	}

	Mat imgMatches;

	if (mode == "-sift") {
		SIFTOperator sift;

		if (!sift.ObjectDetection(templateImg, sceneImg, imgMatches)) {
			std::cerr << "Error: Operation failed!" << std::endl;
			return 3;
		}
	}
	else {
		std::cerr << "Error: Unnown mode!" << std::endl;
		return 4;
	}

	if (imgMatches.empty()) {
		std::cerr << "Error: Empty output image!" << std::endl;
		return 5;
	}

	imshow("Good matches and object detect", imgMatches);
	waitKey(0);

	if (!imwrite(outputFilePath, imgMatches)) {
		std::cerr << "Error: Unable to save the result image!" << std::endl;
		return 6;
	}
	std::cout << "Processing complete. Result image saved to: " << outputFilePath << std::endl;

	return 1;
}