#ifndef __INCLUDE__NEAREST_NEIGHBOR_FINDER_H__
#define __INCLUDE__NEAREST_NEIGHBOR_FINDER_H__

#include <map>
#include <vector>
#include <string>

namespace agesture {

class NearestNeighborFinder
{
    public:
        typedef std::vector<std::vector<double> > Path;
        NearestNeighborFinder(unsigned int dimensions, unsigned int length) :
            dimensions_(dimensions),
            length_(length)
        {
        }
        bool learn(const char *name, const Path &path);
        bool findClosest(const Path &path, std::string &result);
        bool hasKey(const char *name);
    private:
        unsigned int dimensions_;
        unsigned int length_;
        std::map<std::string, Path> learned_;
        double distance(const Path &a, const Path &b);
        bool sizeIsOk(const Path &path);
};

} // end of namespace

#endif

