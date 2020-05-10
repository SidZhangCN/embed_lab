#include "SocketMatTransmissionClient.hpp"
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <zbar.h>
#include <sys/time.h>

#define WATERMARK_LENGTH 15
#define getbit(x, y) ((x) >> (y)&1)

using namespace std;
using namespace zbar;
using namespace cv;

// 字符串分割函数
vector<string> split(const string &str, const string &delim);
Mat addWaterMark(string waterMark, Mat image);

int main()
{
    SocketMatTransmissionClient socketMat;
    if (socketMat.socketConnect("192.168.3.15", 6666) < 0)
    {
        return 0;
    }

    VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    capture.set(CV_CAP_PROP_FPS, 20); //帧数
    // 实例化二维码类
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    Mat frame;
    Mat imgGray;
    Mat mirror;
    timeval start, end;
    string frameNumber("");

    while (1)
    {
        // gettimeofday(&start, NULL);
        if (!capture.isOpened())
        {
            cout << "no camera device!" << endl;
            return 0;
        }

        capture >> frame;
        cvtColor(frame, imgGray, CV_RGB2GRAY);
        flip(imgGray, mirror, 0);

        if (frame.empty())
        {
            cout << "frame is empty!" << endl;
            return 0;
        }

        int width = frame.cols;
        int height = frame.rows;
        Image image(width, height, "Y800", mirror.data, width * height);
        int n = scanner.scan(image);
        if (n > 0)
        {
            // 二维码的内容
            string qrCode("");
            for (Image::SymbolIterator symbol = image.symbol_begin();
                 symbol != image.symbol_end();
                 ++symbol)
            {
                qrCode.append(symbol->get_data());
            }
            // cout << "qrCode: " << qrCode << endl;

            // 二维码内容分割成帧编号和水印字符串
            vector<string> strTmp = split(qrCode, "-");

            // 如果摄像头拍摄的当前帧编号和上一帧相同，就跳出，进行下一次处理
            if (!frameNumber.compare(strTmp[0]))
            {
                continue;
            }
            // 未完待续
            // 下一步要做的是：将水印加到imgGray对象里，udp发送给客户端
            else
            {
                frameNumber = strTmp[0];
                cout << "frame number: " << frameNumber << endl;
                cout << "watermark:  " << strTmp[1] << endl;
                Mat afterWatermark = addWaterMark(qrCode, frame);
                socketMat.transmit(afterWatermark);
            }
        }
        
        // gettimeofday(&end, NULL);
        // int t = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
        // cout << "time: " << t << endl;
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
        string s = p;     //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }

    return res;
}

Mat addWaterMark(string waterMark, Mat image)
{
    Mat tempMat = image.clone();

    // Mat 前两个B、G通道最后一位置0
    for (int col = 0; col < 240; col++)
    {
        for (int row = 0; row < 320; row++)
        {
            tempMat.at<cv::Vec3b>(col, row)[0] = tempMat.at<cv::Vec3b>(col, row)[0] & 0b11111110;
            tempMat.at<cv::Vec3b>(col, row)[1] = tempMat.at<cv::Vec3b>(col, row)[1] & 0b11111110;
        }
    }

    // string转char[]
    char p[16];
    strncpy(p, waterMark.c_str(), waterMark.length() + 1);

    // LSB加水印
    for (uchar col = 0; col < 30; col++)
    {
        for (uchar row = 0; row < 40; row++)
        {
            // printf("%d,%d ", col * 8, row * 8);
            for (uchar i = 0; i < 8; i++)
            {
                for (uchar j = 0; j < 8; j++)
                {
                    if (getbit(p[i], j) == 1)
                    {
                        tempMat.at<cv::Vec3b>(col * 8 + i, row * 8 + j)[0]++;
                    }
                }
            }
            for (uchar i = 0; i < 8; i++)
            {
                for (uchar j = 0; j < 8; j++)
                {
                    if (getbit(p[i + 8], j) == 1)
                    {
                        tempMat.at<cv::Vec3b>(col * 8 + i, row * 8 + j)[1]++;
                    }
                }
            }
        }
    }

    return tempMat;
}