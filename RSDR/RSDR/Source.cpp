#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define SAMPLES_NO 7
#define TRAINING_SET_SUBFOLDERS 43

struct myMatND {
	MatND h, s, v;
};

vector<vector<String>> filenames(TRAINING_SET_SUBFOLDERS);
vector<vector<myMatND>> dbHistograms(TRAINING_SET_SUBFOLDERS);

// Function headers
void onTrackbar(int, void*);
void createTrackbars();
void toggle(int key);
void morphImage(Mat &img);
void showImgContours(Mat &threshedimg, Mat original, int index);
bool loadTrainingSet(char* source);
bool computeDbHistograms();

bool morph = false;
bool detectRed = false;
bool detectBlue = false;
bool detectYellow = false;
bool showContours = false;
bool detectSigns = false;

string imageString;
string videoString;

int oldImg = 1;
int img = 1;
int hMin = 0;
int hMax = 10;
int sMin = 100;
int sMax = 255;
int vMin = 50;
int vMax = 255;
int dSizeX = 3;
int dSizeY = 3;
int eSizeX = 3;
int eSizeY = 4;

Mat bgrImage;
Mat temp;

int main(int argc, char** argv)
{
	if (!loadTrainingSet(argv[1]))
	{
		printf("Cannot load training set images");
		return 0;
	}
	
	// Training set images paths successfully loaded in global filenames vector

	computeDbHistograms();

	//Mat frame;
	//Mat low;
	//Mat high;

	//videoString = argv[1];
	//VideoCapture capture(videoString);

	//if (!capture.isOpened())
	//{
	//	throw "Cannot open video";
	//}

	//int i = 0;
	//for (;;)
	//{
	//	capture >> frame;
	//	if (frame.empty())
	//	{
	//		break;
	//	}
	//	
	//	frame.copyTo(temp);

	//	cvtColor(temp, frame, CV_BGR2HSV);// Convert image from BGR to HSV space (OpenCV stores images in BGR)
	//	inRange(frame, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), low);
	//	inRange(frame, Scalar(160, 100, 50), Scalar(180, 255, 255), high);
	//	addWeighted(low, 1.0, high, 1.0, 0.0, frame, -1);

	//	morphImage(frame);

	//	showImgContours(frame, temp, 1);

	//	waitKey(1);
	//}
	//waitKey(0);

	imageString = argv[2];

	createTrackbars();

	// Read the file
	bgrImage = imread(imageString, IMREAD_COLOR);

	Mat hsvImage;
	Mat resultImage;
	Mat imageDetectRed;
	Mat imageDetectBlue;
	Mat imageDetectYellow;
	int key;
	
	while ((key = waitKey(30)) != 27)
	{
		toggle(key);
		bgrImage.copyTo(resultImage);

		// Convert image from BGR to HSV space (OpenCV stores images in BGR)
		// Threshold image to extract red signs
		if (detectRed)
		{
			/*
			int hMin = 0;
			int hMax = 10;
			int sMin = 100;
			int sMax = 255;
			int vMin = 50;
			int vMax = 255;
			*/

			Mat low, high;
			cvtColor(bgrImage, hsvImage, CV_BGR2HSV);
			inRange(hsvImage, Scalar(0, 100, 50), Scalar(10, 255, 255), low);
			inRange(hsvImage, Scalar(160, 100, 50), Scalar(180, 255, 255), high);
			addWeighted(low, 1.0, high, 1.0, 0.0, imageDetectRed, -1);
		}

		if (detectBlue)
		{
			/*
			int hMin = 100;
			int hMax = 125;
			int sMin = 110;
			int sMax = 255;
			int vMin = 40;
			int vMax = 255;
			*/

			Mat low, high;
			cvtColor(bgrImage, hsvImage, CV_BGR2HSV);
			inRange(hsvImage, Scalar(100, 110, 40), Scalar(125, 255, 255), imageDetectBlue);
		}

		if (detectYellow)
		{	
			/*
			int hMin = 20;
			int hMax = 30;
			int sMin = 100;
			int sMax = 255;
			int vMin = 100;
			int vMax = 255;
			*/

			Mat low, high;
			cvtColor(bgrImage, hsvImage, CV_BGR2HSV);
			inRange(hsvImage, Scalar(20, 100, 100), Scalar(30, 255, 255), imageDetectYellow);
		}
		
		// Apply erosion and dilatation to get rid of some noise
		// and emphasize road signs
		if (morph)
		{
			morphImage(resultImage);
		}

		if (showContours)
		{
			Mat temp;
			bgrImage.copyTo(temp);
			if (detectRed)
			{
				showImgContours(imageDetectRed, bgrImage, 0);
			}
			if (detectBlue)
			{
				showImgContours(imageDetectBlue, bgrImage, 1);
			}
			if (detectYellow)
			{
				showImgContours(imageDetectYellow, bgrImage, 2);
			}
			temp.copyTo(bgrImage);
		}
		else 
		{
			imshow("Result", resultImage);
		}
	}

	return 0;
}

