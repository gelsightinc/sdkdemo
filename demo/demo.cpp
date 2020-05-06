// Demo.cpp
//
// This file contains functions that demonstrate the basic functionality of GelSightSDK
// 
// Demo functions:
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
	string scanfile = setpath + "R513-500/scan.yaml";

	cout << "Running photometric stereo algorithm on " << scanfile << endl;

	// Load a scan from the scan file
	
	auto scan = gs::LoadScanFromYAML(scanfile, gs::DefaultAnalysisManager());

	// Load images from scan
	auto images = gs::util::LoadImages(scan->imagePaths());

	cout << "Loaded " << images.size() << " images" << endl;
	if (images.size() == 0)
		return 1;

	// Do surface normal reconstruction
	auto nrm = pstereo->nonlinearNormalMap(images, pstereo->roi());

	// Integrate normals into heightmap
	cout << "Integrating surface normals..." << endl;
	auto poisson = gs::CreateIntegrator(gs::Version());
	auto heightmap = poisson->integrateNormalMap(nrm, pstereo->resolution());

	// Save surface as TMD
	string out1 = setpath + "R513-500/output.tmd";
	cout << "Saving heightmap: " << out1 << endl;
	gs::util::WriteTMD(out1, heightmap, pstereo->resolution(), 0.0, 0.0);


	// Save normal map
	string out2 = setpath + "R513-500/output_nrm.png";
	cout << "Saving normal map: " << out2 << endl;
	gs::util::WriteNormalMap(out2, nrm, 16);

	return 0;
}


/*
 * This function shows how to load a saved calibration and compute 3D for a scan
 */
int runsavedcalib()
{

	auto modelfile = setpath + "model-dome.yaml";
	cout << "Loading saved calibration data: " << modelfile << endl;
	// Load PhotometricStereo algorithm from settings file
	
	try {
		auto pstereo = gs::LoadPhotometricStereo(modelfile);

        // Run pstereo algorithm on a scan
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

    // We have 4 scans of the calibration target at different positions
    // we will add them all to the list of calibration targets
    for (int i = 0; i < 4; ++i) {
        auto scanfolder = fs::canonicalize(setpath + "BGA-00" + std::to_string(i+1));
        auto target     = gs::BgaTarget::create(scanfolder);
        targets.push_back(target);
    }

    // It's recommended, but not required, to add a scan of a flat plate to the 
    // list of calibration targets
	auto flatp = fs::canonicalize(setpath + "Flat-001");
    auto flat = gs::FlatTarget::create(flatp);
    targets.push_back(flat);

	auto start = std::chrono::system_clock::now();
	
	cout << "Running calibration algorithm..." << endl;

	auto pstereo = gs::CalibratePhotometricStereo(targets, gs::Version());

	std::chrono::duration<double> readtime = std::chrono::system_clock::now() - start;
    
	cout << "calibration took " << readtime.count() << " seconds" << endl;

    // Save the calibration data to a file
    // Only supported file format is YAML
    pstereo->save(setpath + "demo-calibration.yaml", gs::Format::YAML);

	return 0;
}


/*
 * This function is meant to simulate a scan that has been saved to disk
 */
vector<string> doscan(const string& foldername)
{
	string calibdir = setpath + "calib";
	
	vector<string> paths;
	for (int i = 0; i < 6; ++i) {
		paths.push_back(calibdir + "/image0" + std::to_string(i+1) + ".png");
	}
	return paths;
}

/*
 * This function shows how to calibrate the system from folders of BGA scans
 */
void runCalibrationFromImagePaths()
{
//	const double pitchmm  = 0.4;
//	const double radiusmm = 0.15625;
	const double resolution = 0.007812500000000002;

	// List of BGA Targets
	std::vector<std::shared_ptr<gs::CalibrationTarget>> targets;
	for (int i = 0; i < 4; ++i) {
		// Path to scan folder
		auto scanp = string("../../../testdata/BGA-00") + std::to_string(i+1);

		// Now create BGAs from scans
		auto target = gs::BgaTarget::create( scanp );

		targets.push_back(target);
	}

	cout << "Run calibration algorithm..." << endl;
    auto pstereo = gs::CalibratePhotometricStereo(targets, resolution, gs::Version());

	// Save calibration file as model.yaml
	pstereo->save("../../../testdata/testmodel.yaml", gs::Format::YAML);
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
	} catch(std::exception& ex) {
		std::cerr << "first try catch " << ex.what() << endl;
	}
	

	try {

        // Run calibration algorithm on BGA scans
        runcalibration();

        // Run photometric stereo algorithm to generate 3D
        runsavedcalib();

	} catch (gs::Exception& e) {
		std::cerr << "second try catch " << e.what() << " " <<  std::endl;
		
	}
	
	return 0;
}

