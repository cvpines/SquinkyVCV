#pragma once

#include <memory>
#include <string>

class SRegion;
using SRegionPtr = std::shared_ptr<SRegion>;

class CompiledRegion
{
public:
    CompiledRegion(SRegionPtr);

    // Keys were defaulting to -1, -1, but for drums with no
    // keys at all they were skipped. Better default is "all keys".
#if 0
    int lokey = 0;
    int hikey = 127;
#else
    int lokey = -1;
    int hikey = -1;
#endif
    int onlykey = -1;           // can't lokey and hikey represent this just fine?
    int keycenter = -1;
    std::string sampleFile;
    int lovel = 0;
    int hivel = 127;
};

using CompiledRegionPtr = std::shared_ptr<CompiledRegion>;
