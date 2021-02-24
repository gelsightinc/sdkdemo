// fingerdemo.cpp
//
// This file contains functions that demonstrate the basic functionality of GelSightSDK
// 
// demo functions:
//     
//     runcalibration         Calibrate the system from one or more BGA scans
//     runsavedcalib          Load a saved calibration file and run the 3D algorithms
// 
// Kimo Johnson  
// Last Revision: 2/5/2017
//
//

#include "gelsightsdk.h"
#include "gsanalysisroutine.h"
#include "calibration.h"
#include "geometry.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <memory>
#include <opencv2/opencv.hpp>

using std::string;
using std::cout;
using std::endl;
using std::vector;


// Path to testdata
string setpath("../testdata/");

/*
 * Run pstereo algorithm on sine wave sample data
 */
int runpstereo(gs::PhotometricStereo *pstereo)
{
  string scanfile = setpath + "FingerData/coin/scan.yaml";

  cout << "Running photometric stereo algorithm on " << scanfile << endl;

  // Load a scan from the scan file
	
  auto scan = gs::LoadScanFromYAML(scanfile, gs::DefaultAnalysisManager());

  // Load images from scan
  auto images = gs::util::LoadImages(scan->imagePaths());

  cout << "Loaded " << images.size() << " images" << endl;
  if (images.size() == 0)
    return 1;

  // Important to only compute 3D data with a specified crop region
  gs::RectI croproi(300,500,1000,1250);
  
  // Do surface normal reconstruction
  auto nrm = pstereo->linearNormalMap(images, croproi);

  // Integrate normals into heightmap
  cout << "Integrating surface normals..." << endl;
  auto poisson = gs::CreateIntegrator(gs::Version());
  auto heightmap = poisson->integrateNormalMap(nrm, pstereo->resolution());

  // Save surface as TMD
  string out1 = setpath + "FingerData/coin/output.tmd";
  cout << "Saving heightmap: " << out1 << endl;
  gs::util::WriteTMD(out1, heightmap, pstereo->resolution(), 0.0, 0.0);

  // Save normal map
  string out2 = setpath + "FingerData/coin/output_nrm.png";
  cout << "Saving normal map: " << out2 << endl;
  gs::util::WriteNormalMap(out2, nrm, 16);

  return 0;
}


/*
 * This function shows how to load a saved calibration and compute 3D for a scan
 */
int runsavedcalib()
{
  auto modelfile = setpath + "FingerData/finger-model.yaml";
  cout << "Loading saved calibration data: " << modelfile << endl;

  // Load PhotometricStereo algorithm from settings file
  // Run pstereo algorithm on scan
  try {
    auto pstereo = gs::LoadPhotometricStereo(modelfile);
    runpstereo(pstereo.get());
  } catch (gs::Exception& e) {
    cout << "Exception: " << e.error() << endl;
  }

  return 0;
}


/*
 * This function shows how to calibrate the system from one or more calibration scans
 */
int runcalibration()
{

    // Create list of calibration targets
    std::vector<std::shared_ptr<gs::CalibrationTarget>> targets;

    // We have 3 scans of the calibration target at different positions
    // we will add them all to the list of calibration targets
    for (int i = 0; i < 3; ++i) {
        auto scanfolder = fs::canonicalize(setpath + "FingerData/scan00" + std::to_string(i+1));
        auto target     = gs::BgaTarget::create(scanfolder);
        targets.push_back(target);
    }

    auto start = std::chrono::system_clock::now();
	
    cout << "Running calibration algorithm..." << endl;

    auto pstereo = gs::CalibratePhotometricStereo(targets, gs::Version());

    std::chrono::duration<double> readtime = std::chrono::system_clock::now() - start;
    
    cout << "calibration took " << readtime.count() << " seconds" << endl;

    // Save the calibration data to a file
    // Only supported file format is YAML
    pstereo->save(setpath + "FingerData/fingerdemo-calibration.yaml", gs::Format::YAML);

    return 0;
}

/*
 * This function shows how to calibrate the system from folders of BGA scans
 */
void runCalibrationFromImagePaths()
{
  const double resolution = 0.0295297735479;

   // List of BGA Targets
   std::vector<std::shared_ptr<gs::CalibrationTarget>> targets;
   for (int i = 0; i < 3; ++i) {

     auto scanp = fs::canonicalize(setpath + "FingerData/scan00" + std::to_string(i+1));
	
     // Now create BGAs from scans
     auto target = gs::BgaTarget::create( scanp );

     targets.push_back(target);
   }

    cout << "Run calibration algorithm..." << endl;
    auto pstereo = gs::CalibratePhotometricStereo(targets, resolution, gs::Version());

    // Save calibration file as model.yaml
    pstereo->save(setpath+"FingerData/testmodel.yaml", gs::Format::YAML);
}

