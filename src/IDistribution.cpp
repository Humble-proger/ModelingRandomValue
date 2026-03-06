#include "../include/IDistribution.h"

namespace ModelingRandomValue::Interfaces 
{
    default_random_engine IDistribution::generator(random_device{}());
}