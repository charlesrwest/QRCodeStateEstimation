#ifndef  QRCODESTATEESTIMATORHPP
#define QRCODESTATEESTIMATORHPP

#include<string>
#include<algorithm>
#include<map>

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <zbar.h>

//Declare handy constants
static const std::string QRCodeStateEstimatorWindowTitle = "QR Code State Estimator";
static const std::map<std::string, double> unitIdentifierToMetricMeterConversionFactor = {{"m-", 1.0}, {"cm-", .01}, {"mm-", .001}, {"ft-", .3048}, {"in-", .0254}};


/*
This class takes cv::Mats which represents images from a camera which is hopefully pointed at a QR code.  If there is a QR code with its size (assumed square, size is the length of one side) embedded in the code text in the file, it will return the position and orientation of the camera in the coordinate system described by the QR code.
*/
class QRCodeStateEstimator
{
public:
/*
This function initializes the state estimator with the OpenCV camera calibration parameter so that it can determine pose using the camera parameters.
@param inputCameraImageWidth: The width of camera images used in the camera calibration
@param inputCameraImageHeight: The height of the camera images used in the camera calibration
@param inputCameraCalibrationMatrix: This is a 3x3 matrix that describes the camera transform (taking the distortion into account) in opencv format
@param inputDistortionParameters: a 1x5 matrix which has the distortion parameters k1, k2, p1, p2, k3
@param inputShowResultsInWindow: True if you would like the QR results to be shown in a window

@exception: This function can throw exceptions
*/
QRCodeStateEstimator(int inputCameraImageWidth, int inputCameraImageHeight, const cv::Mat_<double> &inputCameraCalibrationMatrix, const cv::Mat_<double> &inputCameraDistortionParameters, bool inputShowResultsInWindow = false);

/*
This function takes a BGR frame of the appropriate size, scans for a QR code with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the pose of the camera (OpenCV format) relative to the coordinate system of the QR tag in the provided buffer.  If multiple tags are recognized, it will only return the information for the first.
@param inputBGRFrame: The frame to process (should be same size as calibration)
@param inputCameraPoseBuffer: The buffer to place the 4x4 camera pose matrix in
@param inputQRCodeIdentifierBuffer: A buffer to place left text from the QR code after the dimension information has been removed
@param inputQRCodeDimensionBuffer: A buffer to place size of the QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool estimateStateFromBGRFrame(const cv::Mat &inputBGRFrame, cv::Mat &inputCameraPoseBuffer, std::string &inputQRCodeIdentifierBuffer, double &inputQRCodeDimensionBuffer);

/*
This function takes a grayscale frame of the appropriate size, scans for a QR code with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the pose of the camera (OpenCV format) relative to the coordinate system of the QR tag in the provided buffer.  If multiple tags are recognized, it will only return the information for the first.
@param inputGrayscaleFrame: The frame to process (should be same size as calibration)
@param inputCameraPoseBuffer: The buffer to place the 4x4 camera pose matrix in
@param inputQRCodeIdentifierBuffer: A buffer to place left text from the QR code after the dimension information has been removed
@param inputQRCodeDimensionBuffer: A buffer to place size of the QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool estimateStateFromGrayscaleFrame(const cv::Mat &inputGrayscaleFrame, cv::Mat &inputCameraPoseBuffer, std::string &inputQRCodeIdentifierBuffer, double &inputQRCodeDimensionBuffer);

/*
This function takes a BGR frame of the appropriate size, scans for a QR codes with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the poses of the camera (OpenCV format) relative to the different coordinate systems of the QR tags in the provided buffer.
@param inputBGRFrame: The frame to process (should be same size as calibration)
@param inputCameraPosesBuffer: The buffer to place the 4x4 camera pose matrices in
@param inputQRCodeIdentifiersBuffer: A buffer to place the text left from each QR code after the dimension information has been removed
@param inputQRCodeDimensionsBuffer: A buffer to place size of each QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool estimateOneOrMoreStatesFromBGRFrame(const cv::Mat &inputBGRFrame, std::vector<cv::Mat> &inputCameraPosesBuffer, std::vector<std::string> &inputQRCodeIdentifiersBuffer, std::vector<double> &inputQRCodeDimensionsBuffer);

/*
This function takes a grayscale frame of the appropriate size, scans for any QR codes with an embedded sizes (recognized decimal formats: ft, in, cm, mm, m), and stores the poses of the camera (OpenCV format) relative to the different coordinate systems of the QR tags in the provided buffers.
@param inputGrayscaleFrame: The frame to process (should be same size as calibration)
@param inputCameraPosesBuffer: The buffer to place the 4x4 camera pose matrices in
@param inputQRCodeIdentifiersBuffer: A buffer to place the text left from each QR code after the dimension information has been removed
@param inputQRCodeDimensionsBuffer: A buffer to place size of each QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool estimateOneOrMoreStatesFromGrayscaleFrame(const cv::Mat &inputGrayscaleFrame, std::vector<cv::Mat> &inputCameraPosesBuffer, std::vector<std::string> &inputQRCodeIdentifiersBuffer, std::vector<double> &inputQRCodeDimensionsBuffer);



int expectedCameraImageWidth;
int expectedCameraImageHeight;
cv::Mat_<double> cameraMatrix;  //3x3 matrix
cv::Mat_<double> distortionParameters; //1x5 matrix
bool showResultsInWindow; //True if the image should be shown in a window
zbar::ImageScanner zbarScanner;
cv::Mat frameBuffer;
};

/*
This function takes a string in the format "dimensionIdentifier" (for example, "12.0in-FKDJL") and stores the dimension from the string in meters and the remainder.  In the example case, it would store 0.3048 and "FKDJL".  It supports the following extensions and is case insensitive: "m-", "cm-", "mm-", "ft-", "in-".
@param inputQRCodeString: The original string
@param inputDimensionBuffer: The buffer to store the extracted dimension (in meters) in
@param inputIdentifierBuffer: The remainder of the string after the dimension has been extracted
@return: true if it was possible to extract the dimension and false otherwise
*/
bool extractQRCodeDimensionFromString(const std::string &inputQRCodeString, double &inputDimensionBuffer, std::string &inputIdentifierBuffer);



/*
Could potentially add support for scanning multiple QR codes in the same frame.
*/


#endif
