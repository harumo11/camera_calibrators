#include <cmath>
#include <iostream>
#include <matplot/freestanding/plot.h>
#include <matplot/matplot.h>
#include <matplot/util/common.h>
#include <random>

int main(void) {
    std::vector<double> x = matplot::randn(10000, 0, 1);
    auto h = matplot::hist(x);
    std::cout << "hist with " << h->num_bins() << " bins" << std::endl;
    matplot::show();
    return 0;
}