void createTrackbars()
{
	namedWindow("Trackbars", WINDOW_NORMAL);
	createTrackbar("image", "Trackbars", &img, SAMPLES_NO, onTrackbar);
	createTrackbar("hMin", "Trackbars", &hMin, 1000, onTrackbar);
	createTrackbar("hMax", "Trackbars", &hMax, 180, onTrackbar);
	createTrackbar("sMin", "Trackbars", &sMin, 255, onTrackbar);
	createTrackbar("sMax", "Trackbars", &sMax, 255, onTrackbar);
	createTrackbar("vMin", "Trackbars", &vMin, 255, onTrackbar);
	createTrackbar("vMax", "Trackbars", &vMax, 255, onTrackbar);
	createTrackbar("dSizeX", "Trackbars", &dSizeX, 10, onTrackbar);
	createTrackbar("dSizeY", "Trackbars", &dSizeY, 10, onTrackbar);
	createTrackbar("eSizeX", "Trackbars", &eSizeX, 10, onTrackbar);
	createTrackbar("eSizeY", "Trackbars", &eSizeY, 10, onTrackbar);

	/*createTrackbar("dilate", "GUI", &dilatationSlider, DILATATION_SLIDER_MAX, OnDilatationTrackbar);
	createTrackbar("erode", "GUI", &erosionSlider, EROSION_SLIDER_MAX, OnErosionTrackbar);*/
}

void toggle(int key)
{
	if (key == 'm')
		morph = !morph;
	if (key == 'r')
		detectRed = !detectRed;
	if (key == 'b')
		detectBlue = !detectBlue;
	if (key == 'y')
		detectYellow = !detectYellow;
	if (key == 'c')
		showContours = !showContours;
	if (key == 'd')
		detectSigns = !detectSigns;
}

void onTrackbar(int, void*)
{
	if (img != oldImg)
	{
		string no = to_string(img);
		no.append(".png");
		imageString.replace(imageString.end() - 5, imageString.end(), no);
		bgrImage = imread(imageString, IMREAD_COLOR);
		oldImg = img;
	}
}

void morphImage(Mat &src)
{
	if (dSizeX < 1 || dSizeY < 1 || eSizeX < 1 || eSizeY < 1)
	{
		return;
	}
	dilate(src, src, getStructuringElement(MORPH_DILATE, Size(dSizeX, dSizeY)));
	erode(src, src, getStructuringElement(MORPH_ERODE, Size(eSizeX, eSizeY)));
	dilate(src, src, getStructuringElement(MORPH_DILATE, Size(dSizeX, dSizeY)));
}

