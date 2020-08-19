#include "EmlidTypes.hpp"

NAV::sensors::emlid::ErbMessageID NAV::sensors::emlid::getMsgIdFromString(const std::string_view& idName)
{
    if (idName == "VER")
    {
        return ErbMessageID::ERB_MessageId_VER;
    }
    if (idName == "POS")
    {
        return ErbMessageID::ERB_MessageId_POS;
    }
    if (idName == "STAT")
    {
        return ErbMessageID::ERB_MessageId_STAT;
    }
    if (idName == "DPOS")
    {
        return ErbMessageID::ERB_MessageId_DPOS;
    }
    if (idName == "VEL")
    {
        return ErbMessageID::ERB_MessageId_VEL;
    }
    if (idName == "SVI")
    {
        return ErbMessageID::ERB_MessageId_SVI;
    }
    if (idName == "RTK")
    {
        return ErbMessageID::ERB_MessageId_RTK;
    }

    return ErbMessageID::ERB_MessageId_NONE;
}