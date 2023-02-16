// demo.cpp
//
// This file contains functions that demonstrate the basic functionality of GelSightSDK
// 
// Demo functions:
//     
//     runCalibration         Calibrate the system from BGA and Flat scans
//     runPhotometricStereo   Load a saved calibration file and run 3D reconstruction from scan images
// 
// Author: Kimo Johnson
// Initial Revision: 2/5/2017
// Latest Revision: 2/16/2023
//
//

#include "gsanalysismanager.h"
#include "pstereo.h"
#include "imageio.h"
#include "integrator.h"
#include "gelsightsdk.h"
#include "flatfieldmodel.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <memory>


/*
 * This function shows how to load a saved calibration and compute 3D for a scan
 * 
 * @param calFile The path and name of the calibration file ending in .yaml
 *				  There should also be a .png file with the same name as the .yaml
 * @param scanPath The path to the scan data
 * 
 */
int runPhotometricStereo(std::string& calFile, std::string& scanPath)
{

	// Load PhotometricStereo algorithm from settings file
	
	gs::PhotometricStereoPtr pstereo;
	try {
		pstereo = gs::LoadPhotometricStereo(calFile);

	} catch (gs::Exception& e) {
		std::cout << "Exception: " << e.error() << std::endl;
	} 
	
	std::cout << "Loading image paths ..." << std::endl;
	std::vector<std::string> imagePaths;
	for (auto i = 1; i <= 6; i++)
	{
		auto imagePath = scanPath + "/image0" + std::to_string(i) + ".png";
		imagePaths.push_back(imagePath);
		std::cout << imagePath << std::endl;
	}

	auto scan = gs::CreateScan(imagePaths);
	scan->setResolution(pstereo->resolution(), gs::Unit::MM);

	std::cout << "Running photometric stereo algorithm on " << scanPath << std::endl;

	auto flatfield = gs::LoadFlatFieldModel(calFile);

	if (flatfield != nullptr)
		flatfield->adjust(scan, DEFAULT_TI);

	const auto ti = DEFAULT_TI;

	std::cout << "Generating the normals ..." << std::endl;
	gs::NormalMap nrm;
	try
	{
		nrm = pstereo->nonlinearNormalMap(scan->images(), pstereo->roi(), ti);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	auto nrmpath = scanPath + "/output_nrm.png";
	gs::util::WriteNormalMap(nrmpath, nrm, 16);
	std::cout << "saved normals to " << nrmpath << std::endl;

	std::cout << "Creating the height map ..." << std::endl;
	auto poisson = gs::CreateIntegrator(gs::Version());
	auto HeightMapImage = poisson->integrateNormalMap(nrm, pstereo->resolution(), ti);

	auto fname = scanPath + "/output_scan.tmd";
	gs::util::WriteTmd(fname, HeightMapImage, pstereo->resolution(), 0.0, 0.0);

	return 0;
}



/*
 * This function shows how to calibrate the system from a set of calibration scans
 * There are 4 BGA scans and a Flat scan
 * 
 * @param calibrationScansPath The toplevel path to the scans using for calibration
 */
int runCalibration(std::string& calibrationScansPath)
{

	// Create list of calibration targets
    gs::CalibrationTargets targets;

    // We have 4 scans of the calibration target at different positions
    // we will add them all to the list of calibration targets
    for (int i = 0; i < 4; ++i) {
        auto scanfolder = fs::canonicalize(calibrationScansPath + "BGA-00" + std::to_string(i+1));
        auto target     = gs::BgaTarget::create(scanfolder);
        targets.push_back(target);
    }

    // It's recommended, but not required, to add a scan of a flat plate to the 
    // list of calibration targets
	auto flatp = fs::canonicalize(calibrationScansPath + "Flat-001");
    auto flat = gs::FlatTarget::create(flatp);
	targets.push_back(flat);

	auto start = std::chrono::system_clock::now();
	
	std::cout << "Running calibration algorithm..." << std::endl;

	auto pstereo = gs::CalibratePhotometricStereo(targets, gs::Version());

	std::chrono::duration<double> readtime = std::chrono::system_clock::now() - start;
    
	std::cout << "calibration took " << readtime.count() << " seconds" << std::endl;

    // Save the calibration data to a file
    // Only supported file format is YAML
    pstereo->save(calibrationScansPath + "/demo-calibration.yaml");

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
		gsSdkInitializeEx();
	} catch(std::exception& ex) {
		std::cerr << "gsSdkInitializeEx try catch " << ex.what() << std::endl;
	}


	//
	// DO_HEIGHTMAP 
	// This is an example of how to create the 3D data from a scan
	// You need the scan and the calibration data used to capture the scan
	//
	auto DO_HEIGHTMAP(true);
	if (DO_HEIGHTMAP)
	{
		// set the path to the scan data

		std::string scanPath("../testdata/HandheldData/japanese-coin-001");

		// set the path to the calibration data
		// ********************** NOTE *****************
		// This is the yaml file, there is also a file name model-test1.png
		// that needs to be in the same directory
		// If you look at the example model-test1.yaml , you will see
		// flatfield: 
		//		modelfile: model- test1.png
		// *********************************************

		std::string calYamlFile("../testdata/HandheldData/model-test1.yaml");

		std::cout << "Input paths = " << scanPath << " " << calYamlFile << std::endl;

		try {

			// Run photometric stereo algorithm to generate 3D
			auto result = runPhotometricStereo(calYamlFile, scanPath);

		}
		catch (gs::Exception& e) {
			std::cerr << "runPhotometricStereo try catch " << e.what() << " " << std::endl;

		}
	}

	// DO_CALIBRATE
	// This is an example of how to run calibration
	//
	auto DO_CALIBRATE(true);
	if (DO_CALIBRATE)
	{
		std::string calibrationScansPath("../testdata/OEMData/");
		try {

			auto result = runCalibration(calibrationScansPath);

		}
		catch (gs::Exception& e) {
			std::cerr << "runCalibration try catch " << e.what() << " " << std::endl;

		}
	}
	
	return 0;
}

