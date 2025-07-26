#ifndef TME_ERROR_CODES_HPP
#define TME_ERROR_CODES_HPP

#include "TME/Export.hpp"

namespace tme
{
    enum class TME_API ErrorCodes
    {
        Success = 0,
        PartialSuccess,
        NetworkEngineNotInitialized,
    };  
}

#endif