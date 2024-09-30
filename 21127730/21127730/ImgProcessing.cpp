#include "ImgProcessing.h"


ImgProcessing::ImgProcessing() {}
ImgProcessing::~ImgProcessing() {}

//Convert a color image to the gray image.
int ImgProcessing::ConvertToGrayscale(const Mat& srcImg, Mat& dstImg) {
	if (srcImg.empty())
		return 0;

	int height = srcImg.rows;
	int width = srcImg.cols;

	//Do nothing is the input image is already in grayscale
	if (srcImg.channels() == 1) {
		dstImg = srcImg;
		return 1;
	}

	if (srcImg.channels() != 3) {
		std::cout << "Unabble to convert to grayscale!" << std::endl;
		return 0;
	}

	dstImg = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const uchar* pixelPtr = &srcImg.data[i * srcImg.step[0] + j * srcImg.step[1]];
			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			//Calculate grayscale value using NTSC formula
			dstPixelPtr[0] = static_cast<uchar>(0.11 * pixelPtr[0] +
				0.59 * pixelPtr[1] +
				0.3 * pixelPtr[2]);
		}
	}
	return 1;
}

int ImgProcessing::ChangeBrightness(const Mat& srcImg, Mat& dstImg, int k) {
	if (srcImg.empty())
		return 0;

	int numChannels = srcImg.channels();
	if (numChannels != 1 && numChannels != 3)
		return 0;

	int height = srcImg.rows;
	int width = srcImg.cols;

	//The output image must be the same type as input
	dstImg = Mat(height, width, numChannels == 1 ? CV_8UC1 : CV_8UC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const uchar* pixelPtr = &srcImg.data[i * srcImg.step[0] + j * srcImg.step[1]];
			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			for (int c = 0; c < numChannels; c++) {
				int newValue = pixelPtr[c] + k;
				//Clip the value in the range from 0 to 255
				dstPixelPtr[c] = static_cast<uchar>(std::max(0, std::min(255, newValue)));
			}

		}
	}
	return 1;
}

int ImgProcessing::ChangeContrast(const Mat& srcImg, Mat& dstImg, float k) {
	if (srcImg.empty())
		return 0;

	int numChannels = srcImg.channels();
	if (numChannels != 1 && numChannels != 3)
		return 0;


	int height = srcImg.rows;
	int width = srcImg.cols;
	dstImg = Mat(height, width, numChannels == 1 ? CV_8UC1 : CV_8UC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const uchar* pixelPtr = &srcImg.data[i * srcImg.step[0] + j * srcImg.step[1]];
			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			for (int c = 0; c < numChannels; c++) {
				int newValue = static_cast<int>(k * (pixelPtr[c] - 128) + 128);
				dstPixelPtr[c] = saturate_cast<uchar>(std::max(0, std::min(255, newValue)));
			}

		}
	}
	return 1;
}

int ImgProcessing::AvgFilter(const Mat& srcImg, Mat& dstImg, int k) {
	if (srcImg.empty())
		return 0;

	//The kernel is an odd number from 3
	if (k % 2 == 0 || k == 1) {
		std::cout << "Invalid kernel size!" << std::endl;
		return 0;
	}

	int numChannels = srcImg.channels();

	if (numChannels != 1 && numChannels != 3) {
		return 0;
	}

	int height = srcImg.rows;
	int width = srcImg.cols;
	dstImg = Mat(height, width, numChannels == 1 ? CV_8UC1 : CV_8UC3);

	for (int i = k / 2; i < height - k / 2; ++i) {
		for (int j = k / 2; j < width - k / 2; ++j) {
			int sumChannels[3] = { 0 };
			for (int m = -k / 2; m <= k / 2; ++m) {
				for (int l = -k / 2; l <= k / 2; ++l) {
					const uchar* pixelPtr = &srcImg.data[(i + m) * srcImg.step[0] + (j + l) * srcImg.step[1]];
					//Calculate the total value in the kernel
					for (int c = 0; c < numChannels; ++c) {
						sumChannels[c] += pixelPtr[c];
					}
				}
			}
			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			for (int c = 0; c < numChannels; ++c) {
				//The new assigned value is the avarage value of the kernel
				dstPixelPtr[c] = sumChannels[c] / (k * k);
			}
		}
	}
	return 1;
}

int ImgProcessing::MedianFilter(const Mat& srcImg, Mat& dstImg, int k) {
	if (srcImg.empty())
		return 0;

	if (k % 2 == 0 || k == 1) {
		std::cout << "Invalid kernel size!" << std::endl;
		return 0;
	}

	int height = srcImg.rows;
	int width = srcImg.cols;

	//The algorithm currently only works on grayscale image
	dstImg = Mat(height, width, CV_8UC1);

	for (int i = k / 2; i < height - k / 2; ++i) {
		for (int j = k / 2; j < width - k / 2; ++j) {
			//Store the pixel within the kernel's value
			std::vector<uchar> neighborValues;
			for (int m = -k / 2; m <= k / 2; ++m) {
				for (int l = -k / 2; l <= k / 2; ++l) {
					const uchar* pixelPtr = &srcImg.data[(i + m) * srcImg.step[0] + (j + l) * srcImg.step[1]];
					neighborValues.push_back(pixelPtr[0]);
				}
			}

			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			//Sort the array and choose the median value as the new value for the pixel
			std::sort(neighborValues.begin(), neighborValues.end());
			uchar newValue = neighborValues[neighborValues.size() / 2];
			dstPixelPtr[0] = newValue;
		}
	}
	return 1;
}

