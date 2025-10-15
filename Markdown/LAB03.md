# LAB 03

## Use separate thread to identify an object in a image stream

Extend the code from previous lab. Modify the code:

   1. In App class, create several private atomic variables for `main_thread <-> tracker_thread` communication: request to terminate tracker thread, notification of no-more-frames, result of the image tracking (x,y normalized coordinates). Where appropriate, use smart pointers, std::pair, std::tuple, etc.

   2. Create function, that will be executed in separate thread:

      ```C++
      void tracker_thread(cv::VideoCapture & capture)
      ```

      The function will contain the loop, that must: pick an image from already opened capture device (and verify success), call the function responsible for object identification, send the result to the main thread, and coordinate thread termination (if necessary).

   3. Main thread opens the camera (video file), creates the tracker thread, and pass reference to opened capture device to the tracker thread. Main thread (in the loop) prints the result in the console (or displays the image), tests key to end program, coordinates thread termination, and at the end joins tracker thread.

   Q: Does the main thread have to update console (display) in each iteration of the loop?

## Measure performance

Measure FPS of each loop. The loop in the tracker thread will be limited mainly by camera hardware (or video decoding speed, if you will decode stored video file). The main thread will be limited mainly by speed of console (or image render).
