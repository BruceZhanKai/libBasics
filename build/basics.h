/******************************************************************************
-  File         [ basics.h ]
-  Author       [ Bruce Zhan ]
-  Synopsis     [ basics ]
-  Date         [ 2017.09.28 ]
-  Lisense		[ LGPL ]
******************************************************************************/


#ifndef _BRUCE_BASE_H_
#define _BRUCE_BASE_H_

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
namespace BZ
{
    /***************************************************************************/
    /*General*/
    /***************************************************************************/
    enum class GetMode{SAVE=0,SHOW=1};
    enum class SaveMode{OUT=0,APP=1};
    enum class TextMode{TEXTT=0,TEXTN=1,EMPTY=2};


    std::string GetAbsolutePath(char* argv);

    std::vector<std::string> GetFolderWholePath(std::string FolderPath);

    std::string GetLine(std::string Line, char Separator=0);

    std::string GetTime(BZ::GetMode getmode);


    template <class Whatever>
    bool DoOfstream(Whatever message, std::string LogPath, BZ::SaveMode savemode, BZ::TextMode  textmode)
    {
        std::ofstream file;
        switch(savemode)
        {
            case BZ::SaveMode::OUT:
            file.open(LogPath, std::ios::out);
            break;
            case BZ::SaveMode::APP:
            file.open(LogPath, std::ios::app);
            break;
        }
        switch(textmode)
        {
            case BZ::TextMode::TEXTT:
            file<<message<<"\t";
            break;
            case BZ::TextMode::TEXTN:
            file<<message<<"\n";
            break;
            case BZ::TextMode::EMPTY:
            break;
        }
        file.close();
        return true;
    }


    /***************************************************************************/
    /*Image*/
    /***************************************************************************/

    bool IsSkinGood(const cv::Mat& Image, float SkinGoodThreshold=0.55);

    cv::Mat add_black(cv::Mat& original_image, int scale=2);

    cv::Mat DrawRectangle(cv::Mat Image, cv::Rect Rectangle, cv::Scalar Color=cv::Scalar(255, 255, 0));

    cv::Mat Rotate(const cv::Mat& Image, double degree=-90.0);

    cv::Mat EyeRotationFace(std::vector<cv::Point>& shape_points, cv::Mat ROI, float OffSet, int ImgSize=192, bool LockScale=false);

    /***************************************************************************/
    /*Socket*/
    /***************************************************************************/

    bool WriteImageTime(cv::Mat& Image);



};//end of namespace



#endif

