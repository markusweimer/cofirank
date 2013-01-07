#include "configuration.hpp"


#include "configuration.hpp"
#include "configexception.hpp"
#include <iostream>
#include <fstream>

#include <vector>
#include <cstdlib>


void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ") {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


void trim(std::string& str) {
    using std::string;
    string::size_type pos = str.find_last_not_of(' ');
    if (pos != string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if (pos != string::npos) str.erase(0, pos);
    } else str.erase(str.begin(), str.end());
}


void Configuration::setDouble(std::string name, double value) {
    increaseWriteCount(name);
    doubles[name] = value;
}


double Configuration::getDouble(std::string name) throw (cofi::ConfigException) {
    increaseReadCount(name);
    if (doubles.count(name) > 0) return doubles[name];
    throw cofi::ConfigException(name);
}


void Configuration::setInt(std::string name, int value) {
    increaseWriteCount(name);
    ints[name] = value;
}


int Configuration::getInt(std::string name) throw (cofi::ConfigException) {
    increaseReadCount(name);
    if (ints.count(name) > 0) return ints[name];
    throw cofi::ConfigException(name);
}


bool Configuration::getIntAsBool(std::string name) throw (cofi::ConfigException) {
    return (this->getInt(name) == 1);
}


void Configuration::setString(std::string name, std::string value) {
    strings[name] = value;
    increaseWriteCount(name);
}


std::string Configuration::getString(std::string name) throw (cofi::ConfigException) {
    increaseReadCount(name);
    if (strings.count(name) > 0) return strings[name];
    throw cofi::ConfigException(name);
}


bool Configuration::isSet(std::string name) {
    if (ints.count(name) > 0) return true;
    if (doubles.count(name) > 0) return true;
    if (strings.count(name) > 0) return true;
    return false;
}


void Configuration::writeUsageStatistics(std::ostream& out) {
    using std::endl;
    using std::string;
    using std::map;

    out << "Usage statistics of configuration fields:" << endl;
    out << "type , name , readCount, writeCount" << endl;

    std::string s = " , ";
    for (map<string, double>::iterator iter = doubles.begin(); iter != doubles.end(); iter++) {
        const std::string& name = iter->first;
        out << "double , " << name << s << readCount[name] << s << writeCount[name] << endl;
    }
    for (map<string, int>::iterator iter = ints.begin(); iter != ints.end(); iter++) {
        const std::string& name = iter->first;
        out << "int    , " << name << s << readCount[name] << s << writeCount[name] << endl;
    }
    for (map<string, string>::iterator iter = strings.begin(); iter != strings.end(); iter++) {
        const std::string& name = iter->first;
        out << "string , " << name << s << readCount[name] << s << writeCount[name] << endl;
    }
}


void Configuration::increaseReadCount(const std::string& name) {
    if (readCount.count(name) == 0) {
        readCount[name] = 1;
    } else {
        readCount[name] = readCount[name] + 1;
    }
}


void Configuration::increaseWriteCount(const std::string& name) {
    if (writeCount.count(name) == 0) {
        writeCount[name] = 0;
    }
    writeCount[name] = writeCount[name] + 1;
}


void Configuration::readFromFile(std::string filename) {
    std::ifstream f(filename.c_str());
    if (not f.good()) {
        throw cofi::ConfigException("Unable to read configuration from " + filename);
    }
    while (!f.eof()) {
        std::string line;
        getline(f, line);
        trim(line);
        // Skip comments
        if (line[0] == '/' and line[1] == '/') continue;
        std::vector<std::string> tokens;
        Tokenize(line, tokens, " ");
        if (tokens.size() == 0) continue;
        std::string name = tokens[1];
        if (tokens[0] == "double") {
            double value = atof(tokens[2].c_str());
            setDouble(name, value);
        } else if (tokens[0] == "int") {
            int value = atoi(tokens[2].c_str());
            setInt(name, value);
        } else if (tokens[0] == "string") {
            std::string value = "";
            for (unsigned int i = 2; i < tokens.size(); ++i) value += tokens[i] + " ";
            trim(value);
            setString(name, value);
        } else {
            std::cerr << "Configuration: cannot read line " << line << " from file " << filename << std::endl;
        }
    }
    f.close();
}


