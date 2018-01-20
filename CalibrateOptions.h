#ifndef CALIBRATE_OPTIONS_H
#define CALIBRATE_OPTIONS_H
#include "FunctionalUtilities.h"
#include "neldermead.h"
#include "cuckoo.h"
namespace calibrateoptions{
    template<typename OptionCal, typename T, typename U>
    auto l2normNelderMead(const std::vector<U>& initArgs, const OptionCal& cal, const std::vector<T>& prices, const std::vector<T>& strikes){
        return neldermead::optimize([&](const auto& args){
            return futilities::sum(prices, [&](const auto& val, const auto& index){
                return futilities::const_power(val-cal(strikes[index], args), 2);
            });
        }, initArgs);
    }
    template<typename OptionCal, typename ConstraintFn, typename AlterCostFn, typename Array, typename T, typename U>
    auto l2normNelderMeadVector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const ConstraintFn& constraintFn, 
        const AlterCostFn& alterCostFn, 
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return neldermead::optimize([&](const auto& args){
            return constraintFn(args)?5000.0:futilities::sum(cal(strikes, args), [&](const auto& val, const auto& index){
                return alterCostFn(index)*futilities::const_power(prices[index]-val, 2);
            });
        }, initArgs);
    }
    template<typename OptionCal, typename ConstraintFn, typename Array, typename T, typename U>
    auto l2normNelderMeadVector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const ConstraintFn& constraintFn, 
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return l2normNelderMeadVector(initArgs, cal, constraintFn, [](const auto& index){return 1.0;}, prices, strikes);
    }
    template<typename OptionCal, typename Array, typename T, typename U>
    auto l2normNelderMeadVector(
        const std::vector<U>& initArgs, 
        const OptionCal& cal,
        const std::vector<T>& prices, 
        const Array& strikes
    ){
        return l2normNelderMeadVector(initArgs, cal, 
            [](const auto& args){return false;}, [](const auto& index){return 1.0;}, 
            prices, strikes
        );
    }
    
    constexpr int numNests=25;
    constexpr int maxNumRuns=1000; 
    constexpr int seed=42;//could make this current time but not right now
    template<typename OptionCal, typename AlterCostFn, typename Array1, typename Array2, typename Array3>
    auto l2normCuckooVector(
        const OptionCal& cal,
        const Array1& ul,
        const AlterCostFn& alterCostFn, 
        const Array2& prices, 
        const Array3& strikes
    ){
        return cuckoo::optimize([&](const auto& args){
            return futilities::sum(cal(strikes, args), [&](const auto& val, const auto& index){
                return alterCostFn(index)*futilities::const_power(prices[index]-val, 2);
            });
        }, ul, numNests, maxNumRuns, seed);
    }
    template<typename OptionCal,typename Array1, typename Array2, typename Array3>
    auto l2normCuckooVector(
        const OptionCal& cal,
        const Array1& ul,
        const Array2& prices, 
        const Array3& strikes
    ){
        return l2normCuckooVector(cal, ul, [](const auto& index){return 1.0;}, prices, strikes);
    }
    
}


#endif