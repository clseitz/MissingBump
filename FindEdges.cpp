#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <numeric>

using namespace cv;
using namespace std;

Mat src; Mat src_gray; Mat grayThresh;
int thresh = 200;
int max_thresh = 255;
RNG rng(12345);
bool debug  = true;
/// Function header
void findEdges(int, void*,std::string name);
void prepPic(int, void* , std::string name);
vector <double> v_area;
/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  v_area.clear();
  cout <<  argc <<endl;
  for (int i = 1; i < argc; i++){
  src = imread( argv[i], 1 );
  
  std::string arg(argv[i]);
  std::string name = arg.substr(0, arg.find('.'));

  cout<<name<<endl;
  prepPic( 0 , 0 , name); 
  findEdges( 0, 0 , name);
  
  }
  cout<<"all areas"<< v_area.size()<<endl;

  double sum = std::accumulate(v_area.begin(), v_area.end(), 0.0);
  double mean = sum / v_area.size();
  double rms = sqrt( ( std::inner_product( v_area.begin(), v_area.end(), v_area.begin(), 0 ) ) / static_cast<double>( v_area.size() ) );

  cout<<"mean "<<mean << " stdev: "<<rms<<endl;


  //  waitKey(0);
  return(0);
}

void prepPic(int, void* , std::string name = "test")
{
  /// Convert image to gray and blur it

  if (debug)  imwrite( name + "_P1_grey_orig.jpg", src );
  cvtColor( src, src_gray, CV_BGR2GRAY );
  if (debug) imwrite( name + "_P2_grey.jpg", src_gray );
  threshold(src_gray, grayThresh, 100, 250, THRESH_BINARY);
  if (debug) imwrite( name + "_P3_thresh.jpg",  grayThresh);
 blur( grayThresh, grayThresh, Size(4,4) );
 if (debug) imwrite( name + "_P4_thresh.jpg",  grayThresh);
  grayThresh.convertTo( grayThresh, -1, 2, 0);
  if (debug) imwrite( name + "_P5_thresh_blur_contrast.jpg",  grayThresh);

}
/** @function thresh_callback */
void findEdges(int, void* , std::string name = "test")
{
  Mat canny_output;
  Mat canny_output2;
  vector<vector<Point> > contours;
  vector<vector<Point> > contours2;
  vector<Vec4i> hierarchy;
  vector<Vec4i> hierarchy2;

  /// Detect edges using canny
  Canny( grayThresh, canny_output, thresh, thresh*2, 3 );
  /// Find contours only outer contours, avoids double counting
  //  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat outImage = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat circles = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat circles_squares = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat circles_squares_all = Mat::zeros( canny_output.size(), CV_8UC3 );
  
  int countholes=0;

for( int i = 0; i< contours.size(); i++ )
    {
      //      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

      Scalar color = Scalar( 0, 255, 0 );
      drawContours(    circles , contours, i, color, 2, 8, hierarchy, 0, Point() );
      drawContours(    circles_squares , contours, i, color, 2, 8, hierarchy, 0, Point() );
      drawContours(    circles_squares_all , contours, i, color, 2, 8, hierarchy, 0, Point() );

      Rect r = boundingRect(contours[i]);
      rectangle(circles_squares_all,Point(r.x,r.y), Point(r.x+r.width,r.y+r.height), Scalar(0,0,255),3,8,0);
      v_area.push_back(r.area());
      if((r.area() > 1500  and r.area() < 3000) and  (r.height/r.width < 1.2 and r.width/r.height < 1.2)){
	  rectangle(circles_squares,Point(r.x,r.y), Point(r.x+r.width,r.y+r.height), Scalar(0,0,255),3,8,0);
	  rectangle(src,Point(r.x,r.y), Point(r.x+r.width,r.y+r.height), Scalar(0,0,255),3,8,0);
	  countholes++;
	}

    }

  cout<<countholes<<endl;
  imwrite( name + "_P6_black_circle.jpg", circles );
  imwrite( name + "_P7_black_circle_square_all.jpg", circles_squares_all );
  imwrite( name+ "_P8_black_circle_square.jpg", circles_squares );
  imwrite( name + "_P9_orig_withsquar.jpg", src );





}