void Configuration::write(std::ostream& out) {
    using std::string;
    using std::map;
    for (map<string, double>::iterator iter = doubles.begin(); iter != doubles.end(); iter++) {
        out << "double " << iter->first << " " << iter->second << std::endl;
    }
    for (map<string, int>::iterator iter = ints.begin(); iter != ints.end(); iter++) {
        out << "int " << iter->first << " " << iter->second << std::endl;
    }
    for (map<string, string>::iterator iter = strings.begin(); iter != strings.end(); iter++) {
        out << "string " << iter->first << " " << iter->second << std::endl;
    }
}


void Configuration::writeToFile(std::string fileName) {
    std::ofstream out(fileName.c_str());
    write(out);
    out.close();
}


// Singleton implementation
Configuration* Configuration::instance = NULL;


Configuration& Configuration::getInstance(void) {
    if (instance == NULL) {
        instance = new Configuration();

        // whether or not to use an offset
        instance->setInt("cofi.useMovieOffset", 0);
        instance->setInt("cofi.useUserOffset", 0);

        // whether or not top use the GraphKernel
        instance->setInt("cofi.useGraphKernel", 0);

        // whether or not U, M and F shall be stored at the end
        instance->setInt("cofi.storeModel", 0);
        instance->setInt("cofi.storeF", 0);

        // Lambdas
        instance->setDouble("cofi.userphase.lambda", 10.0);
        instance->setDouble("cofi.moviephase.lambda", 10.0);

        // Whether or not to use the sigma based regularizer
        instance->setInt("cofi.useSigmaRegularizer", 0);

        // whether or not we want to use adaptive regularization
        instance->setInt("cofi.useAdaptiveRegularization", 0);
        instance->setDouble("cofi.adaptiveRegularization.uExponent", 1.0);
        instance->setDouble("cofi.adaptiveRegularization.wExponent", 1.0);

        // Solver Config
        instance->setString("cofi.solver", "BMRM");

        // Dimension of U and M.
        instance->setInt("cofi.dimW", 10);

        // Stop Criteria
        instance->setInt("cofi.minIterations", 1);
        instance->setInt("cofi.maxIterations", 100);
        instance->setDouble("cofi.allowedDivergence", 0.1);
        instance->setDouble("cofi.minRelativeProgress", 0.01);

        // The loss to optimize for. NO DEFAULT VALUE
        instance->setString("cofi.loss", "REGRESSION");


        // The regularizer to use
        instance->setString("cofi.regularizer", "L2");


        // Evaluation schemes
        instance->setInt("cofi.eval.binary", 0);
        instance->setInt("cofi.eval.ndcg", 0);
        instance->setInt("cofi.eval.ndcg.k", 10);
        instance->setInt("cofi.eval.mse", 1);
        instance->setInt("cofi.eval.brmse", 0);
        instance->setInt("cofi.eval.ir", 0);



        // BMRM options
        instance->setDouble("bmrm.minRelativeOptimProgress", 0.02);
        instance->setDouble("bmrm.minRelativeProgress", 0.02);
        instance->setDouble("bmrm.minProgress", 0.01);
        instance->setDouble("bmrm.minOptimProgress", -1.0);
        instance->setInt("bmrm.maxNumberOfIterations", 70);

        instance->setString("bmrm.innerSolver", "prLOQO");

        // SGD Options
        instance->setDouble("sgd.minRelativeProgress", 0.01);
        instance->setInt("sgd.maxNumberOfIterations", 50);
        instance->setDouble("sgd.initialStepSize", 2e-4);

        // Configuration of the losses
        // NDCG
        instance->setInt("loss.ndcg.trainK", 10);
        instance->setDouble("loss.ndcg.c_exponent", -0.25);
        // Weighted SoftMargin Options
        instance->setDouble("loss.weightedSoftMargin.positiveWeight", 1.0);
        instance->setDouble("loss.weightedSoftMargin.negativeWeight", 1.0);

        // Iterator options
        instance->setString("cofi.iterator", "SPARSE");
        instance->setDouble("cofi.iterator.dense.defaultvalue", -1.0);

        // CodePath setting
        instance->setString("cofi.codePath", "default");


        // Special KDD'09 settings
        instance->setInt("cofi.kdd09.bmrmqueryphase", 0);
        instance->setInt("cofi.sequentialuserphase", 0);
    }
    return *instance;
}
