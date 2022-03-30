#pragma once
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Windows.h>
#include <iostream>



#define KEY(c) ( GetAsyncKeyState((int)(c)) & (SHORT)0x8000 )

using std::string;
using cv::imread;
using cv::imshow;
using cv::waitKey;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using cv::Mat;
using cv::VideoCapture;
using cv::cvtColor;
using cv::GaussianBlur;
using cv::Canny;
using cv::erode;
using cv::dilate;
using cv::resize;
using cv::Size;
using cv::Scalar;
using cv::circle;
using cv::Point;
using cv::rectangle;
using cv::line;
using cv::putText;