#ifndef __BSIMPLIEDVOLATILITY_H_INCLUDED__
#define __BSIMPLIEDVOLATILITY_H_INCLUDED__
#include "BlackScholes.h"
#include "Newton.h"
#include "FunctionalUtilities.h"
#include <cmath>

namespace IV {
    auto localBSCall=[](const auto& asset, const auto& discount, const auto& strike, const auto& sigma){
        return BSCall(asset, discount, strike, sigma);
    };
    auto localBSPut=[](const auto& asset, const auto& discount, const auto& strike, const auto& sigma){
        return BSPut(asset, discount, strike, sigma);
    };

    template<typename S0, typename K, typename Discount, typename Guess, typename Actual, typename OptionPriceCalc>
    auto getIVGeneric(const S0& asset, const K& strike, const Discount& discount, const Actual& optionPrice, const Guess& guess, const OptionPriceCalc& optionPricer){
        const Guess defaultRet=-1.0;//something ridiculous
        if(asset<0||optionPrice<0){
            return defaultRet; //something ridiculous
        }
        const auto result= newton::zeros(
            [&](const auto& sigma) {
                return optionPricer(asset, discount, strike, sigma)-optionPrice;
            },
            guess,
            .0000001,
            .0000001,
            20
        );
        if(std::isnan(result)){
            return defaultRet;
        }
        else if(std::isinf(result)){
            return defaultRet;
        }
        else{
            return result;
        }
    }

    template<typename ArrOfAsset, typename ArrOfPrice, typename K, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByAssetCall(
        const ArrOfAsset& assets, 
        const ArrOfPrice& optionPrices, 
        const K& k, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            assets,    
            [&](
                const auto& asset, 
                const auto& index
            ){
                return getIVGeneric(asset, k, discount, optionPrices[index], guess, localBSCall)/sqrtMat;
            } 
        );
    }
    template<typename ArrOfAsset, typename ArrOfPrice, typename K, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByAssetPut(
        const ArrOfAsset& assets, 
        const ArrOfPrice& optionPrices, 
        const K& k, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            assets,    
            [&](
                const auto& asset, 
                const auto& index
            ){
                return getIVGeneric(asset, k, discount, optionPrices[index], guess, localBSPut)/sqrtMat;
            } 
        );
    }

    template<typename ArrOfStrike, typename ArrOfPrice, typename S0, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByStrikeCall(
        const ArrOfStrike& strikes, 
        const ArrOfPrice& callPrices, 
        const S0& asset, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            strikes,    
            [&](
                const auto& strike, 
                const auto& index
            ){
                return getIVGeneric(asset, strike, discount, callPrices[index], guess, localBSCall)/sqrtMat;
            } 
        );
    }
    template<typename ArrOfStrike, typename ArrOfPrice, typename S0, typename Discount, typename Maturity, typename Guess>
    auto getAllIVByStrikePut(
        const ArrOfStrike& strikes, 
        const ArrOfPrice& callPrices, 
        const S0& asset, 
        const Discount& discount, 
        const Maturity& timeTillMaturity,
        const Guess& guess
    ){
        const Maturity sqrtMat=sqrt(timeTillMaturity);
        return futilities::for_each_parallel_copy(
            strikes,    
            [&](
                const auto& strike, 
                const auto& index
            ){
                return getIVGeneric(asset, strike, discount, callPrices[index], guess, localBSPut)/sqrtMat;
            } 
        );
    }


}


#endif