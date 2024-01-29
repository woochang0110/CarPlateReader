#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace cv;
using namespace std;

int main() {
    // Load an image
    Mat img = imread("detected_plate.png");

    if (img.empty()) {
        cout << "Could not open or find the image." << endl;
        return -1;
    }

    // Initialize Tesseract OCR
    tesseract::TessBaseAPI tess;
    tess.Init(NULL, "kor+eng", tesseract::OEM_DEFAULT);
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    // Convert the image to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Use Tesseract OCR on the grayscale image
    tess.SetImage(gray.data, gray.cols, gray.rows, 1, gray.cols);
    string recognizedText = tess.GetUTF8Text();

    // Print the recognized text
    cout << "Recognized Text: " << recognizedText << endl;

    // Display the image with OCR result
    imshow("Detected Plate with OCR", img);
    waitKey(0);

    return 0;
}