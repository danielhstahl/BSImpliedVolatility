#ifndef CALIBRATE_OPTIONS_H
#define CALIBRATE_OPTIONS_H
#include "FunctionalUtilities.h"
#include "neldermead.h"

namespace calibrateoptions{
    template<typename OptionCal, typename T, typename U>
    auto l2norm(const std::vector<U>& initArgs, const OptionCal& cal, const std::vector<T>& prices, const std::vector<T>& strikes){
        return neldermead::Simplex([&](const auto& args){
            return futilities::sum(prices, [&](const auto& val, const auto& index){
                return futilities::const_power(val-cal(strikes[index], args), 2);
            });
        }, initArgs);
    }
}


#endif