#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main() {
    // Load an image
    Mat img = imread("car_plate.jpg");

    if (img.empty()) {
        cout << "Could not open or find the image." << endl;
        return -1;
    }

    // Convert the image to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Apply GaussianBlur to reduce noise and help in contour detection
    GaussianBlur(gray, gray, Size(5, 5), 0);

    // Use Canny edge detector to find edges
    Mat edges;
    Canny(gray, edges, 50, 150);

    // Find contours in the edges
    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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

        // Display and save the cropped plate
        //imshow("Detected Plate", plateROI);
        imwrite("detected_plate.png", plateROI);

        //waitKey(0);
    }

    return 0;
}
