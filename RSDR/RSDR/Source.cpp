#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define SAMPLES_NO 5

// Function headers
void onTrackbar(int, void*);
void createTrackbars();
void toggle(int key);
void morphImage(Mat &img);
void showImgContours(Mat &threshedimg, Mat original, int index);

bool morph = false;
bool detectRed = false;
bool detectBlue = false;
bool detectYellow = false;
bool showContours = false;

string imageString;

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

int main(int argc, char** argv)
{
	imageString = argv[1];

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
			int hMin = 100;
			int hMax = 125;
			int sMin = 110;
			int sMax = 255;
			int vMin = 40;
			int vMax = 255;
			*/

			Mat low, high;
			cvtColor(bgrImage, hsvImage, CV_BGR2HSV);
			inRange(hsvImage, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), imageDetectYellow);
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
	createTrackbar("hMin", "Trackbars", &hMin, 180, onTrackbar);
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

	findContours(threshedimg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	if (contours.size() > 0)
	{
		int i = 0;
		for (vector<Point> contour : contours)
		{
			if (contourArea(contour, false) > 400)
			{
				if (index == 0)
				{
					drawContours(original, contours, i, CV_RGB(0, 255, 0), 2, 8, hierarchy);
				}
				if (index == 1)
				{
					drawContours(original, contours, i, CV_RGB(255, 0, 0), 2, 8, hierarchy);
				}
				if (index == 2)
				{
					drawContours(original, contours, i, CV_RGB(0, 0, 255), 2, 8, hierarchy);
				}
			}
			i++;
		}
		imshow("Result", original);
	}
}