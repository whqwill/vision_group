#include <iostream>
#include <string>
#include <fstream>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace std;
using namespace cv;

float * readFromBinary(const char * filename, int width, int height)
{
    float *data = new float[width*height];
    fstream stream(filename, ios::in |  std::ios::binary);
    stream.read((char*) data, width*height*sizeof(float));
    stream.close();
    return data;
}

void mat_to_vector(Mat in,vector<float> &out){

       for (int i=0; i < in.rows; i++) {
            for (int j =0; j < in.cols; j++){
                out.push_back(in.at<float>(i,j));
           }
       }
   }

void vector_to_mat(vector<float> in, Mat out,int cols , int rows){

    for (int i=rows-1; i >=0; i--) {
             for (int j =cols -1; j >=0; j--){
                 out.at<float>(i,j) = in.back();
                 in.pop_back();
            }
        }
}

int writeToBinary(const char* filename, const int width, const int height, Mat disp_m)
{
    vector<float> disp_v;
    mat_to_vector(disp_m,disp_v);
    float *disp_f = &disp_v[0];

    for (int i = 0; i < disp_v.size(); i++)
        if (disp_f[i] < 0)
            disp_f[i] = -10;
    ofstream out(filename, ios::out | ios::binary);
    if(!out) {
        cout << "Cannot open file.";
        return 1;
       }

    out.write((char *) disp_f, sizeof(float)*width*height);
    out.close();
    return 0;
}

int bm(char* leftFile, char* rightFile, char* dispLeftFile, char* dispRightFile, char* dispLeftBinFile, char* dispRightBinFile)
{    
    int numberOfDisparities = 128;
    int m_nMaxDisp = 128;

    StereoBM bm;

    Mat img1r = imread(leftFile, 0);
    Mat img2r = imread(rightFile, 0);
    Size img_size = img1r.size();

    Mat img1,img2;

    copyMakeBorder(img1r, img1, 0, 0, m_nMaxDisp, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(img2r, img2, 0, 0, m_nMaxDisp, 0, IPL_BORDER_REPLICATE);

    Mat disp, disp8u, disp_broaden, disp32f;;

    bm.state->preFilterCap = 31;
    bm.state->preFilterSize = 41;
    bm.state->SADWindowSize = 9;
    bm.state->textureThreshold = 20;
    bm.state->numberOfDisparities = numberOfDisparities;
    bm.state->uniquenessRatio = 10;

    //left disparity
    bm.state->minDisparity = 0;

    bm(img1, img2, disp_broaden);
    disp = disp_broaden.colRange(m_nMaxDisp, img1.cols);

    disp.convertTo(disp8u, CV_8U, 255/(numberOfDisparities*16.));
    disp.convertTo(disp32f, CV_32F, 255/(numberOfDisparities*16.));

    imwrite(dispLeftFile, disp8u);
    if (writeToBinary(dispLeftBinFile,img_size.width,img_size.height,disp32f))
        return 1;

    //right disparity
    //can't do that

    return 0;
}


int sgbm(char* leftFile, char* rightFile, char* dispLeftFile, char* dispRightFile, char* dispLeftBinFile, char* dispRightBinFile)
{

    int numberOfDisparities = 128;
    int m_nMaxDisp = 128;

    StereoSGBM sgbm;

    Mat img1r = imread(leftFile, 0);
    Mat img2r = imread(rightFile, 0);
    Size img_size = img1r.size();

    Mat img1,img2;

    copyMakeBorder(img1r, img1, 0, 0, m_nMaxDisp, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(img2r, img2, 0, 0, m_nMaxDisp, 0, IPL_BORDER_REPLICATE);

    Mat disp, disp8u, disp_broaden, disp32f;;

    sgbm.preFilterCap = 63;
    sgbm.SADWindowSize = 3;
    sgbm.P1 = 4*sgbm.SADWindowSize*sgbm.SADWindowSize;
    sgbm.P2 = 32*sgbm.SADWindowSize*sgbm.SADWindowSize;
    sgbm.numberOfDisparities = numberOfDisparities;
    sgbm.uniquenessRatio = 10;
    sgbm.speckleWindowSize = 100;
    sgbm.speckleRange = 32;
    sgbm.disp12MaxDiff = 1;
    sgbm.fullDP = 1;
    sgbm.minDisparity = 0;

    sgbm(img1, img2, disp_broaden);
    disp = disp_broaden.colRange(m_nMaxDisp, img1.cols);

    disp.convertTo(disp8u, CV_8U, 255/(numberOfDisparities*16.));
    disp.convertTo(disp32f, CV_32F, 255/(numberOfDisparities*16.));

    imwrite(dispLeftFile, disp8u);
    if (writeToBinary(dispLeftBinFile,img_size.width,img_size.height,disp32f))
        return 1;

    //right disparity
    //can't do that

    return 0;
}

// /home/aljosa/image%d.png
int main(int argc, char *argv[])
{

    // N params: algorithm, path(leftImage), path(rightImage), path(leftDisp), path(rightDisp), path(leftBin), path(rightBin), startFrame, endFrame
    cout << "begin.." << endl;

    if (argc<10) {
        cout << "Error(the number of parameters is at least 9)!" << endl;
        return 1;
    }

    int startFrame = stoi(string(argv[8]));
    int endFrame = stoi(string(argv[9]));
    enum { STEREO_BM=0, STEREO_SGBM=1};
    int algorithm = -1;


    if (strcmp(argv[1],"-bm") == 0)
        algorithm = STEREO_BM;
    else if (strcmp(argv[1],"-sgbm") == 0)
    {
        algorithm = STEREO_SGBM;
    }
    else
    {
        cout << "Error(wrong algorithm parameter)!" << endl;
        return 1;
    }

    for (int i=startFrame; i<endFrame; i++) {
        cout << "processing picture" << i << endl;

        char leftImgeFile[500];
        char rightImgeFile[500];
        char leftDispFile[500];
        char rightDispFile[500];
        char leftDispBinFile[500];
        char rightDispBinFile[500];

        sprintf(leftImgeFile, argv[2], i);
        sprintf(rightImgeFile, argv[3], i);
        sprintf(leftDispFile, argv[4], i);
        sprintf(rightDispFile, argv[5], i);
        sprintf(leftDispBinFile, argv[6], i);
        sprintf(rightDispBinFile, argv[7], i);


        if (algorithm == STEREO_BM)
        {
            if (bm(leftImgeFile, rightImgeFile, leftDispFile, rightDispFile, leftDispBinFile, rightDispBinFile))
                return 1;
        }
        else
        {
            if (sgbm(leftImgeFile, rightImgeFile, leftDispFile, rightDispFile, leftDispBinFile, rightDispBinFile))
                return 1;
        }

    }

    cout << "finished" << endl;
    return 0;
}
