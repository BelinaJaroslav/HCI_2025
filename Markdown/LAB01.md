# LAB 01

## Task 1: Identify the object using luminance

Explore the source code for finding the center of the lightbulb. Process the image manually and perform per-pixel operation in C++ code as shown in the example (two nested loops iterating over X, Y dimensions) to identify the object.

   1. Convert the image from RGB to YCbCr. Use Y channel for further operations.

      ```C++
      // JPEG 8bit conversion:

      Y  =       + 0.299    * R + 0.587    * G + 0.114    * B
      Cb = 128   - 0.168736 * R - 0.331264 * G + 0.5      * B
      Cr = 128   + 0.5      * R - 0.418688 * G - 0.081312 * B
      ```

   2. Use thresholding to separate object. Try different thresholds.

   3. Find centroid of white pixels. Show the result in normalized coordinates.

   4. Refactor the code: create the function _find_object_luma()_, where return value is normalized coordinate of the center of the object.

      ```C++
      cv::Point2f find_object_luma(cv::Mat & frame) {...}
      ```

## Task 2: Find the red cup in static image

   1. Use similar approach as in Task 1 (colorspace conversion, thresholding, centroid) to find the object, but search for RED color in HSV color model, and use existing functions of OpenCV. Create similar funtion:

      ```C++
      cv::Point2f find_object_chroma(cv::Mat & frame) {...}
      ```

   2. Use color information to separate the object from background.

      ```C++
      // Convert image to HSV using OpenCV.
      cv::cvtColor(img_RGB, img_HSV, cv::COLOR_BGR2HSV);

      // Set threshold range (example values)
      cv::Scalar lower_threshold = cv::Scalar(10,0,0);
      cv::Scalar upper_threshold = cv::Scalar(20,255,255);
      ```

      ⚠ Example range in HSV is not correct - it only has valid range of Hues, but all Saturations and all Values are accepted. Why is it not very smart? Use 'HSV-MAP.png' to find and set a proper range. If necessary, create separate project for _hsv-finder - helper_ - the interactive application, that allow you to choose proper values. Note the values and use them in the original project.

      ```C++
      // Compute threshold
      cv::InRangeS(img_HSV, lower_threshold, upper_threshold, img_dest);

      // Find all non-zero (ie. white) pixels in thresholded image
      std::vector<cv::Point> whitePixels;
      cv::findNonZero(scene_threshold, whitePixels);

      // Count white pixels
      int whiteCnt = whitePixels.size();

      // Count SUM of X_coords, Y_coords of white pixels
      // You need at least C++17 for std::reduce()
      cv::Point2f whiteAccum = std::reduce(whitePixels.begin(), whitePixels.end());

      // or faster = parallel version, with automatic multi-threading
      cv::Point2f whiteAccum = std::reduce(std::execution::par_unseq, whitePixels.begin(), whitePixels.end());

      // Divide by whiteCnt to get average, ie. centroid (only if whiteCnt != 0 !!!)
      cv::Point2f centroid_absolute = whiteAccum / whiteCnt;
      // Compute NORMALIZED coordinates
      cv::Point2f centroid_normalized = { centroid_absolute.x / scene.cols, centroid_absolute.y / scene.rows }; 
      ```

   3. (optional) Use morphological operations (dilatation, erosion, ...) to remove noise and improve the accuracy of the result.

## Task 3: Find the red object in sequence of images - camera stream or video

   1. Open capture device (camera or file) stream in init() function, store capture device variable as protected member variable in App class. If camera is not found, open the video file.

   2. Use similar approach as in Task 2 to find a moving object.

   3. Modify App::run(), so that there is only loop that grabs the image from a capture device until it is empty, calls find_object_***() and prints result, and test ESC key.

   4. Use sample code in time_measure.c to find the fastest approach. Try to spend at most **10 milliseconds** per frame.

   5. (optional) resize the image, if speed is too low (less pixels to analyze)

      ```C++
      cv::Resize(source_frame, frame, cv::INTER_NEAREST); //or cv::INTER_LINEAR etc.
      ```

## Task 4: Find the center of your face :-)

   1. Use code from Task 3 and sample code from _face detect_ directory.
