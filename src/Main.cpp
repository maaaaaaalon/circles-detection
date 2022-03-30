#include "Header0.h"

#include <vector>
#include <math.h>
#define verbose

int main(int argc, char* argv[]) {

	//cout << argv[0] << endl;//Get PWD
	//Mat img = imread("../../sample_images/s1.PNG");
	Mat img = imread("../../sample_images/s2.PNG");

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
	Mat dx;
	cv::Sobel(img, dx, 3, 1, 0, 3);
	//imshow("dx", dx);

	Mat dy;
	cv::Sobel(img, dy, 3, 0, 1, 3);
	//imshow("dy", dy);

	Mat Edges;
	char c;
	
	int Th = 200;
	int Tl = 150;
	int TR100 = 400;
	cv::namedWindow("Controls", (580, 400));
	cv::createTrackbar("High Threshold", "Controls", &Th, 255);
	cv::createTrackbar("Low Threshold", "Controls", &Tl, 255);
	cv::createTrackbar("Vote Threshold", "Controls", &TR100, 600);
	int Rmin = 5;
	int NR = 45;
	unsigned int* ac = new unsigned int[NR * (long)h * w];
	if (ac == nullptr) {
		cout << "failed to allocate sufficient memory." << endl;
		cerr << "Something went wrong, press any button to exit." << endl;
		waitKey(0);
		return 2;
	}

	while (!(KEY('C')) || !(KEY(VK_LCONTROL))) {
		imshow("SRC Image", img);
		cv::Canny(dx, dy, Edges, Th, Tl);//Seems to not care about threshold values...
		#ifdef verbose
		imshow("Edges", Edges);
		#endif
		//Partially inspired by https://github.com/eToTheEcs/hough-transform/blob/master/hough.cpp
		std::vector<std::pair<int, int>>  edgePoints;
		int nP = 0;
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				//if (Edges.at<uchar>(i, j) == 255) {
				if (Edges.at<uchar>(i, j) == 255) {
					++nP;
					edgePoints.push_back(std::make_pair(i, j));
				}
			}
		}
		#ifdef verbose
		cout << "Number of edge points: " << nP << endl;
		#endif
		int d;
		ZeroMemory(ac, NR * (long)h * (long)w * sizeof(unsigned int));
		for (auto&& P : edgePoints) {
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j) {
					d = (int)std::sqrt((i - P.first) * (i - P.first) + (j - P.second) * (j - P.second));
					if (d < NR + Rmin && d >= Rmin) ac[(i + j * h) * NR + d - Rmin]++;
				}
			}
		}
		
		double Tr = TR100 / 100.0;
		output = img.clone();
		for (int i = 1; i < h - 1; ++i) {
			for (int j = 1; j < w - 1; ++j) {
				for (int k = 1; k < NR; ++k) {
					int temp = ac[(i + j * h) * NR + k];
					if ( temp > Tr * (k + Rmin)) {
						if (temp > ac[(i + j * h) * NR + k - 1] && temp > ac[(i + j * h) * NR + k + 1]) {
							if (temp > ac[(i - 1 + j * h) * NR + k] && temp > ac[(i+1 + j * h) * NR + k])
								if (temp > ac[(i + (j-1) * h) * NR + k] && temp > ac[(i + (j + 1) * h) * NR + k])
									cv::circle(output, cv::Point(j, i), k + Rmin, cv::Scalar(255, 255,0), 2);
									//cv::circle(output, cv::Point(i, j), k + Rmin, cv::Scalar(0, 255, 255));
						}
					}
				}
			}
		}

		imshow("Output", output);
		#ifdef verbose
		long long NumZeros = 0;
		for(long long i = 0; i < ((long long)NR) * ((long long) w) * ((long long) h); ++i){
			if (ac[i] == 0) ++NumZeros;
		}
		cout << endl << "-----Sparcity Data-----" << endl;
		cout << "Number of zeros: " << NumZeros << endl;
		cout << "Memory Utilization: " << (((long long)NR) * ((long long)w) * ((long long)h) - NumZeros) << " / " << (((long)NR) * ((long)w) * ((long)h)) << endl;
		cout << "Percent Utilization: " << ((((long long)NR) * ((long long)w) * ((long long)h) - NumZeros))  *100.0/  (((long long)NR) * ((long long)w) * ((long long)h)) <<"%" <<  endl;
		cout << "-----------------------" << endl << endl;
		#endif
		c = (char)waitKey(0);
		if (c == 27) {//ESC
			break;
		}
	}

	return 0;
}
