#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "UserInterface.h"

using namespace cv;
using namespace std;

#define DILATATION_SLIDER_MAX 25
#define EROSION_SLIDER_MAX 25

void CreateGUI();
Mat Dilate(Mat, int amount);
Mat Erode(Mat, int amount);

void OnDilatationTrackbar(int, void*);
void OnErosionTrackbar(int, void*);
void OnCleanTrackbar(int, void*);

int dilatationSlider = 0;
int erosionSlider = 0;
int cleanSlider = 0;

void CreateGUI()
{
	namedWindow("GUI", 1);
	int clean;

	createTrackbar("dilate", "GUI", &dilatationSlider, DILATATION_SLIDER_MAX, OnDilatationTrackbar);
	createTrackbar("erode", "GUI", &erosionSlider, EROSION_SLIDER_MAX, OnErosionTrackbar);
	createTrackbar("clean", "GUI", &clean, 1, OnCleanTrackbar);
}

void OnDilatationTrackbar(int, void*)
{
	if (currentImg.data)
	{
		currentImg = defaultImg;
		currentImg = Dilate(currentImg, dilatationSlider);
		imshow("result", currentImg);
	}
}

void OnErosionTrackbar(int, void*)
{
	if (currentImg.data)
	{
		currentImg = defaultImg;
		currentImg = Erode(currentImg, erosionSlider);
		imshow("result", currentImg);
	}
}

void OnCleanTrackbar(int, void*)
{
	currentImg = defaultImg;
}

Mat Dilate(Mat src, int amount)
{
	Mat dst;
	int dilatationType = MORPH_DILATE;
	Mat element = getStructuringElement(dilatationType,
		Size(2 * amount + 1, 2 * amount + 1),
		Point(amount, amount));
	dilate(src, dst, element);
	return dst;
}

Mat Erode(Mat src, int amount)
{
	Mat dst;
	int erosionType = MORPH_ERODE;
	Mat element = getStructuringElement(erosionType,
		Size(2 * amount + 1, 2 * amount + 1),
		Point(amount, amount));
	erode(src, dst, element);
	return dst;
}