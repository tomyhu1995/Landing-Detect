#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>

#define PI 3.1415926

#define DIFF_RANGE 0.02

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

typedef struct _REF_{
  float neg_slope;
  float pos_slope;
  POINT middle_point;
}Reference;

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

  printf("\n");
  printf("number_of_lines = %d\tmean = %f\n", number_of_lines, mean);
  for( int i = 0; i < number_of_lines; i++ )
  {
    L = detect_lines[i].length();
    //if(L == max || L == second_max){
    printf("No.%d => length : %f\tSlope : %f\tStart (%d,%d), End(%d,%d)\n", i, L, detect_lines[i].slope(), detect_lines[i].l[0], detect_lines[i].l[1], detect_lines[i].l[2], detect_lines[i].l[3]);
    //if(L >= mean){
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
    //}
  }

  printf("right_count = %d, left_count = %d\n", right_count, left_count );
  if(right_count != 0){
    right_x = right_x / right_count;
    right_y = right_y / right_count;
  }

  if(left_count != 0){
    left_x = left_x / left_count;
    left_y = left_y / left_count;
  }
  
  middle_point.x = (right_x+left_x) /2;
  middle_point.y = (right_y+left_y) /2;

  printf("left = (%d, %d)\tright = (%d, %d)\nmiddle point(%d,%d)\n",left_x, left_y, right_x, right_y, middle_point.x, middle_point.y);

  return middle_point;
}

int learning_machine(Mat image, Reference *Ref_sig){
  int number_of_lines;
  if (!image.data){
    printf("input_image is empty\n");
    return -1;
  }
  float positive_slope = 0, negative_slope = 0;
  int positive_count = 0, negative_count = 0;

  LINES *detect_lines = LINES_detect(image, 223, &number_of_lines);

  for(int i = 0; i < number_of_lines; i++){
    if(detect_lines[i].slope() <0){
      negative_slope += detect_lines[i].slope();
      negative_count++;
    }else{
      positive_slope += detect_lines[i].slope();
      positive_count++;
    }
  }

  negative_slope = negative_slope / negative_count;
  positive_slope = positive_slope / positive_count;

  printf("Mean negative_slope = %f, mean positive_slope = %f\n", negative_slope, positive_slope);

  float mean = Mean_length(detect_lines, number_of_lines);

  POINT middle_point = find_middle_point(detect_lines, number_of_lines, mean);
  //printf("middle_point x = %d, y = %d\n", middle_point.x, middle_point.y);

  Ref_sig->neg_slope = negative_slope;
  Ref_sig->pos_slope = positive_slope;
  Ref_sig->middle_point = middle_point;

  //circle(image, Point(middle_point.x, middle_point.y), 5, Scalar(0,0,255), 1, 8, 0);
}

int classfier_machine(Mat image, Reference Ref_sig){
  int number_of_lines;
  if (!image.data){
    printf("input_image is empty\n");
    return -1;
  }
  float positive_slope = 0, negative_slope = 0;
  int positive_count = 0, negative_count = 0;

  LINES *detect_lines = LINES_detect(image, 223, &number_of_lines);

  for(int i = 0; i < number_of_lines; i++){
    if(detect_lines[i].slope() <0){
      negative_slope += detect_lines[i].slope();
      negative_count++;
    }else{
      positive_slope += detect_lines[i].slope();
      positive_count++;
    }
  }

  negative_slope = negative_slope / negative_count;
  positive_slope = positive_slope / positive_count;

  printf("Mean negative_slope = %f, mean positive_slope = %f\n", negative_slope, positive_slope);

  float mean = Mean_length(detect_lines, number_of_lines);

  POINT middle_point = find_middle_point(detect_lines, number_of_lines, mean);
  //printf("middle_point x = %d, y = %d\n", middle_point.x, middle_point.y);

  printf("\n******************Landing guide******************\n");
  printf("=================Height guidness=================\n");
  if(positive_slope < Ref_sig.pos_slope || negative_slope > Ref_sig.neg_slope){
    if(abs(positive_slope - Ref_sig.pos_slope) > DIFF_RANGE || abs(negative_slope - Ref_sig.neg_slope) > DIFF_RANGE){
      printf("Too Low !!\n");
    }else{
      printf("Good for landing\n");
    }
  }else if(positive_slope > Ref_sig.pos_slope || negative_slope < Ref_sig.neg_slope){
    if(abs(positive_slope - Ref_sig.pos_slope) > DIFF_RANGE || abs(negative_slope - Ref_sig.neg_slope) > DIFF_RANGE){
      printf("Too high!!\n");
    }else{
      printf("Good for landing\n");
    }
  }
  printf("=================Direction guidness=================\n");
  if(abs(middle_point.x - Ref_sig.middle_point.x) > DIFF_RANGE){
    if(middle_point.x < Ref_sig.middle_point.x){
      printf("You need to turn left\n");
    }else{
      printf("You need to turn right\n");
    }
  }

  
}

int main(int argc, char* argv[]) {
  // Read input image
  
  Mat image;
  char file_path[20] = {0};
  int command;
  Reference Ref_sig;
  memset(&Ref_sig, 0, sizeof(Reference));
  
  printf("------------------Welcome to UAV auto landing system------------------\n");
  while(1){
    printf("Training or Runing ? Training : 1, Running : 2, exit : 0 ---> ");
    scanf("%d", &command);

    if(command == 1){
      memset(&Ref_sig, 0, sizeof(Reference));
      printf("\n------------------Training phase------------------\n\n");
      printf("file_path = ");
      scanf("%s", file_path);
      image = cv::imread(file_path);
      
      if(!image.data){
        printf("empty data\n");
        return -1;
      }

      learning_machine(image, &Ref_sig);
      printf("Ref_sig.neg_slope = %f\n", Ref_sig.neg_slope);
      printf("Ref_sig.pos_slope = %f\n", Ref_sig.pos_slope);
      printf("Ref_sig.middle_point.x = %d, Ref_sig.middle_point.y = %d\n", Ref_sig.middle_point.x, Ref_sig.middle_point.y);
    
      memset(file_path, 0, sizeof(file_path));
      printf("\n------------------End of training phase------------------\n\n");
    }else if(command == 2){
      clock_t start, end;
      start = clock();
      printf("\n------------------Running phase------------------\n\n");
      printf("file_path = ");
      scanf("%s", file_path);
      
      image = cv::imread(file_path);
      
      if(!image.data){
        printf("empty data\n");
        return -1;
      }

      classfier_machine(image, Ref_sig);
      
      memset(file_path, 0, sizeof(file_path));
      end = clock();
      printf("Time usage : %f\n", (float)(end-start)/CLOCKS_PER_SEC);
      printf("\n------------------End of running phase------------------\n\n");
      

    }else if(command == 0){
      printf("------------------Thanks for your using------------------\n");
      break;
    }else{
      printf("No good input\n");
    }
  }
  
  return 0;
}