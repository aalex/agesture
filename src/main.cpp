#include "nearestneighborfinder.h"
#include <iostream>
#include <glib.h>

GRand *RANDOMIZER = NULL;
static const unsigned int DIMENSIONS = 2;
static const unsigned int LENGTH = 3;

std::vector<std::vector<double> > makeRandom(unsigned int dimensions, unsigned int length)
{
    std::vector<std::vector<double> > result;
    for (unsigned int i = 0; i < length; i++)
    {
        std::vector<double> vec;
        for (unsigned int j = 0; j < dimensions; j++)
        {
            vec.push_back(g_rand_double(RANDOMIZER));
        }
        result.push_back(vec);
    }
    std::cout << __FUNCTION__ << " ok\n";
    return result;
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    RANDOMIZER = g_rand_new();
    agesture::NearestNeighborFinder finder(DIMENSIONS, LENGTH);
    std::cout << "learn...\n";
    bool ok;
    ok = finder.learn("a", makeRandom(DIMENSIONS, LENGTH));
    if (! ok)
    {
        std::cout << "could not learn\n";
        return 1;
    }
    finder.learn("b", makeRandom(DIMENSIONS, LENGTH));
    finder.learn("c", makeRandom(DIMENSIONS, LENGTH));
    finder.learn("d", makeRandom(DIMENSIONS, LENGTH));

    std::string closest;
    std::vector<std::vector<double> > data = makeRandom(DIMENSIONS, LENGTH);
    std::cout << "find closest...\n";
    ok = finder.findClosest(data, closest);
    if (ok)
        std::cout << "closest is " << closest << std::endl;
    else
    {
        std::cout << "failed" << std::endl;
        return 1;
    }
    return 0;
}

