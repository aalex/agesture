#include "nearestneighborfinder.h"
#include <iostream>
#include <cmath>
#include <limits>

namespace agesture {

bool NearestNeighborFinder::learn(const char *name, const NearestNeighborFinder::Path &data)
{
    if (this->hasKey(name))
    {
        std::cout << "Already has key " << name << std::endl;
        return false;
    }
    if (! this->sizeIsOk(data))
    {
        std::cout << "size not ok" << std::endl;
        return false;
    }
    this->learned_[std::string(name)] = data;
    return true;
}

bool NearestNeighborFinder::hasKey(const char *name)
{
    return (learned_.find(std::string(name)) != learned_.end());
}

bool NearestNeighborFinder::sizeIsOk(const NearestNeighborFinder::Path &path)
{
    if (path.size() != this->length_)
    {
        std::cout << "Wrong length " << path.size() << std::endl;
        return false;
    }
    std::vector<std::vector<double> >::const_iterator iter;
    for (iter = path.begin(); iter != path.end(); ++iter)
    {
        unsigned int size = (*iter).size();
        if (size != this->dimensions_)
        {
            std::cout << "Wrong dimensions " << size << std::endl;
            return false;
        }
    }
    return true;
}

bool NearestNeighborFinder::findClosest(const NearestNeighborFinder::Path &data, std::string &result)
{
    if (! this->sizeIsOk(data))
    {
        std::cout << "size not ok" << std::endl;
        return false;
    }
    if (learned_.size() == 0)
    {
        std::cout << "No learned data" << std::endl;
        return false;
    }
    // TODO

    std::map<std::string, double> distances;
    // calculate distance from each stored path:
    {
        std::map<std::string, Path>::const_iterator iter;
        for (iter = this->learned_.begin(); iter != this->learned_.end(); iter++)
        {
            double dist = this->distance((*iter).second, data);
            distances[(*iter).first] = dist;
        }
    }

    std::map<std::string, double>::const_iterator iter2;
    std::string closest;
    double closest_distance = std::numeric_limits<double>::max();
    // look for closest:
    {
        for (iter2 = distances.begin(); iter2 != distances.end(); iter2++)
        {
            if ((*iter2).second < closest_distance)
            {
                closest_distance = (*iter2).second;
                closest = (*iter2).first;
            }
        }
    }
    result = closest;
    return true;
}

double NearestNeighborFinder::distance(const NearestNeighborFinder::Path &a, const NearestNeighborFinder::Path &b)
{
    double cumulative_distance = 0.0;
    for (unsigned int i = 0; i < this->length_; i++)
    {
        double tmp = 0.0;
        for (unsigned int j = 0; j < this->dimensions_; j++)
        {
            tmp += a[i][j] * a[i][j] + b[i][j] * b[i][j];
        }
        cumulative_distance += std::sqrt(tmp);
    }
    return cumulative_distance;
}

} // end of namespace

