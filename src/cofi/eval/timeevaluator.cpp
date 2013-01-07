#include "timeevaluator.hpp"

cofi::TimeEvaluator::TimeEvaluator(void) {
    timer.Start();
    iteration = 0;
}

std::vector<std::string> cofi::TimeEvaluator::names() {
    std::vector<std::string> result;
    result.push_back("iteration");
    result.push_back("time");
    return result;
}

void cofi::TimeEvaluator::eval(cofi::Problem& p, std::map<std::string, double>& results) {
    timer.Stop();
    const double time = timer.Total();
    ++iteration;
    timer.Start();
    results["iteration"] = static_cast<double> (iteration);
    results["time"] = time;
}

