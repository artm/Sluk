#pragma once

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/variate_generator.hpp>

/*
 * The following helper functions aren't thread-safe or friendly. But it is
 * possible to make a multithreaded random number generator by maintaining a
 * separate state (and separate seed) in each thread
 */
double normal_rnd(double mean = 0.0, double sigma = 1.0) {
    static boost::variate_generator< boost::mt19937, boost::normal_distribution<> >
        r(boost::mt19937(time(0)), boost::normal_distribution<>());
    return mean + sigma * r();
}

double u01_rnd(double min=0.0, double max=1.0) {
    static boost::variate_generator< boost::mt19937, boost::uniform_01<> >
        r(boost::mt19937(time(0)), boost::uniform_01<>());
    return min + r() * (max-min);
}

struct NormalRnd {
    double mean, sigma;
    NormalRnd(double m = 0.0, double s = 1.0) : mean(m), sigma(s) { }
    template<typename Index>
    double operator()(Index = 0, Index = 0) const
    { return normal_rnd(mean,sigma); }
};

