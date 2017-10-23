/******************************************************************************
-  File         [ basics.h ]
-  Author       [ Bruce Zhan ]
-  Synopsis     [ basics ]
-  Date         [ 2017.09.28 ]
-  Lisense		[ LGPL ]
******************************************************************************/

#include "basics.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

namespace BZ
{


    /***************************************************************************/
    /*General*/
    /***************************************************************************/

    std::string GetAbsolutePath(char* argv)
    {
        //std::cout<<"argv="<<argv<<std::endl;
        std::string project(argv), projectname;
        std::stringstream liness_project(project);
        std::getline(liness_project, projectname, '/');
        std::getline(liness_project, projectname);

        char* basePath = realpath(argv,NULL);
        std::string fullpath(basePath), singlepath, finalpath;
        //std::cout<<"fullpath="<<fullpath<<std::endl;
        delete[] basePath;
        basePath = 0;
        delete[] basePath;

        std::stringstream liness_fullpath(fullpath);

        while(std::getline(liness_fullpath, singlepath, '/'))
        {
            if(singlepath!=projectname)
                finalpath=finalpath+singlepath+"/";
        }

        //std::cout<<"projectname="<<projectname<<std::endl;
        //std::cout<<"AbsolutePath="<<finalpath<<std::endl;
        return finalpath;
    }


    std::vector<std::string> GetAllNameInFolder(std::string FolderPath)
    {
        std::vector<std::string> WholePath;
        DIR * dir;
		struct dirent * ptr;
		dir =opendir(FolderPath.c_str());
		if(dir != NULL)
		{
		   while((ptr = readdir(dir))!=NULL)
			{
				//printf(?d_name: %s\n?,);
				std::string ImagePath(ptr->d_name);
				if(ImagePath == "." || ImagePath == "..")
				{
					//
				}
				else
				{
                    WholePath.push_back(ImagePath);
				}
            }
        }
        closedir(dir);
        return WholePath;
    }




    std::string GetLine(std::string Line, char Separator)
    {
        std::stringstream liness(Line);
        std::string Separating;
        std::getline(liness, Separating, Separator);
        return Separating;
    }




