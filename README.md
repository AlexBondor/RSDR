# RSDR - Road signs detection and recognition
This is a project I have built for the Pattern Recognition Systems course. The purpose of this project is to detect and recognize traffic signs from images using the OpenCV framework.

## Requirements
In order to run this project you will need the following:
* [OpenCV for Visual Studio](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_visual_studio_Opencv/windows_visual_studio_Opencv.html)
  * You can also check my website [alexbondor.me] (http://alexbondor.me/thoughts/opencv-in-vs2015) if you want to run it with VS2015
* [German Traffic Signs Dataset] (http://benchmark.ini.rub.de/?section=gtsdb&subsection=dataset) - Training images
* Patience to understand the code if something goes wrong

## Setup
After you make sure that the code is compiling you will need to have the following project directory tree:
├── root
|   ├── Images
|   |   ├── 0.png
|   |   ├── 1.png
└── README.md
- root
  - Images
    0.png
    1.png
    ...
    - Training
      - 00
        - 00000.ppm
        ...
      - 01
        - 00000.ppm
        ...
      ...
  -
