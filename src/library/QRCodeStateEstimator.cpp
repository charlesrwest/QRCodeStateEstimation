#include "QRCodeStateEstimator.hpp" 

/*
This function initializes the state estimator with the OpenCV camera calibration parameter so that it can determine pose using the camera parameters.
@param inputCameraImageWidth: The width of camera images used in the camera calibration
@param inputCameraImageHeight: The height of the camera images used in the camera calibration
@param inputCameraCalibrationMatrix: This is a 3x3 matrix that describes the camera transform (taking the distortion into account) in opencv format
@param inputDistortionParameters: a 1x5 matrix which has the distortion parameters k1, k2, p1, p2, k3
@param inputShowResultsInWindow: True if you would like the QR results to be shown in a window

@exception: This function can throw exceptions
*/
QRCodeStateEstimator::QRCodeStateEstimator(int inputCameraImageWidth, int inputCameraImageHeight, const cv::Mat_<double> &inputCameraCalibrationMatrix, const cv::Mat_<double> &inputCameraDistortionParameters, bool inputShowResultsInWindow)
{
//Check inputs
if(inputCameraImageWidth <= 0 || inputCameraImageHeight <= 0)
{
throw SOMException(std::string("Camera image dimensions invalid\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Check camera calibration matrix dimensions
if(inputCameraCalibrationMatrix.dims != 2)
{
throw SOMException(std::string("Camera calibration matrix is not 3x3\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

for(int i=0; i < inputCameraCalibrationMatrix.dims; i++)
{
if(inputCameraCalibrationMatrix.size[i] != 3)
{
throw SOMException(std::string("Camera calibration matrix is not 3x3\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}
}

//Check distortion coefficients size
if(inputCameraDistortionParameters.dims != 2)
{
throw SOMException(std::string("Distortion coefficents vector is not 1x5\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

if(inputCameraDistortionParameters.size[1] != 5 || inputCameraDistortionParameters.size[0] != 1)
{
throw SOMException(std::string("Distortion coefficents vector is not 1x5\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

expectedCameraImageWidth = inputCameraImageWidth;
expectedCameraImageHeight = inputCameraImageHeight;

cameraMatrix = inputCameraCalibrationMatrix;
distortionParameters = inputCameraDistortionParameters;
showResultsInWindow = inputShowResultsInWindow;

//Configure the QR code reader object
zbarScanner.set_config(zbar::ZBAR_QRCODE , zbar::ZBAR_CFG_ENABLE, 1);
zbarScanner.enable_cache(false); //Set it so that it will show QR code result even if it was in the last frame

//Create window to show results, if we are suppose to
if(showResultsInWindow == true)
{
cv::namedWindow(QRCodeStateEstimatorWindowTitle, CV_WINDOW_AUTOSIZE);
}
}

/*
This function takes a BGR frame of the appropriate size, scans for a QR code with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the pose of the camera (OpenCV format) relative to the coordinate system of the QR tag in the provided buffer.  If multiple tags are recognized, it will only return the information for the first.
@param inputBGRFrame: The frame to process (should be same size as calibration)
@param inputCameraPoseBuffer: The buffer to place the 4x4 camera pose matrix in
@param inputQRCodeIdentifierBuffer: A buffer to place left text from the QR code after the dimension information has been removed
@param inputQRCodeDimensionBuffer: A buffer to place size of the QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool QRCodeStateEstimator::estimateStateFromBGRFrame(const cv::Mat &inputBGRFrame, cv::Mat &inputCameraPoseBuffer, std::string &inputQRCodeIdentifierBuffer, double &inputQRCodeDimensionBuffer)
{
if(inputBGRFrame.channels() != 3)
{
throw SOMException(std::string("Given frame is not BGR\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Convert the frame to grayscale
cvtColor(inputBGRFrame, frameBuffer, CV_BGR2GRAY);

//Get the pose using the grayscale version
SOM_TRY
return estimateStateFromGrayscaleFrame(frameBuffer, inputCameraPoseBuffer, inputQRCodeIdentifierBuffer, inputQRCodeDimensionBuffer);
SOM_CATCH("Error calculating pose from image\n")
}

/*
This function takes a grayscale frame of the appropriate size, scans for a QR code with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the pose of the camera (OpenCV format) relative to the coordinate system of the QR tag in the provided buffer.  If multiple tags are recognized, it will only return the information for the first.
@param inputGrayscaleFrame: The frame to process (should be same size as calibration)
@param inputCameraPoseBuffer: The buffer to place the 4x4 camera pose matrix in
@param inputQRCodeIdentifierBuffer: A buffer to place left text from the QR code after the dimension information has been removed
@param inputQRCodeDimensionBuffer: A buffer to place size of the QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool QRCodeStateEstimator::estimateStateFromGrayscaleFrame(const cv::Mat &inputGrayscaleFrame, cv::Mat &inputCameraPoseBuffer, std::string &inputQRCodeIdentifierBuffer, double &inputQRCodeDimensionBuffer)
{
std::vector<cv::Mat> cameraPosesBuffer;
std::vector<std::string> QRCodeIdentifiersBuffer;
std::vector<double> QRCodeDimensionBuffer;
bool returnValue;

returnValue = estimateOneOrMoreStatesFromGrayscaleFrame(inputGrayscaleFrame, cameraPosesBuffer, QRCodeIdentifiersBuffer, QRCodeDimensionBuffer);

if(cameraPosesBuffer.size() > 0)
{
inputCameraPoseBuffer = cameraPosesBuffer[0];
inputQRCodeIdentifierBuffer = QRCodeIdentifiersBuffer[0];
inputQRCodeDimensionBuffer = QRCodeDimensionBuffer[0];
}

return returnValue;
}

/*
This function takes a BGR frame of the appropriate size, scans for a QR codes with an embedded size (recognized decimal formats: ft, in, cm, mm, m), and stores the poses of the camera (OpenCV format) relative to the different coordinate systems of the QR tags in the provided buffer.
@param inputBGRFrame: The frame to process (should be same size as calibration)
@param inputCameraPosesBuffer: The buffer to place the 4x4 camera pose matrices in
@param inputQRCodeIdentifiersBuffer: A buffer to place the text left from each QR code after the dimension information has been removed
@param inputQRCodeDimensionsBuffer: A buffer to place size of each QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool QRCodeStateEstimator::estimateOneOrMoreStatesFromBGRFrame(const cv::Mat &inputBGRFrame, std::vector<cv::Mat> &inputCameraPosesBuffer, std::vector<std::string> &inputQRCodeIdentifiersBuffer, std::vector<double> &inputQRCodeDimensionsBuffer)
{
if(inputBGRFrame.channels() != 3)
{
throw SOMException(std::string("Given frame is not BGR\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Convert the frame to grayscale
cvtColor(inputBGRFrame, frameBuffer, CV_BGR2GRAY);

//Get the pose using the grayscale version
SOM_TRY
return estimateOneOrMoreStatesFromGrayscaleFrame(frameBuffer, inputCameraPosesBuffer, inputQRCodeIdentifiersBuffer, inputQRCodeDimensionsBuffer);
SOM_CATCH("Error calculating pose from image\n")
}

/*
This function takes a grayscale frame of the appropriate size, scans for any QR codes with an embedded sizes (recognized decimal formats: ft, in, cm, mm, m), and stores the poses of the camera (OpenCV format) relative to the different coordinate systems of the QR tags in the provided buffers.
@param inputGrayscaleFrame: The frame to process (should be same size as calibration)
@param inputCameraPosesBuffer: The buffer to place the 4x4 camera pose matrices in
@param inputQRCodeIdentifiersBuffer: A buffer to place the text left from each QR code after the dimension information has been removed
@param inputQRCodeDimensionsBuffer: A buffer to place size of each QR code in meters 
@return: true if it was able to scan a QR code and estimate its pose relative to it and false otherwise

@exceptions: This function can throw exceptions
*/
bool QRCodeStateEstimator::estimateOneOrMoreStatesFromGrayscaleFrame(const cv::Mat &inputGrayscaleFrame, std::vector<cv::Mat> &inputCameraPosesBuffer, std::vector<std::string> &inputQRCodeIdentifiersBuffer, std::vector<double> &inputQRCodeDimensionsBuffer)
{
if(inputGrayscaleFrame.channels() != 1)
{
throw SOMException(std::string("Given frame is not grayscale\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Wrap the image data so that it can be used by zbar
int frameWidth = inputGrayscaleFrame.cols;
int frameHeight = inputGrayscaleFrame.rows;
uchar *rawData = (uchar *)(inputGrayscaleFrame.data);


// Wrap image data
zbar::Image zbarFrame(frameWidth, frameHeight, "Y800", rawData, frameWidth * frameHeight);
SOMScopeGuard zbarFrameGuard([&](){zbarFrame.set_data(NULL, 0);});

//Scan for QR codes
if(zbarScanner.scan(zbarFrame) == -1)
{
printf("TestScanner error\n");
throw SOMException(std::string("QR code scanner returned with error\n"), ZBAR_ERROR, __FILE__, __LINE__);
}

//Clear the buffers to store everything in
inputCameraPosesBuffer.clear();
inputQRCodeIdentifiersBuffer.clear();
inputQRCodeDimensionsBuffer.clear();

for (zbar::Image::SymbolIterator symbol = zbarFrame.symbol_begin();  symbol != zbarFrame.symbol_end();  ++symbol) 
{
if(symbol->get_type() != zbar::ZBAR_QRCODE || symbol->get_location_size() != 4)
{
continue; //Skip if it isn't a QR code or its outline is described by more than 4 vertices
} 

double QRCodeDimensionInMeters;
std::string QRCodeIdentifierString;
if(extractQRCodeDimensionFromString(symbol->get_data(), QRCodeDimensionInMeters, QRCodeIdentifierString) != true)
{
continue; //Couldn't read dimension
}

//Convert zbar points to opencv points
std::vector<cv::Point2d> openCVPoints;
for(int i=0; i < symbol->get_location_size(); i++)
{
openCVPoints.push_back(cv::Point2d(symbol->get_location_x(i), symbol->get_location_y(i)));
}


//The center of the coordinate system associated with the QR code is in the center of the rectangle

double buf = QRCodeDimensionInMeters/2.0;
std::vector<cv::Point3d> objectVerticesInObjectCoordinates = 
{
cv::Point3d(-buf, -buf, 0), 
cv::Point3d(buf, -buf, 0),
cv::Point3d(buf, buf, 0),
cv::Point3d(-buf, buf, 0)
};

//Make buffers to get 3x1 rotation vector and 3x1 translation vector
cv::Mat_<double> rotationVector(3,1);
cv::Mat_<double> translationVector(3,1);


//Use solvePnP to get the rotation and translation vector of the QR code relative to the camera
cv::solvePnP(objectVerticesInObjectCoordinates, openCVPoints, cameraMatrix, distortionParameters, rotationVector, translationVector);


cv::Mat_<double> rotationMatrix, viewMatrix(4, 4);

//Get 3x3 rotation matrix
cv::Rodrigues(rotationVector, rotationMatrix);

//Zero out view matrix
viewMatrix = cv::Mat::zeros(4, 4, CV_64F); 

//Get opencv transfer matrix (camera -> object)
for(int row = 0; row < 3; row++)
{
for(int col = 0; col < 3; col ++)
{
viewMatrix.at<double>(row, col) = rotationMatrix.at<double>(row, col);
}
viewMatrix.at<double>(row, 3) = translationVector.at<double>(row, 0);
}
viewMatrix.at<double>(3,3) = 1.0;


//Invert matrix to get position and orientation of camera relative to tag, storing it in a buffer
cv::Mat cameraPoseBuffer;
invert(viewMatrix, cameraPoseBuffer);

//Store in buffer vectors
inputCameraPosesBuffer.push_back(cameraPoseBuffer);
inputQRCodeIdentifiersBuffer.push_back(QRCodeIdentifierString);
inputQRCodeDimensionsBuffer.push_back(QRCodeDimensionInMeters);

} //End symbol for loop

if(showResultsInWindow)
{
//Draw base image
cv::Mat bufferFrame = inputGrayscaleFrame;

// Draw location of the symbols found
for (zbar::Image::SymbolIterator symbol = zbarFrame.symbol_begin();  symbol != zbarFrame.symbol_end();  ++symbol) 
{
if(symbol->get_type() != zbar::ZBAR_QRCODE || symbol->get_location_size() != 4)
{
continue; //Skip if it isn't a QR code or its outline is described by more than 4 vertices
} 

double QRCodeDimensionInMeters;
std::string QRCodeIdentifierString;
if(extractQRCodeDimensionFromString(symbol->get_data(), QRCodeDimensionInMeters, QRCodeIdentifierString) != true)
{
continue; //Couldn't read dimension
}

line(bufferFrame, cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Scalar(0, 0, 0), 2, 8, 0); //Red 0->1
line(bufferFrame, cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Scalar(85, 85, 85), 2, 8, 0); //Green 1 -> 2
line(bufferFrame, cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Scalar(150, 150, 150), 2, 8, 0); //Blue 2 -> 3
line(bufferFrame, cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Scalar(255, 255, 255), 2, 8, 0); //Yellow  3 -> 0
}

imshow(QRCodeStateEstimatorWindowTitle, bufferFrame);
cv::waitKey(30);
}


//Make sure it updates every frame, even if it found the qr code in the last frame
zbarScanner.recycle_image(zbarFrame);

if(inputCameraPosesBuffer.size() > 0)
{
return true;
}

 
//Didn't find/process any suitable QR codes, so return false
return false;
}




/*
This function takes a string in the format "dimensionIdentifier" (for example, "12.0in-FKDJL") and stores the dimension from the string in meters and the remainder.  In the example case, it would store 0.3048 and "FKDJL".  It supports the following extensions and is case insensitive: "m-", "cm-", "mm-", "ft-", "in-".
@param inputQRCodeString: The original string
@param inputDimensionBuffer: The buffer to store the extracted dimension (in meters) in
@param inputIdentifierBuffer: The remainder of the string after the dimension has been extracted
@return: true if it was possible to extract the dimension and false otherwise
*/
bool extractQRCodeDimensionFromString(const std::string &inputQRCodeString, double &inputDimensionBuffer, std::string &inputIdentifierBuffer)
{
//Check the string for extensions and use the closest one
std::string buffer = inputQRCodeString;

//Convert local copy to lower case
std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);

int minimumUnitIdentifierStartingIndex = buffer.size(); //The first index of the string that matches one of the unit IDs
std::string minimumUnitIDType; //The unit ID that generated the minimum


//Find which identifier (if any) to use
for(auto iter = unitIdentifierToMetricMeterConversionFactor.begin(); iter != unitIdentifierToMetricMeterConversionFactor.end(); iter++)
{
//Find where the current unit identifier is in the string
int currentIndex = buffer.find(iter->first);
if(currentIndex == std::string::npos)
{
continue; //Unit identifier not found, so skip
}

//Update minimum if better
if(currentIndex < minimumUnitIdentifierStartingIndex)
{
minimumUnitIdentifierStartingIndex = currentIndex;
minimumUnitIDType = iter->first;
}

}

//Check if we found any of the unit identifiers
if(minimumUnitIDType.size() == 0)
{
return false; //We did not
}

//Grab all of the string up to the unit identifier and convert it to a double 
std::string numberPortion = buffer.substr(0, minimumUnitIdentifierStartingIndex);
double dimensionInOriginalUnits;

try
{
dimensionInOriginalUnits = std::stod(numberPortion);
}
catch(const std::exception &inputException)
{
return false; //Invalid numeric input
}

//Store the equivalent value in meters
inputDimensionBuffer = dimensionInOriginalUnits * unitIdentifierToMetricMeterConversionFactor.at(minimumUnitIDType);

//Store the remainder of the string
inputIdentifierBuffer = inputQRCodeString.substr(minimumUnitIdentifierStartingIndex + minimumUnitIDType.size(), std::string::npos); 

return true;
}

