#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>

#define PI 3.1415926

using namespace cv;

typedef struct lines_{
public:
  Vec4i l;
  float length(){
    return sqrt((float(l[2] - l[0]) * (l[2] - l[0])) + float((l[3] - l[1]) * (l[3] - l[1])));
  }
  float slope(){
   return (float)(l[3] - l[1]) / (float)(l[2] - l[0]);
  }
}LINES;


int main(int argc, char* argv[]) {
  // Read input image
  Mat image= cv::imread(argv[1]);
  if (!image.data){return -1;}

  //Mat imageGreen;

  //inRange(image, Scalar(50, 50, 50), Scalar(80,80,80), imageGreen);

  //imshow("imageGreen", imageGreen);
  //waitKey(0);

  Mat src_gray;
  /// Convert the image to grayscale
  cvtColor( image, src_gray, CV_BGR2GRAY );
  // Canny algorithm
  Mat contours;
  /// Reduce noise with a kernel 3x3
  blur( src_gray, contours, Size(3,3) );
  int threshold = 223;
  Canny(contours,contours, threshold, threshold*3, 3);

  blur( contours, contours, Size(3,3) );

  //imshow("contoursInv", contoursInv);
  imshow("contours", contours);

  vector<Vec4i> lines;
  HoughLinesP(contours, lines, 1, CV_PI/180, 50, 50, 10 );

  float L;
  float max = 0, second_max = 0;
  float mean = 0;

  LINES *detect_lines = new LINES[lines.size()];

  for( size_t i = 0; i < lines.size(); i++ ){
    detect_lines[i].l = lines[i];
    if(detect_lines[i].length() > max){
      max = detect_lines[i].length();
    }
    mean += detect_lines[i].length();
    printf("%f\n", detect_lines[i].length());
  }

  mean = mean / lines.size();

  for( size_t i = 0; i < lines.size(); i++ ){
    if(detect_lines[i].length() > second_max && detect_lines[i].length() != max){
      second_max = detect_lines[i].length();
    }
  }

  printf("\n\n");
  for( size_t i = 0; i < lines.size(); i++ )
  {
    L = detect_lines[i].length();
    //if(L == max || L == second_max){
    if(L >= mean && detect_lines[i].slope() > 0){
      printf("length : %f\tSlope : %f\n", L, detect_lines[i].slope());
      line( image, Point(detect_lines[i].l[0], detect_lines[i].l[1]), Point(detect_lines[i].l[2], detect_lines[i].l[3]), Scalar(0,0,255), 3, CV_AA);
    }
    
  }

  imshow("line", image);
  /// Wait until user exit program by pressing a key
  waitKey(0);
  
  return 0;
}