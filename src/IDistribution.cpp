#include "../include/IDistribution.h"

namespace ModelingRandomValue::Interfaces 
{
    std::default_random_engine IDistribution::generator(std::random_device{}());
}