    std::string GetTime(BZ::GetMode getmode)
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::string year, month, date, hour, minute, second;
        year = std::to_string(ltm->tm_year + 1900);
        month = std::to_string(ltm->tm_mon + 1);
        date = std::to_string(ltm->tm_mday);
        hour = std::to_string(ltm->tm_hour);
        minute = std::to_string(ltm->tm_min);
        second = std::to_string(ltm->tm_sec);
        std::string time_str;
        switch(getmode)
        {
            case BZ::GetMode::SAVE:
            time_str = year + "-" + month + "-" + date + "-" + hour + "-" + minute + "-" + second;
            break;
            case BZ::GetMode::SHOW:
            time_str = year + "-" + month + "-" + date + " " + hour + ":" + minute + ":" + second;
            break;
        }
        return time_str;
    };





    /***************************************************************************/
    /*Image*/
    /***************************************************************************/



    bool IsSkinGood(const cv::Mat& Image, float SkinGoodThreshold)
    {
        //cv::Mat copy = Image.clone();
        cv::Mat bgr2hsvImg, hsv2skinImg;
        cv::cvtColor(Image, bgr2hsvImg, cv::COLOR_BGR2HSV);
        cv::inRange(bgr2hsvImg, cv::Scalar(0, 58, 40), cv::Scalar(35, 174, 255), hsv2skinImg);

        //imshow("HSV2Skin", hsv2skinImg);

        float pixels = 0.0;
        for (int i = 0; i < hsv2skinImg.rows; i++)
            for (int j = 0; j < hsv2skinImg.cols; j++)
            {
                float value = hsv2skinImg.at<uchar>(i, j);
                //std::cout << "value=" << value << std::endl;
                if (value == 255)
                    pixels++;
            }
        float ratio = pixels / float(hsv2skinImg.rows*hsv2skinImg.cols);

        if (ratio > SkinGoodThreshold)
        {
            //std::cout << "IsSkinGood=true" << std::endl;
            return true;
        }
        else
        {
            //std::cout << "IsSkinGood=false" << std::endl;
            return false;
        }
        //std::cout << "ratio=" << ratio << std::endl;


        //cv::Mat bgr2ycrcbImg, ycrcb2skinImg;
        //cv::cvtColor(copy, bgr2ycrcbImg, cv::COLOR_BGR2YCrCb);
        //cv::inRange(bgr2ycrcbImg, cv::Scalar(80, 135, 85), cv::Scalar(255, 180, 135), ycrcb2skinImg);
        //imshow("YCrCb2Skin", ycrcb2skinImg);
        //imshow("Original", copy);

        //waitKey(10);
    }


    cv::Mat add_black(cv::Mat& original_image, int scale)
    {

        cv::Mat ROI_land = cv::Mat(cv::Size((original_image.cols + (original_image.cols / (2*scale))), (original_image.rows + (original_image.rows / (2*scale)))), CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Mat ROI_regist;
        cv::Rect rect_original_image(std::ceil(ROI_land.cols - original_image.cols) / scale, std::ceil(ROI_land.rows - original_image.rows) / scale, original_image.cols, original_image.rows);
        ROI_regist = ROI_land(rect_original_image);
        original_image.copyTo(ROI_regist);
        return ROI_land;
    }


    cv::Mat DrawRectangle(cv::Mat Image, cv::Rect Rectangle, cv::Scalar Color)/*, std::vector<cv::Point>& points)*/
    {
        cv::Mat frame = Image;
        cv::Rect rec_d = Rectangle;

        cv::Point P0, P0_1, P0_2;           //draw lines of outer retangle
        cv::Point P1, P1_1, P1_2;
        cv::Point P2, P2_1, P2_2;
        cv::Point P3, P3_1, P3_2;

        cv::Point k0_1, k0_2;            //draw lines of center retangle
        cv::Point k1_1, k1_2;
        cv::Point k2_1, k2_2;
        cv::Point k3_1, k3_2;


        P0 = cvPoint(rec_d.x, rec_d.y);
        P0_1 = cvPoint(rec_d.x + rec_d.width / 4, rec_d.y);
        P0_2 = cvPoint(rec_d.x, rec_d.y + rec_d.height / 4);

        P1 = cvPoint(rec_d.x + rec_d.width, rec_d.y);
        P1_1 = cvPoint(rec_d.x + rec_d.width - rec_d.width / 4, rec_d.y);
        P1_2 = cvPoint(rec_d.x + rec_d.width, rec_d.y + rec_d.height / 4);

        P2 = cvPoint(rec_d.x, rec_d.y + rec_d.height);
        P2_1 = cvPoint(rec_d.x + rec_d.width / 4, rec_d.y + rec_d.height);
        P2_2 = cvPoint(rec_d.x, rec_d.y + rec_d.height - rec_d.height / 4);

        P3 = cvPoint(rec_d.x + rec_d.width, rec_d.y + rec_d.height);
        P3_1 = cvPoint(rec_d.x + rec_d.width - rec_d.width / 4, rec_d.y + rec_d.height);
        P3_2 = cvPoint(rec_d.x + rec_d.width, rec_d.y + rec_d.height - rec_d.width / 4);

        //-----------------------------------------------------------------------

        k0_1 = cvPoint(rec_d.x + rec_d.width / 2, rec_d.y + rec_d.height / 9);
        k0_2 = cvPoint(rec_d.x + rec_d.width / 2, rec_d.y - rec_d.height / 9);


        k1_1 = cvPoint(rec_d.x + rec_d.width / 9, rec_d.y + rec_d.height / 2);
        k1_2 = cvPoint(rec_d.x - rec_d.width / 9, rec_d.y + rec_d.height / 2);


        k2_1 = cvPoint(rec_d.x + rec_d.width + rec_d.width / 9, rec_d.y + rec_d.height / 2);
        k2_2 = cvPoint(rec_d.x + rec_d.width - rec_d.width / 9, rec_d.y + rec_d.height / 2);


        k3_1 = cvPoint(rec_d.x + rec_d.width / 2, rec_d.y + rec_d.height + rec_d.height / 9);
        k3_2 = cvPoint(rec_d.x + rec_d.width / 2, rec_d.y + rec_d.height - rec_d.height / 9);
        //-----------------------------------------------------------------------

        cv::line(frame, P0, P0_1, Color, 2, CV_AA);
        cv::line(frame, P0, P0_2, Color, 2, CV_AA);

        cv::line(frame, P1, P1_1, Color, 2, CV_AA);
        cv::line(frame, P1, P1_2, Color, 2, CV_AA);

        cv::line(frame, P2, P2_1, Color, 2, CV_AA);
        cv::line(frame, P2, P2_2, Color, 2, CV_AA);

        cv::line(frame, P3, P3_1, Color, 2, CV_AA);
        cv::line(frame, P3, P3_2, Color, 2, CV_AA);

        //------------------------------------------------------------------------
        cv::line(frame, k0_1, k0_2, Color, 1, CV_AA);
        cv::line(frame, k1_1, k1_2, Color, 1, CV_AA);

        cv::line(frame, k2_1, k2_2, Color, 1, CV_AA);
        cv::line(frame, k3_1, k3_2, Color, 1, CV_AA);
        //-------------------------------------------------------------------------
        return frame;
        //for (int i = 0; i < points.size(); i++)
        //	circle(img, points[i], 1, color, 1);

        //imshow("frame",frame);
    }




    cv::Mat Rotate(const cv::Mat& Image, double degree)
    {
        cv::Mat input;
        if (!Image.empty())
        {
            input = Image.clone();
            int width = input.cols;
            int height = input.rows;
            cv::Mat output = cv::Mat(cv::Size(height, width), CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Point2f Center;
            Center.x = width / 2.0;
            Center.y = height / 2.0;
            double scale = 1.0;
            //double degree = -90.0;
            cv::Mat Trans = cv::getRotationMatrix2D(Center, degree, scale);
            Trans.at<double>(0, 2) += cvRound(height - width) / 2;
            Trans.at<double>(1, 2) += cvRound(width - height) / 2;
            cv::warpAffine(input, output, Trans, output.size());
            return output;
        }
        else
            return input;
    }



    cv::Mat EyeRotationFace(std::vector<cv::Point>& shape_points, cv::Mat ROI, float OffSet, int ImgSize, bool LockScale)
    {
        //cv::Mat ResultImage_224;
        cv::Mat	ResultImage_192;
        //cv::Mat rotlindmark = cv::Mat(cv::Size(ROI.cols, ROI.rows), CV_8U, cv::Scalar(255));
        cv::Point leftEye, rightEye; // left is from looking.
        leftEye.x = shape_points[39].x;
        leftEye.y = shape_points[39].y;
        rightEye.x = shape_points[42].x;
        rightEye.y = shape_points[42].y;
        cv::Point2f face_center;
        face_center.x = (shape_points[36].x + shape_points[45].x)*0.5f;
        face_center.y = shape_points[30].y*1.0f;

        //int SizeSelect_224 = ImgHeight;
        int SizeSelect_192 = ImgSize;

        //t11 = getTickCount();
        if (face_center.x > 0 && face_center.y > 0 && leftEye.x >= 0 && rightEye.x >= 0 && leftEye.y >= 0 && rightEye.y >= 0 && face_center.x < ROI.cols &&  face_center.y < ROI.rows && leftEye.x < ROI.cols && rightEye.x < ROI.cols && leftEye.y < ROI.rows && rightEye.y < ROI.rows && !ROI.empty())
        {
            //t12 = getTickCount();
            double dy = (rightEye.y - leftEye.y);
            double dx = (rightEye.x - leftEye.x);
            double len = sqrt(dx*dx + dy*dy);                                                   //實際兩眼距離(計算是歪斜情況下)
            if (len == 0)
                return ResultImage_192;
            double degree = std::atan2(dy, dx) * 180.0 / CV_PI; // Convert from radians to degrees.     //轉換rad變成degree
            double desiredLen = double(ImgSize)/192.0*46.667;
            double scale = desiredLen / len;                                                    //縮放比例
            //======================================getRotationMatrix2D====================================//
            //const double degree = 30;
            double angle = degree * CV_PI / 180.;
            double alpha = std::cos(angle);
            double beta = std::sin(angle);
            int iWidth = ROI.cols;
            int iHeight = ROI.rows;
            int iNewWidth = cvRound(iWidth * fabs(alpha) + iHeight * fabs(beta));
            int iNewHeight = cvRound(iHeight * fabs(alpha) + iWidth * fabs(beta));
            double m[6];
            m[0] = alpha;
            m[1] = beta;
            m[2] = (1 - alpha) * face_center.x - beta * face_center.y;
            m[3] = -m[1];
            m[4] = m[0];
            m[5] = beta * face_center.x + (1 - alpha) * face_center.y;
            cv::Mat M = cv::Mat(2, 3, CV_64F, m);
            cv::Mat matDst1 = cv::Mat(cv::Size(iNewWidth, iNewHeight), ROI.type(), cv::Scalar::all(0));
            double D = m[0] * m[4] - m[1] * m[3];
            D = D != 0 ? 1. / D : 0;
            double A11 = m[4] * D, A22 = m[0] * D;
            m[0] = A11; m[1] *= -D;
            m[3] *= -D; m[4] = A22;
            double b1 = -m[0] * m[2] - m[1] * m[5];
            double b2 = -m[3] * m[2] - m[4] * m[5];
            m[2] = b1; m[5] = b2;
            //=======================================warpAffine==============================================//
            cv::Point rot_landmark_point[68];
            int round_delta = 512;//nearest
            for (int y = 0; y < iNewHeight; ++y)
            {
                for (int x = 0; x < iNewWidth; ++x)
                {
                    int adelta = cv::saturate_cast<int>(m[0] * x * 1024);
                    int bdelta = cv::saturate_cast<int>(m[3] * x * 1024);
                    int X0 = cv::saturate_cast<int>((m[1] * y + m[2]) * 1024) + round_delta;
                    int Y0 = cv::saturate_cast<int>((m[4] * y + m[5]) * 1024) + round_delta;
                    int X = (X0 + adelta) >> 10;
                    int Y = (Y0 + bdelta) >> 10;
                    if ((unsigned)X < iWidth && (unsigned)Y < iHeight)
                    {
                        matDst1.at<cv::Vec3b>(y, x) = ROI.at<cv::Vec3b>(Y, X);
                        for (int i = 0; i < 3; i++)
                        {
                            int p[3] = { 30, 36, 45 };
                            int int_x_range = abs(shape_points[p[i]].x - X);
                            int int_y_range = abs(shape_points[p[i]].y - Y);
                            if (int_x_range <= 1 && int_y_range <= 1)
                            {
                                rot_landmark_point[p[i]].x = x;
                                rot_landmark_point[p[i]].y = y;
                                //printf("lindX: %d  lindY: %d i:%d\n", rot_landmark_point[i].x, rot_landmark_point[i].y, i);
                            }
                        }
                    }
                }
            }
            if (LockScale)
            {
                if (scale < 1)
                {
                    //========================================resize_image================================================//
                    cv::resize(matDst1, matDst1, cv::Size(std::ceil(matDst1.cols*scale), std::ceil(matDst1.rows*scale)));
                    //=======================================rotation_landmark============================================//
                    for (int i = 0; i < 3; i++)//landmark  平移
                    {
                        int p[3] = { 30, 36, 45 };
                        shape_points[p[i]].x = rot_landmark_point[p[i]].x * scale;
                        shape_points[p[i]].y = rot_landmark_point[p[i]].y * scale;
                    }
                    face_center.x = (shape_points[36].x + shape_points[45].x)*0.5f;
                    face_center.y = shape_points[30].y*1.0f;
                    cv::Rect rect_matDst1(face_center.x - (SizeSelect_192 / 2), face_center.y - (SizeSelect_192 / 2), SizeSelect_192, SizeSelect_192);
                    if (rect_matDst1.x >= 0 && rect_matDst1.x < matDst1.cols && rect_matDst1.y >= 0 && rect_matDst1.y < matDst1.rows
                        && (rect_matDst1.x + SizeSelect_192) >= 0 && (rect_matDst1.x + SizeSelect_192) < matDst1.cols && (rect_matDst1.y + SizeSelect_192) >= 0 && (rect_matDst1.y + SizeSelect_192) < matDst1.rows)
                    {
                        ResultImage_192 = matDst1(rect_matDst1);
                    }
                }
            }
            else
            {

                //========================================resize_image================================================//
                cv::resize(matDst1, matDst1, cv::Size(std::ceil(matDst1.cols*scale), std::ceil(matDst1.rows*scale)));
                for (int i = 0; i < 3; i++)//landmark  平移
                {
                    int p[3] = { 30, 36, 45 };
                    shape_points[p[i]].x = rot_landmark_point[p[i]].x * scale;
                    shape_points[p[i]].y = rot_landmark_point[p[i]].y * scale;
                }
                float Eye_Y = (float)(shape_points[36].y + shape_points[45].y)*0.5f;
                face_center.x = (float)(shape_points[36].x + shape_points[45].x)*0.5f;
                face_center.y = (float)shape_points[30].y*1.0f;

                cv::Mat matDst2 = matDst1.clone();
                cv::Rect RectMat = cv::Rect(face_center.x - SizeSelect_192 / 2, Eye_Y - OffSet, SizeSelect_192, SizeSelect_192);
                if (RectMat.x >= 0 && RectMat.x < matDst2.cols && RectMat.y >= 0 && RectMat.y < matDst2.rows
                    && (RectMat.x + SizeSelect_192) >= 0 && (RectMat.x + SizeSelect_192) < matDst2.cols && (RectMat.y + SizeSelect_192) >= 0 && (RectMat.y + SizeSelect_192) < matDst2.rows)
                {
                    ResultImage_192 = matDst2(RectMat);
                }
        /*		if (Save224)
                {
                    cv::Rect rect_matDst1(face_center.x - (SizeSelect_224 / 2), face_center.y - (SizeSelect_224 / 2), SizeSelect_224, SizeSelect_224);
                    //cv::rectangle(matDst2, rect_matDst1, CV_RGB(255, 255, 0), 2);
                    //cv::circle(matDst2, face_center, 2, CV_RGB(255, 255, 0), 2);
                    if (rect_matDst1.x >= 0 && rect_matDst1.x < matDst1.cols && rect_matDst1.y >= 0 && rect_matDst1.y < matDst1.rows
                        && (rect_matDst1.x + SizeSelect_224) >= 0 && (rect_matDst1.x + SizeSelect_224) < matDst1.cols && (rect_matDst1.y + SizeSelect_224) >= 0 && (rect_matDst1.y + SizeSelect_224) < matDst1.rows)
                    {
                        ResultImage_224 = matDst1(rect_matDst1);
                        std::string time_now;
                        time_t now = time(0);
                        tm *ltm = localtime(&now);
                        std::string year = std::to_string(ltm->tm_year + 1900);
                        std::string month = std::to_string(ltm->tm_mon + 1);
                        std::string day = std::to_string(ltm->tm_mday);
                        std::string hour = std::to_string(ltm->tm_hour);
                        std::string min = std::to_string(ltm->tm_min);
                        std::string sec = std::to_string(ltm->tm_sec);
                        time_now = year + "-" + month + "-" + day + "-" + hour + "-" + min + "-" + sec + ".jpg";
                        //cv::imwrite("./Record/" + time_now, ResultImage_224);
                        //if (this->OpenUI)
                        //{
                        //	cv::Mat FinalShow = matDst3(rect_matDst1);
                        //	std::string time_now;
                        //	time_t now = time(0);
                        //	tm *ltm = localtime(&now);
                        //	std::string year = std::to_string(ltm->tm_year + 1900);
                        //	std::string month = std::to_string(ltm->tm_mon + 1);
                        //	std::string day = std::to_string(ltm->tm_mday);
                        //	std::string hour = std::to_string(ltm->tm_hour);
                        //	std::string min = std::to_string(ltm->tm_min);
                        //	std::string sec = std::to_string(ltm->tm_sec);
                        //	time_now = year + "-" + month + "-" + day + "-" + hour + "-" + min + "-" + sec + ".jpg";
                        //	cv::imwrite("./Record/" + time_now, FinalShow);
                        //}
                    }
                    //imshow("32142321", result_image_192);
                    //load_success = true;
                }*/
            }
        }
        return ResultImage_192;
    }





    /***************************************************************************/
    /*Socket*/
    /***************************************************************************/


    bool WriteImageTime(cv::Mat& Image)
    {
        //std::string year, month, date, hour, minute, second;
        if(Image.empty())
            return false;
        int pixel[6];
        tm *ptrnow;
        time_t loc_now = 0;
        time(&loc_now);
        ptrnow = localtime(&loc_now);
        pixel[0] = (ptrnow->tm_year + 1900) - 2000;
        pixel[1] = (ptrnow->tm_mon + 1);
        pixel[2] = (ptrnow->tm_mday);
        pixel[3] = (ptrnow->tm_hour);
        pixel[4] = (ptrnow->tm_min);
        pixel[5] = (ptrnow->tm_sec);
        for (int i = 0; i < 6; i++)
        {
            Image.at<int>(i, 1) = pixel[i];
        }
        return true;
    }




};//end of namespace








