#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void getContours(Mat imgDil, Mat img);
int differentCars(Rect);
Mat recognizeShapes();

string path = "inputImage.jpg";
Mat img = imread(path);
Mat img2 = imread(path);
int cannyMin = 25, cannyMax = 100, karnelSize = 10;
int counter = 0;

void main() {
	
	path = "example.jpg";
  
	img = imread(path);
	Mat secondImg = recognizeShapes();

	
	imshow("Recognized Image", secondImg);

	waitKey(0);
}

Mat recognizeShapes() {
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode,out;

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	threshold(imgGray, out, 200, 255, THRESH_BINARY);
	GaussianBlur(out, imgBlur, Size(3, 3), 3, 0);

	Canny(imgBlur, imgCanny, cannyMin, cannyMax);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(karnelSize, karnelSize));
	dilate(imgCanny, imgDil, kernel);

	getContours(imgDil, img);

	return img;
}


/*
int calcSlope(vector <Point> p) {
	int count = 0;
	for (int i=0; i<p.size()/2 ; i++){
	Point p1 = p[i];
	Point p2 = p[p.size()-i];
	//if ((abs(p1.x - p2.x)>=0.000000000001)||(abs(p1.y - p2.y) >= 0.000000000001)) return -1; /// it is a verical /HORIZental line
	count += slope(p1,p2);
      }
	return count / (p.size() / 2);
}
int slope(Point p1, Point p2) {
	int slope;
	if ((p1.x - p2.x) == 0) slope = 0;
	else  slope = (p1.y - p2.y) / (p1.x - p2.x);
	return slope < 0 ? (-1 * slope) : slope;
}
bool isLine(vector<Point> v) {
	   Point p = v[v.size()*19/100];
	    int slope = calcSlope(v);// printf("slope = %d ...%d, \n", slope, (p.y / p.x));
		if (slope == -2) return false;
	   if ((slope == -1) || (slope == 1))  return true;
	   else if ((p.y / p.x) == slope) return true;
	   return false;
   }
*/
void getContours(Mat imgDil, Mat imgOut) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	int area, objCor;
	string objectType;
	float peri;

	cout << "no of contors " << contours.size()<<endl;
	cout << "no of hierarchy " << hierarchy.size()<<endl;



	for (int i = 0; i < contours.size(); i++)
	{ 
		cout << endl << hierarchy[i]<<endl;

		area = contourArea(contours[i]);

		if (area > 500)
		{
			objectType = "";
			peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.043 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);

			objCor = (int)conPoly[i].size();
			cout << "obj core=" << objCor << endl;

			/*            LINE OR CURVE          */
			if ((hierarchy[i][2] == -1) && (hierarchy[i][3] == -1)) {
				if (objCor == 2) {
					objectType = "line";
				}


				else {
					objectType = "curve";
				}

				rectangle(imgOut, boundRect[i].tl(), boundRect[i].br(), Scalar(21, 21, 112), 2);

				putText(imgOut, objectType, { boundRect[i].x + boundRect[i].width / 3 ,boundRect[i].y - 4 }, FONT_HERSHEY_PLAIN, 1, Scalar(21, 21, 112), 2);

			}
			/*           other shapes but not a face           */
			else if ((hierarchy[i][2] != -1) && (hierarchy[i][3] == -1) && (hierarchy[hierarchy[i][2]][2] == -1)) {//its child doesct have children

				if (objCor == 4) {
					float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
					if (aspRatio > 0.95 && aspRatio < 1.05) objectType = "Square";
					else									objectType = "rectangle";
				}
				else if (objCor == 3)  objectType = "triangle";
				else {
					objectType = "circle";
				}

				rectangle(imgOut, boundRect[i].tl(), boundRect[i].br(), Scalar(21, 21, 112), 2);

				putText(imgOut, objectType, { boundRect[i].x + boundRect[i].width / 3 ,boundRect[i].y - 4 }, FONT_HERSHEY_PLAIN, 1, Scalar(21, 21, 112), 2);

			}
			//else if ((hierarchy[i][2] == -1) && (hierarchy[i][3] != -1) && (hierarchy[i][2] == -1));
			else if ((hierarchy[i][3] == -1) && (hierarchy[i][2] != -1)) {
				objectType = "face";


				rectangle(imgOut, boundRect[i].tl(), boundRect[i].br(), Scalar(21, 21, 112), 2);

				putText(imgOut, objectType, { boundRect[i].x + boundRect[i].width / 3 ,boundRect[i].y - 4 }, FONT_HERSHEY_PLAIN, 1, Scalar(21, 21, 112), 2);

			}

			else if ((hierarchy[hierarchy[i][3]][3] == -1)); // to discard the iner border of the face 

			else if ((hierarchy[i][3] != -1) && (hierarchy[i][0] == -1) && (hierarchy[i][1] == -1)); // inner borders of shapes 
			
		  
			else {
				if((hierarchy[i][1] == -1)&& (hierarchy[i][0] != -1)) objectType = "mouth";// doesnt have next
				else if (hierarchy[hierarchy[i][1]][1]==-1)  objectType = "nose";// the next of it doesnt have next
				else  objectType = "eye";
				 rectangle(imgOut, boundRect[i].tl(), boundRect[i].br(), Scalar(21, 21, 112), 2);

				 putText(imgOut, objectType, { boundRect[i].x + boundRect[i].width / 3 ,boundRect[i].y - 4 }, FONT_HERSHEY_PLAIN, 1, Scalar(21, 21, 112), 2);

			}

        }
		cout << objectType << endl;
	}
}