void showImgContours(Mat &threshedimg, Mat original, int index)
{
	if (!detectRed && !detectBlue && !detectYellow)
	{
		return;
	}

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(threshedimg, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	if (contours.size() > 0)
	{			   
		int i = 0;
		CvScalar color;
		vector<Point> currentContour;
		Rect currentBoundRect;
		for (vector<Point> contour : contours)
		{
			approxPolyDP(Mat(contour), currentContour, 3, true);
			currentBoundRect = boundingRect(Mat(currentContour));

			double ratio = currentBoundRect.width > currentBoundRect.height ? ((double)currentBoundRect.width) / currentBoundRect.height : ((double)currentBoundRect.height) / currentBoundRect.width;

			if (contourArea(contour, false) > 150 && ratio < 1.5)
			{
				if (index == 0)
				{
					color = CV_RGB(0, 255, 0);
				}
				if (index == 1)
				{
					color = CV_RGB(255, 0, 0);
				}
				if (index == 2)
				{
					color = CV_RGB(0, 0, 255);
				}
				//drawContours(original, contours, i, color, 2, 8, hierarchy);
				rectangle(original, currentBoundRect.tl(), currentBoundRect.br(), color, 2, 8, 0);

				if (detectSigns)
				{
					Mat imgPart;
					original(currentBoundRect).copyTo(imgPart);

					// Compare histograms
					vector<Mat> hsvPlanes;
					int hSize = 181;
					int svSize = 256;

					// Set the ranges for H, S, V channels
					float hRng[] = { 0, 180 };
					float sRng[] = { 0, 255 };
					float vRng[] = { 0, 255 };
					const float* hRange = { hRng };
					const float* sRange = { sRng };
					const float* vRange = { vRng };

					// We want our bins to have the same size (uniform) and 
					// to clear the histograms in the beginning, so
					bool uniform = true;
					bool accumulate = false;

					cvtColor(imgPart, imgPart, CV_BGR2HSV);
					split(imgPart, hsvPlanes);
					double max = 0;
					int indexI, indexJ;

					for (int j = 0; j < TRAINING_SET_SUBFOLDERS; j++)
					{
						int k = 0;
						for (myMatND dbMatND : dbHistograms[j])
						{
							MatND hTemp, sTemp, vTemp;
							double result = 0;

							calcHist(&hsvPlanes[0], 1, 0, Mat(), hTemp, 1, &hSize, &hRange, uniform, accumulate);
							normalize(hTemp, hTemp, 0, 1, NORM_MINMAX, -1, Mat());
							result += compareHist(hTemp, dbMatND.h, 0);

							calcHist(&hsvPlanes[1], 1, 0, Mat(), sTemp, 1, &svSize, &sRange, uniform, accumulate);
							normalize(sTemp, sTemp, 0, 1, NORM_MINMAX, -1, Mat());
							result += compareHist(sTemp, dbMatND.s, 0);

							calcHist(&hsvPlanes[2], 1, 0, Mat(), vTemp, 1, &svSize, &vRange, uniform, accumulate);
							normalize(vTemp, vTemp, 0, 1, NORM_MINMAX, -1, Mat());
							result += compareHist(vTemp, dbMatND.v, 0);

							if (result > max)
							{
								max = result;
								indexI = j;
								indexJ = k;
							}
							k++;
						}
					}
					putText(original, to_string(indexI).append(" ").append(to_string(indexJ)), { currentBoundRect.tl().x, currentBoundRect.tl().y - 10 }, 0, 0.5, color, 1, 8, false);
					putText(original, to_string(max), { currentBoundRect.tl().x, currentBoundRect.tl().y }, 0, 0.5, color, 1, 8, false);
				}
				i++;
			}
		}
		imshow("Result", original);
	}
}

bool loadTrainingSet(char* source)
{
	size_t len = strlen(source);

	char* path = new char[len + 2];

	strcpy(path, source);
	path[len] = '\\';
	path[len + 1] = '\0';

	for (int i = 0; i < TRAINING_SET_SUBFOLDERS; i++)
	{
		string no = to_string(i);
		if (i < 10)
		{
			no.insert(0, "0");
		}
		no.insert(0, path);
		no.append("\\*.ppm");
		glob(no, filenames[i], false);
		dbHistograms[i].resize(filenames[i].size());
	}
	return true;
}

bool computeDbHistograms()
{
	Mat currentImage;
	vector<Mat> hsvPlanes;

	int hSize = 181;
	int svSize = 256;

	// Set the ranges for H, S, V channels
	float hRng[] = { 0, 180 };
	float sRng[] = { 0, 255 };
	float vRng[] = { 0, 255 };
	const float* hRange = { hRng };
	const float* sRange = { sRng };
	const float* vRange = { vRng };

	// We want our bins to have the same size (uniform) and 
	// to clear the histograms in the beginning, so
	bool uniform = true; 
	bool accumulate = false;

	for (int i = 0; i < TRAINING_SET_SUBFOLDERS; i++)
	{
		int j = 0;
		for (String filename : filenames[i])
		{
			currentImage = imread(filename, IMREAD_COLOR);

			cvtColor(currentImage, currentImage, CV_BGR2HSV);
			split(currentImage, hsvPlanes);

			calcHist(&hsvPlanes[0], 1, 0, Mat(), dbHistograms[i][j].h, 1, &hSize, &hRange, uniform, accumulate);
			normalize(dbHistograms[i][j].h, dbHistograms[i][j].h, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsvPlanes[1], 1, 0, Mat(), dbHistograms[i][j].s, 1, &svSize, &sRange, uniform, accumulate);
			normalize(dbHistograms[i][j].s, dbHistograms[i][j].s, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsvPlanes[2], 1, 0, Mat(), dbHistograms[i][j].v, 1, &svSize, &vRange, uniform, accumulate);
			normalize(dbHistograms[i][j].v, dbHistograms[i][j].v, 0, 1, NORM_MINMAX, -1, Mat());

			/// Compute the histograms:
			//calcHist(&hsvPlanes[0], 1, 0, Mat(), dbHistograms[i][j][0], 1, &hSize, &hRange, uniform, accumulate);
			//calcHist(&hsvPlanes[1], 1, 0, Mat(), dbHistograms[i][j][1], 1, &svSize, &sRange, uniform, accumulate);
			//calcHist(&hsvPlanes[2], 1, 0, Mat(), dbHistograms[i][j][2], 1, &svSize, &vRange, uniform, accumulate);

			j++;
		}
	}
	return true;
}