/*
 * Run pstereo algorithm on sine wave sample data
 */
int runopencvex()
{
  string imagefile = setpath + "FingerData/coin/image01.png";
  string outfile = setpath + "FingerData/coin/gsimage.png";
  string memcpyoutfile = setpath + "FingerData/coin/gsimagememcpy.png";

  cout << "Running with data from" << imagefile << endl;

  // Load images from scan into opencv
  cv::Mat inimage, matimage;
  inimage = cv::imread(imagefile);

  if (inimage.empty()) {                                   
    cout << "could not open " << imagefile << endl;
    return 1;
  }
  
  // First example, using memcpy to copy a cv::Mat BGR to a gelsight BGR image
  // Allocate the gelsight image
  const auto bgr8 = gs::Bgr8(0,0,0);
  gs::ImageBgr8 gscopy(gs::SizeI(inimage.cols, inimage.rows), bgr8);
  // copy the image and write it out 
  memcpy(gscopy.ptr(0), &inimage.data[0], inimage.cols*inimage.rows*3);
  gs::util::WritePng(memcpyoutfile, gscopy);

  // Second example convert cv::Mat to gray image and write it out to a png file
  cv::cvtColor(inimage, matimage, cv::COLOR_BGR2GRAY); 

  // Third example, brute force copy pixel by pixel
  // Split the cv::Mat image into BGR channels
  std::vector<cv::Mat> channels;
  cv::split(inimage, channels);
  cv::Mat imageB = channels[0];
  cv::Mat imageG = channels[1];
  cv::Mat imageR = channels[2];

  //cv::imshow("Input image", matimage);
  //cv::waitKey(0);

  // Convert to gs::image
  auto xdim = matimage.cols;
  auto ydim = matimage.rows;

  // gsimage is a floating point image just for example
  // bgrim is the converted cv::Mat BGR image
  // this is the data we need to create the surface 
  gs::ImageF gsimage(gs::SizeI(xdim,ydim),0.0);
  gs::ImageBgr8 bgrim(gs::SizeI(xdim,ydim), bgr8);

  for (int y = 0; y < ydim; y++)
  {
    uchar *mimptr = inimage.ptr<uchar>(y);

    for (int x = 0; x < xdim; x++)
    {
      float pix = matimage.at<uchar>(y,x) / 255.;
      gsimage.setpel(y, x, pix);

      gs::Bgr8 bgrpix = gs::Bgr8(imageB.at<uchar>(y,x), imageG.at<uchar>(y,x), imageR.at<uchar>(y,x));
      bgrim.setpel(y, x, bgrpix);
    }
  }

  cout << "Write output png file" << outfile << endl;
  gs::util::WritePng(outfile, gsimage);
  
  // run the calibration
   auto modelfile = setpath + "FingerData/finger-model.yaml";
   cout << "Loading saved calibration data: " << modelfile << endl;

  // Load PhotometricStereo algorithm from settings file
  // Run pstereo algorithm on scan
  auto pstereo = gs::LoadPhotometricStereo(modelfile);

  gs::RectI croproi(300,500,1000,1250);

  // Do surface normal reconstruction
  // This is where the converted cv::Mat BGR image is used
  // We copied it into bgrim but we could have also used gscopy
  auto nrm = pstereo->linearNormalMap(bgrim, croproi);

  // Integrate normals into heightmap
  cout << "Integrating surface normals..." << endl;
  auto poisson = gs::CreateIntegrator(gs::Version());
  auto heightmap = poisson->integrateNormalMap(nrm, pstereo->resolution());

  // Save surface as TMD
  string out1 = setpath + "FingerData/coin/output.tmd";
  cout << "Saving heightmap: " << out1 << endl;
  gs::util::WriteTMD(out1, heightmap, pstereo->resolution(), 0.0, 0.0);

  // Save normal map
  string out2 = setpath + "FingerData/coin/output_nrm.png";
  cout << "Saving normal map: " << out2 << endl;
  gs::util::WriteNormalMap(out2, nrm, 16);

  return 0;
}


//
// 
//
int main(int argc, char *argv[])
{
	
	//
	// IMPORTANT: Must call gsSdkInitialize() before using the SDK
	//
	try {
		gsSdkInitialize();
		gs::Version();
	} catch(std::exception& ex) {
		std::cerr << "first try catch " << ex.what() << endl;
	}
/*
	try {

        // Run photometric stereo algorithm to generate 3D
        runsavedcalib();

	} catch (gs::Exception& e) {
		std::cerr << "second try catch " << e.what() << " " <<  std::endl;
		
	}
*/
  auto ret = runopencvex();


	return 0;
}

