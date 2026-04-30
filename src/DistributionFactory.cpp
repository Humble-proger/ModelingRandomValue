#include "../include/DistributionFactory.h"

namespace ModelingRandomValue::Factories
{
    using namespace std;
    using namespace Interfaces;
    
    DistributionFactory* DistributionFactory::instance() 
    {
        static DistributionFactory obj;
        return &obj;
    }

    bool DistributionFactory::registerDistribution(const string &distId, Creator creator) 
    {
        return callbacks.insert({distId, creator}).second;
    }

    bool DistributionFactory::unregisterDistribution(const string &distId) 
    {
        return callbacks.erase(distId) == 1;
    }

    IDistribution* DistributionFactory::createDist(const string &distId) 
    {
        CallbackMap::const_iterator i = callbacks.find(distId);
        if (i == callbacks.end()) 
        {
            throw std::runtime_error("Unknown distribution type: " + distId);
        }
        return (i->second)();
    }
}