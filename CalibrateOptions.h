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
    template<typename OptionCal, typename ConstraintFn, typename AlterCostFn, typename Array, typename T, typename U>
    auto l2normvector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const ConstraintFn& constraintFn, 
        const AlterCostFn& alterCostFn, 
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return neldermead::Simplex([&](const auto& args){
            return constraintFn(args)?5000.0:futilities::sum(cal(strikes, args), [&](const auto& val, const auto& index){
                return alterCostFn(index)*futilities::const_power(prices[index]-val, 2);
            });
        }, initArgs);
    }
    template<typename OptionCal, typename ConstraintFn, typename Array, typename T, typename U>
    auto l2normvector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const ConstraintFn& constraintFn, 
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return l2normvector(initArgs, cal, constraintFn, [](const auto& index){return 1.0;}, prices, strikes);
    }
    template<typename OptionCal, typename Array, typename T, typename U>
    auto l2normvector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return l2normvector(initArgs, cal, 
            [](const auto& args){return false;}, [](const auto& index){return 1.0;}, 
            prices, strikes
        );
    }
}


#endif