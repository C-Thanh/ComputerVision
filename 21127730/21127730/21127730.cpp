#include "ImgProcessing.h"

int main(int argc, char* argv[])
{
	std::string mode = argv[1];
	std::string inputFilePath = argv[2];
	std::string outputFilePath = argv[3];
	std::cout << argc << std::endl;

	Mat inputImage = imread(inputFilePath);
	if (inputImage.empty()) {
		std::cerr << "Error: Unable to read the input image." << std::endl;
		return 2;
	}

	ImgProcessing imgProcessor;
	Mat resultImage;

	imshow("Input Image", inputImage);

	if (argc == 4) {
		if (mode == "-rgb2gray") {
			if (!imgProcessor.ConvertToGrayscale(inputImage, resultImage)) {
				std::cerr << "Error: Grayscale conversion failed." << std::endl;
				return 3;
			}
		}
		else if (mode == "-sobel") {
			Mat grayImg;
			if (inputImage.channels() != 1) {
				if (!imgProcessor.ConvertToGrayscale(inputImage, grayImg)) {
					std::cerr << "Error: Invalid image." << std::endl;
					return 3;
				}
			}
			grayImg = inputImage;
			if (!imgProcessor.SobelEdgeDetect(grayImg, resultImage)) {
				std::cerr << "Error: Sobel edge detection failed." << std::endl;
				return 4;
			}
		}
		else if (mode == "-laplace") {
			Mat grayImg;
			if (inputImage.channels() != 1) {
				if (!imgProcessor.ConvertToGrayscale(inputImage, grayImg)) {
					std::cerr << "Error: Invalid image." << std::endl;
					return 3;
				}
			}
			grayImg = inputImage;
			if (!imgProcessor.LaplaceEdgeDetect(grayImg, resultImage)) {
				std::cerr << "Error: Laplace edge detection failed." << std::endl;
				return 4;
			}
		}
		else {
			std::cerr << "Error: Unknown mode specified." << std::endl;
			return 4;
		}
	}
	else if (argc == 5) {
		if (mode == "-contrast") {
			std::cout << mode << std::endl;
			float c = atof(argv[4]);
			if (!imgProcessor.ChangeContrast(inputImage, resultImage, c)) {
				std::cerr << "Error: Contrast change failed." << std::endl;
				return 4;
			}
		}
		else if (mode == "-brightness") {
			int c = atoi(argv[4]);
			if (!imgProcessor.ChangeBrightness(inputImage, resultImage, c)) {
				std::cerr << "Error: Brightness change failed." << std::endl;
				return 4;
			}
		}
		else if (mode == "-avg") {
			int c = atoi(argv[4]);
			if (!imgProcessor.AvgFilter(inputImage, resultImage, c)) {
				std::cerr << "Error: Average blurring failed." << std::endl;
				return 4;
			}
		}
		else if (mode == "-med") {
			int c = atoi(argv[4]);
			Mat grayImg;
			if (inputImage.channels() != 1) {
				if (!imgProcessor.ConvertToGrayscale(inputImage, grayImg)) {
					std::cerr << "Error: Invalid image." << std::endl;
					return 3;
				}
			}
			grayImg = inputImage;
			if (!imgProcessor.MedianFilter(grayImg, resultImage, c)) {
				std::cerr << "Error: Median blurring failed." << std::endl;
				return 4;
			}
		}
		else if (mode == "-gau") {
			int c = atoi(argv[4]);
			if (!imgProcessor.AvgFilter(inputImage, resultImage, c)) {
				std::cerr << "Error: Gaussian blurring failed." << std::endl;
				return 4;
			}
		}
		else {
			std::cerr << "Error: Unknown mode specified." << std::endl;
			return 4;
		}
	}

	// Show the result image
	imshow("Result Image", resultImage);
	waitKey(0);

	if (resultImage.empty()) {
		std::cerr << "Error: Result image is empty." << std::endl;
		return 6;
	}

	// Save the result image
	if (!imwrite(outputFilePath, resultImage)) {
		std::cerr << "Error: Unable to save the result image." << std::endl;
		return 5;
	}

	std::cout << "Processing complete. Result image saved to: " << outputFilePath << std::endl;

	return 0;
}