#ifndef TME_ERROR_CODES_HPP
#define TME_ERROR_CODES_HPP

#include "TME/Export.hpp"

namespace tme
{
    enum class ErrorCodes
    {
        Success = 0,
        CompletedWithErrors,
        NetworkEngineNotInitialized,

        Failure
    };  
}

#endif