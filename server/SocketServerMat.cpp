#include "SocketMatTransmissionServer.hpp"
#include "./CJsonObject.hpp"
#include <fstream>
using namespace std;

vector<string> split(const string &str, const string &delim);
string getWaterMark(Mat image);
void FormatTime(time_t time1, char *szTime);

int main()
{
	SocketMatTransmissionServer socketMat;
	if (socketMat.socketConnect(6666) < 0)
	{
		return 0;
	}
	cv::Mat image;
	time_t lt = time(NULL);
	char timeNow[30];
	FormatTime(lt, timeNow);
	string filePath = "./";
	filePath = filePath + timeNow;
	filePath = filePath + ".json";
	// int cnt = 276;
	while (socketMat.isConnect())
	{
		ofstream openfile(filePath.c_str(), std::ios::app);
		neb::CJsonObject oJson;
		if (socketMat.receive(image) > 0)
		{
			cv::imshow("server", image);

			string watermark = getWaterMark(image);
			cout << "watermark: " << watermark << endl;
			vector<string> strTmp = split(watermark, "-");
			oJson.AddEmptySubArray(strTmp[0].c_str());
			oJson[strTmp[0].c_str()].Add(neb::CJsonObject("{}"));
			oJson[strTmp[0].c_str()][0].Add("watermark", strTmp[1].c_str());

			/*
				形状识别
			*/
			Mat mask = Mat::zeros(image.size(), CV_8UC1);
			Mat inputImage;
			vector<vector<Point>> cont;
			vector<Point2i> points;
			points.push_back(Point2i(110, 0));
			points.push_back(Point2i(320, 0));
			points.push_back(Point2i(320, 240));
			points.push_back(Point2i(0, 240));
			points.push_back(Point2i(0, 100));
			points.push_back(Point2i(110, 100));
			// points.push_back(Point2i(320, 0));
			cont.push_back(points);
			drawContours(mask, cont, 0, Scalar::all(255), -1);
			// imshow("src", image);
			image.copyTo(inputImage, mask);
			// imshow("noqrcode", inputImage);

			Mat tmpImage;
			cvtColor(inputImage, tmpImage, CV_BGRA2BGR);
			tmpImage.convertTo(inputImage, CV_8U);

			Mat otherImage = inputImage.clone();
			Mat resultImage = inputImage.clone();

			Mat otherGrayImage;
			cvtColor(otherImage, otherGrayImage, CV_BGR2GRAY);

			// 图像二值化
			Mat otherThresholdImage;
			threshold(otherGrayImage, otherThresholdImage, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
			imshow("二值化结果", otherThresholdImage);

			vector<vector<Point>> counters;
			vector<Vec4i> hierarchy;

			// 轮廓检测
			findContours(otherThresholdImage, counters, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
			vector<vector<Point>> countPloy(counters.size());

			// 定义三种颜色
			const Scalar blue = Scalar(255, 0, 0);
			const Scalar green = Scalar(0, 255, 0);
			const Scalar red = Scalar(0, 0, 255);
			// 计算色彩差的阈值
			int coclorValue = 5;

			// 处理发现的轮廓
			Mat drawing = Mat::zeros(resultImage.size(), CV_8UC3);
			int circleCnt = 0, triangleCnt = 0, quadrilateralCnt = 0, pentagonCnt = 0, hexagonCnt = 0;

			for (int i = 0; i < counters.size(); i++)
			{
				// 轮廓周长
				double epsilon = 0.02 * arcLength(counters[i], true);

				// 多边拟合
				approxPolyDP(counters[i], countPloy[i], epsilon, true);

				// 计算中心距
				Moments moment;
				moment = moments(counters[i]);
				int x = (int)(moment.m10 / moment.m00);
				int y = (int)(moment.m01 / moment.m00);

				if (x < 90 && y < 90)
				{
					continue;
				}

				if (countPloy[i].size() == 3)
				{
					// 画轮廓
					drawContours(otherImage, counters, i, blue, 2);
					drawContours(resultImage, counters, i, blue, 2);

					// 中心点坐标
					string tempText = "<" + to_string(x) + ", " + to_string(y) + ">";
					putText(otherImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);
					putText(resultImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);

					// 绘制中心点
					circle(otherImage, Point(x, y), 2, red, -1);
					circle(resultImage, Point(x, y), 2, red, -1);

					// print
					// cout << "位于" << to_string(x) << ", " << to_string(y) << ">"
					// 	 << "triangle detect! " << endl;

					triangleCnt++;
				}
				else if (countPloy[i].size() == 4)
				{
					// 画轮廓
					drawContours(otherImage, counters, i, blue, 2);
					drawContours(resultImage, counters, i, blue, 2);

					// 中心点坐标
					string tempText = "<" + to_string(x) + ", " + to_string(y) + ">";
					putText(otherImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);
					putText(resultImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);

					// 绘制中心点
					circle(otherImage, Point(x, y), 2, red, -1);
					circle(resultImage, Point(x, y), 2, red, -1);

					// print
					// cout << "位于" << to_string(x) << ", " << to_string(y) << ">"
					// 	 << "四边形 detect! " << endl;
					quadrilateralCnt++;
				}
				else if (countPloy[i].size() == 5)
				{
					// 画轮廓
					drawContours(otherImage, counters, i, green, 2);
					drawContours(resultImage, counters, i, green, 2);

					// 中心点坐标
					string tempText = "<" + to_string(x) + ", " + to_string(y) + ">";
					putText(otherImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);
					putText(resultImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);

					// 绘制中心点
					circle(otherImage, Point(x, y), 2, red, -1);
					circle(resultImage, Point(x, y), 2, red, -1);

					// print
					// cout << "位于" << to_string(x) << ", " << to_string(y) << ">"
					// 	 << "五边形 detect! " << endl;
					pentagonCnt++;
				}
				else if (countPloy[i].size() == 6)
				{
					// 画轮廓
					drawContours(otherImage, counters, i, green, 2);
					drawContours(resultImage, counters, i, green, 2);

					// 中心点坐标
					string tempText = "<" + to_string(x) + ", " + to_string(y) + ">";
					putText(otherImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);
					putText(resultImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);

					// 绘制中心点
					circle(otherImage, Point(x, y), 2, red, -1);
					circle(resultImage, Point(x, y), 2, red, -1);

					// print
					// cout << "位于" << to_string(x) << ", " << to_string(y) << ">"
					// 	 << "六边形 detect! " << endl;
					hexagonCnt++;
				}
				else if (countPloy[i].size() >= 8)
				{
					// 画轮廓
					drawContours(otherImage, counters, i, green, 2);
					drawContours(resultImage, counters, i, green, 2);

					// 中心点坐标
					string tempText = "<" + to_string(x) + ", " + to_string(y) + ">";
					putText(otherImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);
					putText(resultImage, tempText, Point(x, y), FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 255), 1, 8, false);

					// 绘制中心点
					circle(otherImage, Point(x, y), 2, red, -1);
					circle(resultImage, Point(x, y), 2, red, -1);

					// print
					// cout << "位于" << to_string(x) << ", " << to_string(y) << ">"
					// 	 << "circle detect! " << endl;
					circleCnt++;
				}
				else
				{
				}
			}
			oJson[strTmp[0].c_str()].Add(neb::CJsonObject("{}"));
			oJson[strTmp[0].c_str()][1].Add("amount", circleCnt + triangleCnt + quadrilateralCnt + pentagonCnt + hexagonCnt);
			cout << endl;
			// int circleCnt = 0, triangleCnt = 0, quadrilateralCnt = 0, pentagonCnt = 0, hexagonCnt = 0;
			// cout << circleCnt << " " << triangleCnt << " " << quadrilateralCnt << " " << pentagonCnt << " " << hexagonCnt << endl;
			oJson[strTmp[0].c_str()].Add(neb::CJsonObject("{}"));
			if (circleCnt > 0)
			{
				oJson[strTmp[0].c_str()][2].Add("circle", circleCnt);
			}
			if (triangleCnt > 0)
			{
				oJson[strTmp[0].c_str()][2].Add("triangle", triangleCnt);
			}
			if (quadrilateralCnt > 0)
			{
				oJson[strTmp[0].c_str()][2].Add("quadrilateral", quadrilateralCnt);
			}
			if (pentagonCnt)
			{
				oJson[strTmp[0].c_str()][2].Add("pentagon", pentagonCnt);
			}
			if (hexagonCnt)
			{
				oJson[strTmp[0].c_str()][2].Add("hexagon", hexagonCnt);
			}

			imshow("检测结果", resultImage);

			cv::waitKey(10);
		}
		string result = oJson.ToString();
		cout << result << endl;
		openfile << result << ",";
		openfile.close();
	}

	socketMat.socketDisconnect();
	return 0;
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	if ("" == str)
		return res;
	//先将要切割的字符串从string类型转换为char*类型
	char *strs = new char[str.length() + 1]; //不要忘了
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;	 //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}

	return res;
}

string getWaterMark(Mat image)
{
	Mat tempMat = image.clone();
	string waterMark;

	for (uchar i = 0; i < 8; i++)
	{
		char ch = 0b00000000;
		for (uchar j = 0; j < 8; j++)
		{
			ch = (((int)(tempMat.at<cv::Vec3b>(i, j)[0]) & 0b00000001) << j) | ch;
		}
		waterMark = waterMark + ch;
	}
	for (uchar i = 0; i < 8; i++)
	{
		char ch = 0b00000000;
		for (uchar j = 0; j < 8; j++)
		{
			ch = (((int)(tempMat.at<cv::Vec3b>(i, j)[1]) & 0b00000001) << j) | ch;
		}
		waterMark = waterMark + ch;
	}

	return waterMark;
}

void FormatTime(time_t time1, char *szTime)
{
	struct tm tm1;
	localtime_r(&time1, &tm1);
	sprintf(szTime, "%4.4d-%2.2d-%2.2d_%2.2d:%2.2d:%2.2d",
			tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
			tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
}