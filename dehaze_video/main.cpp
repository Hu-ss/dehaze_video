#include <iostream>
#include <opencv2/opencv.hpp>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<vector>
#include <algorithm>
#include<set>
#include<map>

using namespace cv;
using namespace std;

//暗通道计算
Mat darkChannel(Mat W, int scale)
{
	Mat rgbmin = Mat::zeros(W.rows, W.cols, CV_64FC1);
	int nc1 = W.rows* W.cols;
	for (int j = 0; j<1; j++)
	{
		double* data1 = rgbmin.ptr<double>(j);
		double* data2 = W.ptr<double>(j);
		for (int i = 0; i<nc1; i++)
		{
			*data1++ = min(min(*data2++, *data2++), *data2++);
		}
	}
	//最小值滤波
	Mat D_s(W.rows, W.cols, CV_64FC1);
	int radius = (scale - 1) / 2;
	Mat border;
	copyMakeBorder(rgbmin, border, radius, radius, radius, radius, BORDER_REFLECT_101);
	for (int j = 0; j<W.rows; j++)
	{
		double* data1 = D_s.ptr<double>(j);
		for (int i = 0; i<W.cols; i++)
		{
			//选取兴趣区域
			Mat roi;
			roi = border(Rect(i, j, scale, scale));
			//求兴趣区域的最小值
			double minVal = 0; double maxVal = 0;
			Point minLoc = 0; Point maxLoc = 0;
			minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc, noArray());
			*data1++ = (double)minVal;
		}
	}

	return D_s;
}
//估计大气光强度
double calculate_A(Mat WW, Mat D_s)
{
	double sum_A = 0;
	double n = (D_s.cols)*(D_s.rows) / 1000;
	Mat temp; D_s.copyTo(temp);
	for (int i = 0; i < n; i++)
	{
		double minVal = 0; double maxVal = 0;
		Point minLoc = 0; Point maxLoc = 0;
		minMaxLoc(temp, &minVal, &maxVal, &minLoc, &maxLoc, noArray());

		sum_A = sum_A + ((WW.at<Vec3d>(maxLoc)[0] + WW.at<Vec3d>(maxLoc)[1] + WW.at<Vec3d>(maxLoc)[2]) / 3.0);
		//sum_A = sum_A + (W.at<double>(maxLoc));
		temp.at<double>(maxLoc) = temp.at<double>(minLoc);
	}
	double A = sum_A / (int)n;
	double max_A = 220.0 / 255;
	if (A > max_A) {
		A = max_A;
	}

	return A;
}
//递归滤波
Mat TransformedDomainRecursiveFilter_Horizontal(Mat I, Mat D, double sigma)
{
	Mat F;
	I.copyTo(F);
	double a;
	a = exp(-sqrt(2) / sigma);

	Mat V(D.rows, D.cols, CV_64FC1);
	int nc1 = D.rows* D.cols;
	for (int j = 0; j<1; j++)
	{
		double* data1 = V.ptr<double>(j);
		double* data2 = D.ptr<double>(j);
		for (int i = 0; i<nc1; i++)
		{
			*data1++ = pow(a, (*data2++));
		}
	}

	//从左往右滤波
	int nl2 = I.rows;
	int nc2 = (I.cols - 1);
	for (int j = 0; j<nl2; j++)
	{
		double* data1 = F.ptr<double>(j);
		double* data2 = V.ptr<double>(j);
		for (int i = 0; i<nc2; i++)
		{
			data1[i + 1] = data1[i + 1] + (data2[i + 1] * (data1[i] - data1[i + 1]));
		}
	}

	//从右往左滤波
	int nl3 = I.rows;
	int nc3 = (I.cols - 2);
	for (int j = 0; j<nl3; j++)
	{
		double* data1 = F.ptr<double>(j);
		double* data2 = V.ptr<double>(j);
		for (int i = nc3; i>-1; i--)
		{
			data1[i] = data1[i] + (data2[i + 1] * (data1[i + 1] - data1[i]));
		}
	}
	return F;
}
//域变换递归滤波
Mat RF(Mat I, double sigma_s, double sigma_r, double num_iterations, Mat J)
{

	//计算定义域变换
	Mat dIcdx(J.rows, (J.cols - 1), CV_64FC3);
	Mat dIcdy((J.rows - 1), J.cols, CV_64FC3);

	int nl1 = dIcdx.rows;
	int nc1 = dIcdx.cols*dIcdx.channels();
	for (int j = 0; j<nl1; j++)
	{
		double* data1 = dIcdx.ptr<double>(j);
		double* data2 = J.ptr<double>(j);
		for (int i = 0; i<nc1; i++)
		{
			data1[i] = data2[i + 3] - data2[i];
		}
	}

	int nl2 = dIcdy.rows;
	int nc2 = dIcdy.cols*dIcdy.channels();
	for (int j = 0; j<nl2; j++)
	{
		double* data1 = dIcdy.ptr<double>(j);
		double* data2 = J.ptr<double>(j);
		double* data3 = J.ptr<double>(j + 1);
		for (int i = 0; i<nc2; i++)
		{
			//*data1++ = (*data3++) - (*data2++);
			data1[i] = data3[i] - data2[i];
		}
	}

	Mat dIdx = Mat::zeros(J.rows, J.cols, CV_64FC1);
	Mat dIdy = Mat::zeros(J.rows, J.cols, CV_64FC1);

	int nl3 = dIcdx.rows;
	int nc3 = dIcdx.cols;
	for (int j = 0; j<nl3; j++)
	{
		double* data1 = dIdx.ptr<double>(j);
		double* data2 = dIcdx.ptr<double>(j);
		for (int i = 0; i<nc3; i++)
		{
			data1[i + 1] = data1[i + 1] + abs(*data2++) + abs(*data2++) + abs(*data2++);
		}
	}

	int nl4 = dIcdy.rows;
	int nc4 = dIcdy.cols;
	for (int j = 0; j<nl4; j++)
	{
		double* data1 = dIdy.ptr<double>(j + 1);
		double* data2 = dIcdy.ptr<double>(j);
		for (int i = 0; i<nc4; i++)
		{
			data1[i] = data1[i] + abs(*data2++) + abs(*data2++) + abs(*data2++);
		}
	}


	Mat dHdx = Mat::zeros(J.rows, J.cols, CV_64FC1);
	Mat dVdy = Mat::zeros(J.rows, J.cols, CV_64FC1);
	int nc5 = J.rows*J.cols;
	for (int j = 0; j<1; j++)
	{
		double* data1 = dHdx.ptr<double>(j);
		double* data2 = dIdx.ptr<double>(j);
		double* data3 = dVdy.ptr<double>(j);
		double* data4 = dIdy.ptr<double>(j);
		for (int i = 0; i<nc5; i++)
		{
			*data1++ = 1 + ((sigma_s / sigma_r) * (*data2++));
			*data3++ = 1 + ((sigma_s / sigma_r) * (*data4++));
		}
	}
	//进行转置
	dVdy = dVdy.t();
	//执行过滤
	double N = num_iterations;
	Mat F;
	I.copyTo(F);

	double sigma_H;
	for (int k = 0; k < 3; k++)
	{
		sigma_H = (sigma_s * sqrt(3) * pow(2, (N - (k + 1)))) / sqrt(pow(4, N) - 1);
		F = TransformedDomainRecursiveFilter_Horizontal(F, dHdx, sigma_H);
		F = F.t();

		F = TransformedDomainRecursiveFilter_Horizontal(F, dVdy, sigma_H);
		F = F.t();

	}
	return F;
}
//伽马变换
uchar lut[256];
void getlut(double gamma)
{
	for (int i = 0; i < 256; i++)
	{
		lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
	}
}
Mat imadjust(Mat I, int low_in, int high_in, int low_out, int high_out)
{
	int nl = 1;
	int nc = I.cols*I.rows*I.channels();
	Mat temp;
	I.copyTo(temp);

	for (int j = 0; j < nl; j++)
	{
		uchar* data1 = temp.ptr<uchar>(j);
		for (int i = 0; i < nc; i++)
		{
			if (*data1 <= low_in)
			{
				*data1++ = lut[low_out];
			}
			else if (*data1 >= high_in)
			{
				*data1++ = lut[high_out];
			}
			else
			{
				*data1 = lut[(*data1)];
				*data1++;
			}
		}
	}
	return temp;
}

