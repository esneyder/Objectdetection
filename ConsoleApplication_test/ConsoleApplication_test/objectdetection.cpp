#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>
using namespace cv;
int Hueminvalue = 0;
int Huemaxvalue = 256;
int Saturationminvalue = 0;
int Saturationmaxvalue = 256;
int Valueminvalue = 0;
int Valuemaxvalue = 256;
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_NUM_OBJECTS=50;
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
const string windowName = "Blob detection";
const string windowName2 = "Camera image";
const string trackbarWindowName = "Trackbars";
void on_trackbar( int, void* )
{

}
string Stringconversion(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}
void Slidercreation(){
    namedWindow(trackbarWindowName,0);
	char TrackbarName[50];
	sprintf( TrackbarName, "Hueminvalue", Hueminvalue);
	sprintf( TrackbarName, "Huemaxvalue", Huemaxvalue);
	sprintf( TrackbarName, "Saturationminvalue", Saturationminvalue);
	sprintf( TrackbarName, "Saturationmaxvalue", Saturationmaxvalue);
	sprintf( TrackbarName, "Valueminvalue", Valueminvalue);
	sprintf( TrackbarName, "Valuemaxvalue", Valuemaxvalue);  
    createTrackbar( "Hueminvalue", trackbarWindowName, &Hueminvalue, Huemaxvalue, on_trackbar );
    createTrackbar( "Huemaxvalue", trackbarWindowName, &Huemaxvalue, Huemaxvalue, on_trackbar );
    createTrackbar( "Saturationminvalue", trackbarWindowName, &Saturationminvalue, Saturationmaxvalue, on_trackbar );
    createTrackbar( "Saturationmaxvalue", trackbarWindowName, &Saturationmaxvalue, Saturationmaxvalue, on_trackbar );
    createTrackbar( "Valueminvalue", trackbarWindowName, &Valueminvalue, Valuemaxvalue, on_trackbar );
    createTrackbar( "Valuemaxvalue", trackbarWindowName, &Valuemaxvalue, Valuemaxvalue, on_trackbar );


}
void Objectonscreen(int x, int y,Mat &frame){
	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);
	putText(frame,Stringconversion(x)+","+Stringconversion(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
}
void objectdilate(Mat &thresh){
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}
void objectdetection(int &x, int &y, Mat threshold, Mat &cameraFeed){
	Mat temp;
	threshold.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;
			}
			if(objectFound ==true){
				putText(cameraFeed,"Object detected",Point(0,50),2,1,Scalar(0,255,0),2);
				Objectonscreen(x,y,cameraFeed);}
		}else putText(cameraFeed,"Error",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}
int main(int argc, char* argv[])
{
	Mat cameraFeed;
	Mat HSV;
	Mat threshold;
	int x=0, y=0;
	Slidercreation();
	VideoCapture capture;
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	while(1){
		capture.read(cameraFeed);
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,Scalar(Hueminvalue,Saturationminvalue,Valueminvalue),Scalar(Huemaxvalue,Saturationmaxvalue,Valuemaxvalue),threshold);
		objectdilate(threshold);
		objectdetection(x,y,threshold,cameraFeed);
		imshow(windowName2,threshold);
		imshow(windowName,cameraFeed);
		waitKey(30);
	}
	return 0;
}

