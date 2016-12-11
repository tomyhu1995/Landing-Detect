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
  int number_of_lines;
}LINES;

typedef struct _point_{
public:
  int x;
  int y;
}POINT;

LINES *LINES_detect(Mat image, int threshold, int *number_of_lines){
  Mat src_gray;
  /// Convert the image to grayscale
  cvtColor( image, src_gray, CV_BGR2GRAY );
  // Canny algorithm
  Mat contours;
  /// Reduce noise with a kernel 3x3
  blur( src_gray, contours, Size(3,3) );
  Canny(contours,contours, threshold, threshold*3, 3);

  blur( contours, contours, Size(3,3) );

  //imshow("contoursInv", contoursInv);
  //imshow("contours", contours);

  vector<Vec4i> lines;
  HoughLinesP(contours, lines, 1, CV_PI/180, 50, 50, 10 );
  LINES *detect_lines = new LINES[lines.size()];

  for( int i = 0; i < (int)lines.size(); i++ ){
    detect_lines[i].l = lines[i];
  }

  *number_of_lines = lines.size();

  return detect_lines;

}

float Mean_length(LINES *detect_lines, int number_of_lines){
  float mean = 0;
  for( int i = 0; i < number_of_lines; i++ ){
    mean += detect_lines[i].length();
  }

  return mean / number_of_lines;
}

POINT find_middle_point(LINES *detect_lines, int number_of_lines, float mean){
  float L;
  int left_x = 0, left_y = 0, right_x = 0, right_y = 0, left_count = 0, right_count = 0;
  POINT middle_point;

  printf("\n\n");
  for( int i = 0; i < number_of_lines; i++ )
  {
    L = detect_lines[i].length();
    //if(L == max || L == second_max){
    if(L >= mean){
      printf("length : %f\tSlope : %f\tStart (%d,%d), End(%d,%d)\n", L, detect_lines[i].slope(), detect_lines[i].l[0], detect_lines[i].l[1], detect_lines[i].l[2], detect_lines[i].l[3]);
      //line( image, Point(detect_lines[i].l[0], detect_lines[i].l[1]), Point(detect_lines[i].l[2], detect_lines[i].l[3]), Scalar(0,0,255), 3, CV_AA);
      if(detect_lines[i].slope() > 0){
        right_x += detect_lines[i].l[0];
        right_y += detect_lines[i].l[1];
        right_count++;
      }else{
        left_x += detect_lines[i].l[2];
        left_y += detect_lines[i].l[3];
        left_count++;
      }
    }
    
  }

  left_x = left_x / left_count;
  right_x = right_x / right_count;

  left_y = left_y / left_count;
  right_y = right_y / right_count;

  middle_point.x = (right_x+left_x) /2;
  middle_point.y = (right_y+left_y) /2;

  printf("left = (%d, %d)\tright = (%d, %d)\nmiddle point(%d,%d)\n",left_x, left_y, right_x, right_y, middle_point.x, middle_point.y);

  return middle_point;
}

int main(int argc, char* argv[]) {
  // Read input image
  int number_of_lines;
  Mat image= cv::imread(argv[1]);
  if (!image.data){return -1;}

  LINES *detect_lines = LINES_detect(image, 223, &number_of_lines);
  float mean = Mean_length(detect_lines, number_of_lines);

  POINT middle_point = find_middle_point(detect_lines, number_of_lines, mean);
  
  printf("middle point(%d,%d)\n",middle_point.x, middle_point.y);
  /// Wait until user exit program by pressing a key
  //waitKey(0);
  
  return 0;
}