int main(int argc, char*argv[])
{
	getlut(0.9);
	namedWindow("去雾视频", 0);
	namedWindow("原视频", 0);
	//用于获取背景
	CvCapture* capture = cvCreateFileCapture("../video/1.mov");
	IplImage* frame;
	//用于每帧图像去雾
	CvCapture* cap = cvCreateFileCapture("../video/1.mov");
	IplImage* fra;

	VideoWriter write;
	string outVideo = "1.avi";
	int height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);//视频帧的宽度
	int width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);//视频帧的高度
	int fps = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	write.open(outVideo, CV_FOURCC('X', 'V', 'I', 'D'), fps, Size(width, height), true);

	IplImage * img_sum = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
	cvZero(img_sum);

	IplImage * img_sum_gray = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	int i = 1;
	int num = 90;
	int nc1 = height*width;
	int nc2 = height*width / 100;
	Mat image, image1, W, D_s, everycupture;;
	Mat D_1(height / 10, width / 10, CV_64FC1);
	Mat dahazed(height, width, CV_64FC3);
	double A;
	double time_sum = 0;
	double start, time;
	Mat J;
	while (1)
	{
		if (i > (numFrames + num))
		{
			cout << time_sum << endl;
			break;
		}
		start = static_cast<double>(getTickCount());
		//cout << i << endl;
		frame = cvQueryFrame(capture);
		if (frame)
		{
			cvAcc(frame, img_sum);
			if ((i%num) == 0)
			{
				//cout << "计算A" << endl;
				cvConvertScale(img_sum, img_sum_gray, 1.0 / num);
				image = cvarrToMat(img_sum_gray, true);

				image.convertTo(image1, CV_64FC3, 1.0 / 255, 0); //转化为double形式
				resize(image1, W, Size(image.cols / 10, image.rows / 10), INTER_AREA);

				//暗通道
				D_s = darkChannel(W, 5);

				//计算大气光强A
				A = calculate_A(W, D_s);

				//double A = 0.53;
				//域变换递归滤波
				D_s = RF(D_s, 30, 0.8, 3, W);

				//求取透射率D_1
				for (int j = 0; j < 1; j++)
				{
					double* data1 = D_1.ptr<double>(j);
					double* data2 = D_s.ptr<double>(j);
					for (int i = 0; i < nc2; i++)
					{
						*data1++ = 1 - (*data2++);
					}
				}
				resize(D_1, D_1, Size(image.cols, image.rows), INTER_LINEAR);
				cvZero(img_sum);
			}
		}


		if (i > num)
		{
			fra = cvQueryFrame(cap);
			everycupture = cvarrToMat(fra, true);

			imshow("原视频", everycupture);

			everycupture.convertTo(everycupture, CV_64FC3, 1.0 / 255, 0); //转化为double形式
																		  //图像复原
			for (int j = 0; j < 1; j++)
			{
				double* data1 = dahazed.ptr<double>(j);
				double* data2 = everycupture.ptr<double>(j);
				double* data3 = D_1.ptr<double>(j);
				for (int i = 0; i < nc1; i++)
				{
					*data1++ = (((*data2++) - A) / (*data3)) + A;
					*data1++ = (((*data2++) - A) / (*data3)) + A;
					*data1++ = (((*data2++) - A) / (*data3++)) + A;
				}
			}
			dahazed.convertTo(J, CV_8UC3, 255, 0);
			J = imadjust(J, 0, 255, 0, 255);

			write.write(J);
			imshow("去雾视频", J);
			waitKey(1);
			if (i == (200 + 150))
			{
				break;
			}
		}

		time = ((double)getTickCount() - start) / getTickFrequency();
		time_sum += time;
		cout << "所用时间为：" << time << "秒" << endl;
		i++;

	}

	return 0;
}
