#include <windows.h>
#include <iostream>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

using namespace cv;
using namespace std;


char* UTF8ToANSI(const char* pszCode)
{
    BSTR    bstrWide;
    char* pszAnsi;
    int     nLength;

    // Get nLength of the Wide Char buffer
    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1,
        NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    // Change UTF-8 to Unicode (UTF-16)
    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    // Get nLength of the multi byte buffer
    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    // Change from unicode to mult byte
    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    return pszAnsi;
}
int getCarPlateROI(void)
{
    // Load an image
    Mat img = imread("car_plate.jpg");
    Mat image = imread("car_plate.jpg");
    if (img.empty()) {
        cout << "Could not open or find the image." << endl;
        return -1;
    }
    resize(image, image, Size(1000, 1000)); // 원하는 크기로 조절

    imshow("Original Car", image);
    waitKey(0);
    // Convert the image to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Apply GaussianBlur to reduce noise and help in contour detection
    GaussianBlur(gray, gray, Size(5, 5), 0);

    // Use Canny edge detector to find edges
    Mat edges;
    Canny(gray, edges, 50, 150);

    // Apply threshold to create a binary image
    Mat binaryImage;
    threshold(edges, binaryImage, 128, 255, THRESH_BINARY);

    // Find contours in the binary image
    vector<vector<Point>> contours;
    findContours(binaryImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Filter contours based on area (adjust the threshold as needed)
    vector<vector<Point>> validContours;
    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area > 1000 && area < 200000) {
            validContours.push_back(contour);
            break; // Break after the first valid contour
        }
    }

    // Draw the contours on the original image
    Mat contourImg = img.clone();
    drawContours(contourImg, validContours, -1, Scalar(0, 255, 0), 2);

    // Display the image with contours
    //imshow("Contours", contourImg);
    //waitKey(0);

    // Crop and save the detected plate (only the first one)
    if (!validContours.empty()) {
        Rect roi = boundingRect(validContours[0]);

        // Adjust the region to add some padding if needed
        roi.x -= 10;
        roi.y -= 10;
        roi.width += 20;
        roi.height += 20;

        // Crop the region of interest
        Mat plateROI = img(roi);
        GaussianBlur(plateROI, plateROI, Size(5, 5), 0);
        // Display and save the cropped plate
        imshow("Detected Plate", plateROI);
        imwrite("detected_plate.png", plateROI);
        waitKey(0);
        
    }
}
int ROItoText(void)
{

    const char* input_image = "detected_plate.png";
    const char* dataPath = "C:/Program Files/Tesseract-OCR/tessdata";

    // tesseract api 설정
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    if (api->Init(dataPath, "kor")) {
        return -1;
    }

    // 이미지 설정
    Pix* image = pixRead(input_image);
    api->SetImage(image);

    std::string utf_text = api->GetUTF8Text();
    std::string text = UTF8ToANSI(utf_text.c_str());
    std::cout << "Car Number: " << text << std::endl;
    api->End();
    delete api;

}

int main() {
    getCarPlateROI();
    ROItoText();
    return 0;
}