#include "vis/ScatterPlotVisualizer.hpp"
#include <sstream>
void ScatterPlotVisualizer::plot(const cv::Mat& X, const cv::Mat& y,
                                 std::optional<float> error)
{
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    cv::Scalar colors[3]{{0,255,0},{255,0,0},{0,0,255}};
    cv::Mat plot = cv::Mat::zeros(512,512,CV_8UC3);
    double min0,max0,min1,max1;
    cv::minMaxIdx(X.col(0),&min0,&max0);
    cv::minMaxIdx(X.col(1),&min1,&max1);
    for(int i=0;i<X.rows;++i){
        int cls=y.at<int>(i);
        int x=static_cast<int>(512*(X.at<float>(i,0)-min0)/(max0-min0));
        int yy=static_cast<int>(512*(X.at<float>(i,1)-min1)/(max1-min1));
        cv::circle(plot,{x,yy},3,colors[cls%3],-1);
    }
    if(error){std::ostringstream ss;ss<<"Acc:"<<*error<<"%";cv::putText(plot,ss.str(),{20,490},cv::FONT_HERSHEY_SIMPLEX,0.6,{200,200,200},1);}    
    cv::imshow("Training scatter",plot);cv::waitKey(1);
#endif
}
