#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>


#include "dataStructures.h"
#include "structIO.hpp"

using namespace std;

// Compute time-to-collision (TTC) based on keypoint correspondences in successive images
void computeTTCCamera(std::vector<cv::KeyPoint> &kptsPrev, std::vector<cv::KeyPoint> &kptsCurr,
                      std::vector<cv::DMatch> kptMatches, double frameRate, double &TTC)
{
    // DMatch struct
    // --> trainIdx: index of matched keypoint in curr. frame
    // --> queryIdx: index of matched keypoint in prev. frame
    // compute distance ratios between all matched keypoints
    vector<double> distRatios; // stores the distance ratios for all keypoints between curr. and prev. frame
    std::cout << kptMatches.size() << "\n";
    for (auto it1 = kptMatches.begin(); it1 != kptMatches.end() - 1; ++it1)
    { // outer kpt. loop

        // get current keypoint and its matched partner in the prev. frame
        std::cout << "trainIdx: " << it1->trainIdx << ", queryIdx: " << it1->queryIdx << "\n";
        cv::KeyPoint kpOuterCurr = kptsCurr.at(it1->trainIdx);
        cv::KeyPoint kpOuterPrev = kptsPrev.at(it1->queryIdx);

        for (auto it2 = kptMatches.begin() + 1; it2 != kptMatches.end(); ++it2)
        { // inner kpt.-loop

            double minDist = 100.0; // min. required distance

            // get next keypoint and its matched partner in the prev. frame
            cv::KeyPoint kpInnerCurr = kptsCurr.at(it2->trainIdx);
            cv::KeyPoint kpInnerPrev = kptsPrev.at(it2->queryIdx);

            // compute distances and distance ratios
            double distCurr = cv::norm(kpOuterCurr.pt - kpInnerCurr.pt);
            double distPrev = cv::norm(kpOuterPrev.pt - kpInnerPrev.pt);

            if (distPrev > std::numeric_limits<double>::epsilon() && distCurr >= minDist)
            { // avoid division by zero

                double distRatio = distCurr / distPrev;
                distRatios.push_back(distRatio);
            }
        } // eof inner loop over all matched kpts
    }     // eof outer loop over all matched kpts

    // only continue if list of distance ratios is not empty
    if (distRatios.size() == 0)
    {
        TTC = NAN;
        return;
    }

    // compute camera-based TTC from distance ratios
//     double meanDistRatio = std::accumulate(distRatios.begin(), distRatios.end(), 0.0) / distRatios.size();

//     double dT = 1 / frameRate;
//     TTC = -dT / (1 - meanDistRatio);

    // STUDENT TASK (replacement for meanDistRatio)
    
    // Sort the distances
    std::sort(distRatios.begin(), distRatios.end());
  
    // Obtain the middle index of the vector
    const size_t index = distRatios.size() / 2;
  
    // Get the median
    double medianDistRatio = distRatios[index];
    std::cout << index << "\n";
  
    // If the size of the vector is even, access the
    // vector at the index previous to it and average
    if (distRatios.size() % 2 == 0) {
        medianDistRatio = (medianDistRatio + distRatios[index - 1]) / 2.0;
    }
    
    // Now calculate TTC
    const double dT = 1 / frameRate;
    TTC = -dT / (1 - medianDistRatio);  
}

int main()
{
    vector<cv::KeyPoint> kptsSource, kptsRef;
    readKeypoints("../dat/C23A5_KptsSource_AKAZE.dat", kptsSource); // readKeypoints("./dat/C23A5_KptsSource_SHI-BRISK.dat"
    readKeypoints("../dat/C23A5_KptsRef_AKAZE.dat", kptsRef); // readKeypoints("./dat/C23A5_KptsRef_SHI-BRISK.dat"

    vector<cv::DMatch> matches;
    readKptMatches("../dat/C23A5_KptMatches_AKAZE.dat", matches); // readKptMatches("./dat/C23A5_KptMatches_SHI-BRISK.dat", matches);
    double ttc; 
    computeTTCCamera(kptsSource, kptsRef, matches, 10.0, ttc);
    cout << "ttc = " << ttc << "s" << endl;
}