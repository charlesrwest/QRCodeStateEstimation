#include <iostream>
#include <iomanip>
#include <memory>

#include "../library/QRCodeStateEstimator.hpp"
#include<cmath>

int main(int argc, char **argv) 
{
///////////////////////A few tests to make sure strings are processed right////////////
std::string bufferString;
double      bufferDouble;

//Test string extraction
std::string testString0 = "12.3in-inches";
std::string expectedRemainder0 = "inches";
if(extractQRCodeDimensionFromString(testString0, bufferDouble, bufferString) != true)
{
fprintf(stderr, "Failed to read something we should have\n");
return 1;
}

if(bufferString != expectedRemainder0)
{
fprintf(stderr, "Failed extract identifier\n");
return 1;
}

if(fabs(bufferDouble - 0.31242) > .001)
{
printf("returned: %lf\n", bufferDouble);
fprintf(stderr, "Failed to extract double and convert to meters\n");
return 1;
}

//Test string extraction
std::string testString1 = "12.3mm-tester";
std::string expectedRemainder1 = "tester";
if(extractQRCodeDimensionFromString(testString1, bufferDouble, bufferString) != true)
{
fprintf(stderr, "Failed to read something we should have\n");
return 1;
}

if(bufferString != expectedRemainder1)
{
fprintf(stderr, "Failed extract identifier: %s\n", bufferString.c_str());
return 1;
}

if(fabs(bufferDouble - 0.0123) > .0001)
{
printf("returned: %lf\n", bufferDouble);
fprintf(stderr, "Failed to extract double and convert to meters\n");
return 1;
}

////////////////////////////////////End tests/////////////////////////////////////////////////

//Define camera matrix according to webcam calibration (from OpenCV camera calibration file)
cv::Mat_<double> cameraMatrix(3, 3);
cameraMatrix.at<double>(0,0) =  1.3442848643472917e+03;
cameraMatrix.at<double>(0,1) = 0.0;
cameraMatrix.at<double>(0,2) = 6.3950000000000000e+02;
cameraMatrix.at<double>(1,0) =  0.0;
cameraMatrix.at<double>(1,1) = 1.3442848643472917e+03;
cameraMatrix.at<double>(1,2) = 3.595e+02;
cameraMatrix.at<double>(2,0) = 0.0;
cameraMatrix.at<double>(2,1) = 0.0; 
cameraMatrix.at<double>(2,2) = 1.0;

//Define distortion parameters according to webcam calibration (from OpenCV camera calibration file)
cv::Mat_<double> distortionParameters(1, 5); //k1, k2, p1, p2, k3
distortionParameters.at<double>(0, 0) = 7.9440223269640672e-03;
distortionParameters.at<double>(0, 1) = -5.6562236732221527e-01;
distortionParameters.at<double>(0, 2) =  0.0;
distortionParameters.at<double>(0, 3) =  0.0;
distortionParameters.at<double>(0, 4) = 1.6991852512288661e+00;

//Make it so that you can set which camera to use as a video source
int cam_idx = 0;
//Set camera ID from argument if there is one
if (argc == 2) 
{
cam_idx = atoi(argv[1]);
}

//Open opencv camera video source
cv::VideoCapture cap(cam_idx);
if (!cap.isOpened()) 
{
fprintf(stderr, "Could not open camera.\n");
exit(EXIT_FAILURE);
}

//Make size same as calibration (Change to match your calibration)
cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

//Initialize the state estimator, while wrapping any exceptions so we know where it came from
std::unique_ptr<QRCodeStateEstimator> stateEstimator;
SOM_TRY
stateEstimator.reset(new QRCodeStateEstimator(1280, 720, cameraMatrix, distortionParameters, true));
SOM_CATCH("Error initializing state estimator\n")

//Initialize some variables we are going to use while processing frames
cv::Mat frame;
cv::Mat cameraPoseBuffer;
std::string QRCodeIdentifierBuffer;
double QRCodeDimensionBuffer;
bool thereIsANewFrame = false;

while(true)
{
// Capture an OpenCV frame from the camera
cap >> frame;

//Give the frame to the state estimator and try to get the camera's pose from the QR code image
SOM_TRY
thereIsANewFrame = stateEstimator->estimateStateFromBGRFrame(frame, cameraPoseBuffer, QRCodeIdentifierBuffer, QRCodeDimensionBuffer);
SOM_CATCH("Error estimating state\n")

//Print out values of camera's pose matrix
if(thereIsANewFrame)
{
printf("Camera position/orientation matrix:\n");
for(int row = 0; row < 4; row++)
{

for(int col = 0; col < 4; col ++)
{
printf("%lf ", cameraPoseBuffer.at<double>(row, col));
}
printf("\n");
}
}


} //End of frame processing loop


return 0;
}


