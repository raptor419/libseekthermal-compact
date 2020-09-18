// based on Seek Thermal Viewer/Streamer by http://github.com/fnoop/maverick

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "seek.h"
#include "SeekCam.h"
#include <iostream>
#include <string>
#include <signal.h>
#include <math.h>
#include <memory>
#include "args.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

using namespace LibSeek;

// Setup sig handling
static volatile sig_atomic_t sigflag = 0;
void handle_sig(int sig) {
    (void)sig;
    sigflag = 1;
}

/*
 * custom write pgm function since opencv does not support comments
 */
void writepgm(
    const cv::Mat & seekframe, 
    const std::string & prefix, const int framenumber, 
    const std::string & comment, const bool ascii
) {
    std::ostringstream pgmfilename;
    pgmfilename << prefix;
    pgmfilename << std::internal << std::setfill('0') << std::setw(6);
    pgmfilename << framenumber;
    pgmfilename << ".pgm";
    std::ofstream pgmfile;
    pgmfile.open(pgmfilename.str());
    if (ascii) {
        pgmfile << "P2\n";
    } else {
        pgmfile << "P5\n";
    }
    pgmfile << "# " << comment << "\n";
    pgmfile << seekframe.cols << " " << seekframe.rows << "\n";
    pgmfile << std::to_string(std::numeric_limits<uint16_t>::max()) << "\n";
    if (ascii) {
        for (int y = 0; y < seekframe.rows; y++) {
            for (int x = 0; x < seekframe.cols; x++) {
                pgmfile << std::to_string(seekframe.at<uint16_t>(y,x)) << " ";
            }
        }
    } else {
        // TODO: check endianess
        pgmfile.write(reinterpret_cast<char*>(seekframe.data), seekframe.total()*seekframe.elemSize());
    }
    pgmfile.close();
}

int main(int argc, char** argv)
{
    // Setup arguments for parser
    args::ArgumentParser parser("Seek Thermal Viewer");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> _ffc(parser, "FFC", "Additional Flat Field calibration - provide ffc file", {'F', "FFC"});
    args::ValueFlag<std::string> _camtype(parser, "camtype", "Seek Thermal Camera Model - seek or seekpro", {'t', "camtype"});
    args::ValueFlag<int> _interval(parser, "interval", "Interval of images to store on disk. Chose 1 for every image. Default: Every tenth frame.", {'i', "interval"});

    // Parse arguments
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    std::string camtype = "seek";
    if (_camtype)
        camtype = args::get(_camtype);
    int interval = 10;
    if (_interval) {
        interval = args::get(_interval);
        }

    // Register signals
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    // Setup seek camera
    LibSeek::SeekCam* seek;
    LibSeek::SeekThermalPro seekpro(args::get(_ffc));
    LibSeek::SeekThermal seekclassic(args::get(_ffc));
    if (camtype == "seekpro")
        seek = &seekpro;
    else
        seek = &seekclassic;

    if (!seek->open()) {
        std::cout << "Error accessing camera" << std::endl;
        return 1;
    }

    printf("#rmin rmax central devtempsns\n");

    // Mat containers for seek frames
    cv::Mat seekframe, outframe;
    int outframenumber = 0;
    auto time_start = std::chrono::high_resolution_clock::now();

    // Main loop to retrieve frames from camera and output
    // If signal for interrupt/termination was received, break out of main loop and exit
    while (!sigflag) {
        
        // Retrieve frame from seek and process
        if (!seek->read(seekframe)) {
            std::cout << "Failed to read frame from camera, exiting" << std::endl;
            return -1;
        }
        auto time_frame = std::chrono::high_resolution_clock::now();

        // get raw max/min/central values
        double min, max, central;
        cv::minMaxIdx(seekframe, &min, &max);
        cv::Point center(seekframe.cols/2.0, seekframe.rows/2.0);
        cv::Rect middle(center-cv::Point(1,1), cv::Size(3,3));
        cv::Scalar mean = cv::mean(seekframe(middle));
        central = mean(0);

        printf("%d %d %d %d\n", 
        int(min), int(max), int(central), seek->device_temp_sensor());
        
        //int framenumber = seek->frame_counter();
        if (outframenumber % interval == 0) {
            auto frametime = std::chrono::duration_cast<std::chrono::nanoseconds>(time_frame-time_start).count(); // nanoseconds since recording started (for variable framerate presentation)
            std::string comment =
                std::to_string(int(central))
                +" "+std::to_string(seek->device_temp_sensor())
                +" "+std::to_string(frametime);
            writepgm(
                seekframe, 
                "seekframe_", outframenumber, 
                comment,
                true
            );
        }
        outframenumber++;

        seekframe.convertTo(outframe, CV_8U, 1, 128-central);

        if (!outframe.empty()) {
            cv::imshow("SeekThermal", outframe);
        }
        char c = cv::waitKey(10);
        if (c == 'q') {
            break;
        }
    }

    if (sigflag) {
        std::cout << "Break signal detected, exiting" << std::endl;
    }
    
    return 0;
}
