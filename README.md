# Object detection using Local Feature
Implement an object detection application in OpenCV C/C++ using SIFT feature
Input: two images
+ Template of a known object
+ A scence image consisting of this object.
Determine the position of the object in the template image within the scene image following the steps:
+ Load two images
+ Detect keypoints in both images and compute descriptors
+ Find the closest matches between descriptors from the first image to the second using use BruteForceMatcher or kNNMacher
+ Find the homography transformation between two sets of points
