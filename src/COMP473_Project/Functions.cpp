#include "Functions.h"
#include "../Header0.h"
bool IdealPair(int x1, int y1, int x2, int y2, int x3, int y3, float tol12,float tol13) {
	int m1_2 = (x1 * x1 + y1 * y1);//magnitude of 1, squared
	int m2_2 = (x2 * x2 + y2 * y2);//magnitude of 2, squared
	int m3_2 = (x3 * x3 + y3 * y3);
	int dot2a = ((x1 * x2) + (y1 * y2));//dot product
	if ((dot2a*dot2a) > tol12 * ( m1_2* m2_2)) {
		int dot2b = (x1 * x3 + y1 * y3);
		if (dot2b * dot2b > tol13 * (m1_2 * m3_2)){
			int dot2c = (x2 * x3 + y2 * y3);
			if (dot2c * dot2c > tol13 * (m2_2 * m3_2)) {
				return true;
			}
		}
	}
	return false;
}

void fPrintImage(int state, void* userData) {
	
	if(state == 1){
		cout << "Recording oOutput Image (output.jpg)." << endl;
		cv::imwrite("output.jpg", *((Mat*)userData));
	} else if (state == 2) {
		cout << "Recording to CSV not yet implemented." << endl;
	}
}