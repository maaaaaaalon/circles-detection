#include "Header0.h"

#include <vector>
#include <stack>
#include <math.h>
#define verbose /*gives more detail*/
#define trackTime /*tracks elapsed time in each iteration*/
//define s4e /*gives error rate (false positives) for image 4*/
#include "COMP473_Project/Functions.h"

int main(int argc, char* argv[]) {

	//cout << argv[0] << endl;//Get PWD
	//Mat img = imread("../../sample_images/s1.PNG");
	//Mat img = imread("../../sample_images/s2.PNG");//Random Logo
	//Mat img = imread("../../sample_images/S3.jpg");//Too large, takes 3 minutes to process
	Mat img = imread("../../sample_images/s4.jpg");//Coins

	Mat output;
	if (img.empty()) {
		cout << "Image not loaded." << endl;
		cerr << "Something went wrong, press any button to exit." << endl;
		waitKey(0);
		return 1;
	}
	cv::Size a = img.size();
	int h = a.height;
	int w = a.width;
	#ifdef verbose
	cout << "Height: " << h << "\tWidth: " << w << endl;
	#endif

	Mat imgG;
	cvtColor(img, imgG, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(imgG, imgG, Size(3, 3), 0);
	Mat dx;
	Mat dxM;
	//cv::Sobel(img, dx, 3, 1, 0, 3);
	cv::Sobel(imgG, dx, 3, 1, 0, 3);
	dxM = dx;
	//cv::cvtColor(dx, dxM, cv::COLOR_RGB2GRAY);
	//imshow("dx", dx);

	Mat dy;
	Mat dyM;
	//cv::Sobel(img, dy, 3, 0, 1, 3);
	cv::Sobel(imgG, dy, 3, 0, 1, 3);
	//cv::cvtColor(dy, dyM, cv::COLOR_RGB2GRAY);
	dyM = dy;
	//imshow("dy", dy);
	Mat dyMv = cv::abs(dyM*(short)32);
	imshow("dyM", dyMv);

	Mat Edges;
	char c;
	
	int Th = 200;
	int Tl = 150;
	int TR100 = 216;
	int Tol12_100 = 55;
	int Tol13_100 = 95;
	/*
	//S2.png
	int TR100 = 216;
	int Tol12_100 = 55;
	int Tol13_100 = 95;
	*/
	cv::namedWindow("Controls", (580, 400));
	cv::createTrackbar("Tol grad", "Controls", &Tol12_100, 100);
	cv::createTrackbar("Tol pos", "Controls", &Tol13_100, 100);
	cv::createTrackbar("Vote Threshold", "Controls", &TR100, 600);
	int Rmin = 5;
	cv::createTrackbar("Min Radius", "Controls", &Rmin, 25);
	int record = 0;
	//cv::createTrackbar("Record", "Controls", &record, 2, &fPrintImage, (void*)&output);
	cv::createTrackbar("Record", "Controls", &record, 1, &fPrintImage, (void*)&output);
	//cv::createButton("Print Image", &fPrintImage, &output, cv::QtButtonTypes::QT_PUSH_BUTTON);
	
	//int NR = 45;
	int NR = 35;
	//unsigned int* ac = new unsigned i0nt[NR * (long long)h * w];
	unsigned short* ac = new unsigned short[NR * (long long)h * w];
	if (ac == nullptr) {
		cout << "failed to allocate sufficient memory." << endl;
		cerr << "Something went wrong, press any button to exit." << endl;
		waitKey(0);
		return 2;
	}
	
	imshow("SRC Image", img);
	cv::Canny(dx, dy, Edges, Th, Tl);//Seems to not care about threshold values...
	//cv::Canny(dx, dy, Edges, Th / 255.0, Tl / 255.0);//Maybe? Untested
	#ifdef verbose
	imshow("Edges", Edges);
	#endif
	//Partially inspired by https://github.com/eToTheEcs/hough-transform/blob/master/hough.cpp
	std::stack<std::pair<int, int>>  edgePoints;
	int nP = 0;
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			//if (Edges.at<uchar>(i, j) == 255) {
			if (Edges.at<uchar>(i, j) == 255) {
				++nP;
				edgePoints.push(std::make_pair(i, j));
			}
		}
	}
	#ifdef verbose
	cout << "Number of edge points: " << nP << endl;
	#endif
	
	

	while (!(KEY('C')) || !(KEY(VK_LCONTROL))) {
		cout << "beginning computation" << endl;
		int d;
		#ifdef trackTime
		auto started = std::chrono::high_resolution_clock::now();
		#endif
		ZeroMemory(ac, NR * (long long)h * (long long)w * sizeof(unsigned short));
		std::stack<std::pair<int, int>>  s1(edgePoints);
		while (!s1.empty()) {
			std::pair<int, int> P1 = s1.top(); s1.pop();
			std::stack< std::pair<int, int>> s2(s1);
			while (!s2.empty()) {
				std::pair<int, int> P2 = s2.top(); s2.pop();
				if (IdealPair(dxM.at<short>(P1.first, P1.second), dyM.at<short>(P1.first, P1.second), dxM.at<short>(P2.first, P2.second), dyM.at<short>(P2.first, P2.second),(P1.second-P2.second),(P1.first-P2.first), Tol12_100/100.0f, Tol13_100/100.0f)) {
					int d = (int)std::sqrt((P1.first - P2.first) * (P1.first - P2.first) + (P1.second - P2.second) * (P1.second - P2.second)) / 2;
					int i = (P1.first + P2.first) / 2;
					int j = (P1.second + P2.second) / 2;
					if (d < NR + Rmin && d >= Rmin) ac[(i + j * h) * NR + d - Rmin]++;
				}
			}

		}
		
		double Tr = TR100 / 100.0;
		output = img.clone();
		#ifdef s4e
		int nC = 0;
		#endif
		for (int i = 1; i < h - 1; ++i) {
			for (int j = 1; j < w - 1; ++j) {
				for (int k = 1; k < NR; ++k) {
					int temp = ac[(i + j * h) * NR + k];
					if ( temp > Tr * (k + Rmin)) {
						if (temp > ac[(i + j * h) * NR + k - 1] && temp > ac[(i + j * h) * NR + k + 1]) {
							if (temp > ac[(i - 1 + j * h) * NR + k] && temp > ac[(i+1 + j * h) * NR + k])
								if (temp > ac[(i + (j - 1) * h) * NR + k] && temp > ac[(i + (j + 1) * h) * NR + k]) {
									cv::circle(output, cv::Point(j, i), k + Rmin, cv::Scalar(255, 255,0), 2);
									#ifdef s4e
									++nC;
									#endif
								}
									
						}
					}
				}
			}
		}

		imshow("Output", output);
		#ifdef verbose
		long long NumZeros = 0;
		for (long long i = 0; i < ((long long)NR) * ((long long)w) * ((long long)h); ++i) {
			if (ac[i] == 0) ++NumZeros;
		}
		cout << endl << "-----Sparsity Data-----" << endl;
		cout << "Number of zeros: " << NumZeros << endl;
		cout << "Memory Utilization: " << (((long long)NR) * ((long long)w) * ((long long)h) - NumZeros) << " / " << (((long)NR) * ((long)w) * ((long)h)) << endl;
		cout << "Percent Utilization: " << ((((long long)NR) * ((long long)w) * ((long long)h) - NumZeros)) * 100.0 / (((long long)NR) * ((long long)w) * ((long long)h)) << "%" << endl;
		cout << "-----------------------" << endl << endl;
		#endif

		#ifdef trackTime
		auto done = std::chrono::high_resolution_clock::now();
		cout << "Elapsed time: ";
		cout << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count() << " (ms)" << endl;
		#endif

		#ifdef s4e
		cout << "Number of false positives: " << nC - 8 << endl;
		#endif

		c = (char)waitKey(0);
		if (c == 27) {//ESC
			break;
		}
	}

	return 0;
}