int ImgProcessing::GaussianFilter(const Mat& srcImg, Mat& dstImg, int k) {
	if (srcImg.empty())
		return 0;

	if (k % 2 == 0 || k == 1) {
		std::cout << "Invalid kernel size!" << std::endl;
		return 0;
	}

	int numChannels = srcImg.channels();

	if (numChannels != 1 && numChannels != 3) {
		return 0;
	}

	int height = srcImg.rows;
	int width = srcImg.cols;
	dstImg = Mat(height, width, numChannels == 1 ? CV_8UC1 : CV_8UC3);

	float sigma = k / 2.0;

	float** kernel = new float* [k];
	for (int i = 0; i < k; i++) {
		kernel[i] = new float[k];
	}

	float sumKernel = 0.0;

	//Calculate the Gaussian filter kernel
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			kernel[i][j] = (1 / (2 * pi) * sigma * sigma) * exp(-(i * i + j * j) / (2 * sigma * sigma));
			sumKernel += kernel[i][j];
		}
	}

	//Normalize the kernel
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			kernel[i][j] /= sumKernel;
		}
	}

	for (int i = k / 2; i < height - k / 2; ++i) {
		for (int j = k / 2; j < width - k / 2; ++j) {
			float sumPixel[3] = { 0.0 };
			for (int m = -k / 2; m <= k / 2; ++m) {
				for (int l = -k / 2; l <= k / 2; ++l) {
					const uchar* pixelPtr = &srcImg.data[(i + m) * srcImg.step[0] + (j + l) * srcImg.step[1]];
					//Convolution with the kernel
					for (int c = 0; c < numChannels; ++c) {
						sumPixel[c] += kernel[m + k / 2][l + k / 2] * pixelPtr[c];
					}
				}
			}
			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			for (int c = 0; c < numChannels; ++c) {
				dstPixelPtr[c] = static_cast<uchar>(std::max(0.0f, std::min(255.0f, sumPixel[c])));
			}
		}
	}

	for (int i = 0; i < k; i++)
		delete[]kernel[i];
	delete[]kernel;

	return 1;
}

int ImgProcessing::SobelEdgeDetect(const Mat& srcImg, Mat& dstImg) {
	if (srcImg.empty())
		return 0;

	//Initialize Solbel filter 3x3 kernel
	float kernelX[3][3] = { {1.0 / 4, 0, -1.0 / 4.0},
						{2.0 / 4.0, 0, -2.0 / 4.0},
						{1.0 / 4, 0, -1.0 / 4.0} };

	float kernelY[3][3] = { {-1.0 / 4, -2.0 / 4.0, -1.0 / 4.0},
						{0, 0, 0},
						{1.0 / 4, 2.0 / 4.0, 1.0 / 4.0} };

	int height = srcImg.rows;
	int width = srcImg.cols;

	//Edge image is grayscale
	dstImg = Mat(height, width, CV_8UC1);

	for (int i = 1; i < height - 1; ++i) {
		for (int j = 1; j < width - 1; ++j) {
			float sumX = 0, sumY = 0;
			for (int m = -1; m <= 1; ++m) {
				for (int l = -1; l <= 1; ++l) {
					const uchar* pixelPtr = &srcImg.data[(i + m) * srcImg.step[0] + (j + l) * srcImg.step[1]];
					//Calculate the gradient value for both axes
					sumX += kernelX[m + 1][l + 1] * pixelPtr[0];
					sumY += kernelY[m + 1][l + 1] * pixelPtr[0];
				}
			}

			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			//Calculate and assign the magnitude value
			dstPixelPtr[0] = static_cast<uchar>(sqrt(sumX * sumX + sumY * sumY));

		}
	}
	return 1;
}

int ImgProcessing::LaplaceEdgeDetect(const Mat& srcImg, Mat& dstImg) {
	if (srcImg.empty())
		return 0;

	//Initialize Laplace filter 3x3 kernel
	int kernel[3][3] = { {0, 1, 0},
						{1, -4, 1},
						{0, 1, 0} };

	int height = srcImg.rows;
	int width = srcImg.cols;

	//Edge image is grayscale
	dstImg = Mat(height, width, CV_8UC1);

	for (int i = 1; i < height - 1; ++i) {
		for (int j = 1; j < width - 1; ++j) {
			float sum = 0;
			for (int m = -1; m <= 1; ++m) {
				for (int l = -1; l <= 1; ++l) {
					const uchar* pixelPtr = &srcImg.data[(i + m) * srcImg.step[0] + (j + l) * srcImg.step[1]];
					//Apply convolution
					sum += kernel[m + 1][l + 1] * (*pixelPtr);
				}
			}

			uchar* dstPixelPtr = &dstImg.data[i * dstImg.step[0] + j * dstImg.step[1]];
			(*dstPixelPtr) = static_cast<uchar>(std::abs(sum));
		}
	}
	return 1;
}