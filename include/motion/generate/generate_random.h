#pragma once

#include "Generate.h"

namespace Motion
{

class GenerateRandom : public Generate
{
public:
    void generateInternal() override;
};

}  // namespace Motion