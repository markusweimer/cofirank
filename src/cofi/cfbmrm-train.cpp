/* The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Authors      : Markus Weimer       (cofirank@weimo.de)
 *                Alexandros Karatzoglou (alexis@ci.tuwien.ac.at)
 *                Quoc V. Le          (Quoc.Le@nicta.com.au)
 *
 * Created      : 30/04/2007
 *
 * Last Updated :
 */

// Standard
#include <iostream>
#include <string>
#include <limits.h>



// Our Code
#include "core/types.hpp"
#include "io/io.hpp"
#include "cofi/cofibmrm.hpp"
#include "utils/configuration.hpp"
#include "cofi/problem.hpp"



/**
 * A simple hack to print to the log or screen whether or not assertions are
 * enabled.
 */
bool assertionsEnabled(void) {
    std::clog << "THIS IS A DEBUG RUN; ASSERTIONS ARE ENABLED" << std::endl;
    return true;
}


/**
 * The main method of cofirank. It does a number of things:
 * (1) Read the default config.
 * (2) Read the user submitted config
 * (3) Setup logging into a file "clog.txt" in the output folder
 * (4) Instanciate the Problem and COFIBMRM objects
 * (5) Train the system
 * (6) If configured, save the resulting matrices to the output folder
 */
int main(int argc, char **argv) {
    std::string outFolder = "./";
    try {
        if (argc < 2) {
            std::cout << "Please give the config file as the first argument" << std::endl;
            return -1;
        }


        // Read configuration
        Configuration& conf = Configuration::getInstance();
        std::string configFileName(argv[1]);
        conf.readFromFile(configFileName);


        // Make sure that clog is properly redirected
        outFolder = conf.getString("cofi.outfolder");
        std::ofstream running((outFolder + "COFI-RUNNING").c_str());
        running.close();
        std::clog << "All output including logs will go to " << outFolder << std::endl;
        std::ofstream clogOut((outFolder + "clog.txt").c_str());
        std::clog.rdbuf(clogOut.rdbuf());
        std::clog << "Starting up" << std::endl;
        std::clog << "Configuration file used: " << configFileName << std::endl;
        assert(assertionsEnabled());
        std::string codepath = conf.getString("cofi.codePath");

        // Train the system
        cofi::Problem p;
        cofi::COFIBMRM b(p);
        b.train();
        if (conf.getInt("cofi.storeModel") == 1) {
            cofi::io::storeMatrix(p.getU(), outFolder + "U.lsvm");
            cofi::io::storeMatrix(p.getM(), outFolder + "M.lsvm");
            //                cofi::io::storeMatrix(p.getA(), outFolder + cofi::Cofi::AFileName);
            //                cofi::io::storeMatrix(p.getS(), outFolder + cofi::Cofi::SFileName);
        }
        if (conf.getInt("cofi.storeF") == 1) {
            cofi::io::storeMatrix(prod(p.getU(), trans(p.getM())), outFolder + "F.lsvm");
        }


        // Store the configuration and log its useage statistics
        conf.writeUsageStatistics(std::clog);
        conf.writeToFile((outFolder + "effective-configuration.cfg"));


        // All done
        std::clog << "Done!" << std::endl;
        std::cout << "Done!" << std::endl;
        clogOut.close();
        std::ofstream done((outFolder + "COFI-DONE").c_str());
        done.close();
    } catch (cofi::CoFiException& e) {
        std::cerr << "A cofi exception occurred: " << e.describe() << std::endl;
        std::ofstream error((outFolder + "COFI-ERROR").c_str());
        error.close();
    }
    return 0;
}