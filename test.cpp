#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "BSImpliedVolatility.h"
#include "BlackScholes.h"
#include "CalibrateOptions.h"
#include <deque>
#include "get_cf.h"
#include "OptionPricing.h"


TEST_CASE("Test IV Alone Call", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=1;
    const double k=50;
    const double discount=exp(-r*T);
    const double callPrice=3.0;
    const double iv=IV::getIVGeneric(S0, k, discount, callPrice, .1, IV::localBSCall);
    std::cout<<"iv: "<<iv<<std::endl;
    const double callPriceAtVol=BSCall(S0, discount, k, iv);
    REQUIRE(callPriceAtVol==Approx(callPrice));
}
TEST_CASE("Test IV Alone Put", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=1;
    const double k=50;
    const double discount=exp(-r*T);
    const double putPrice=3.0;
    const double iv=IV::getIVGeneric(S0, k, discount, putPrice, .1, IV::localBSPut);
    std::cout<<"iv: "<<iv<<std::endl;
    const double putPriceAtVol=BSPut(S0, discount, k, iv);
    REQUIRE(putPriceAtVol==Approx(putPrice));
}

TEST_CASE("Test IV edge case: negative call", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=.25;
    const double k=42.7673;
    const double discount=exp(-r*T);
    const double callPrice=-5;
    const double iv=IV::getIVGeneric(S0, k, discount, callPrice, .1, IV::localBSCall);
    std::cout<<"iv: "<<iv<<std::endl;
    REQUIRE(iv==-1.0);
}
TEST_CASE("Test IV edge case: negative asset", "[BSIV]"){
    const double r=.03;
    const double S0=-5;
    const double T=.25;
    const double k=42.7673;
    const double discount=exp(-r*T);
    const double callPrice=5;
    const double iv=IV::getIVGeneric(S0, k, discount, callPrice, .1, IV::localBSCall);
    std::cout<<"iv: "<<iv<<std::endl;
    REQUIRE(iv==-1.0);
}


//42.7175,"atPoint":8.96451
TEST_CASE("Test IV another edge case", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=.25;
    const double callPrice=42.7175;
    const double k=8.96451;
    const double discount=exp(-r*T);
    const double iv=IV::getIVGeneric(S0, k, discount, callPrice, 5.0, IV::localBSCall);
    std::cout<<"iv: "<<iv<<std::endl;
    REQUIRE(iv==-1.0);
}
TEST_CASE("Test getAllIVByAsset", "[BSIV]"){
    const double r=.03;
    std::vector<double> S0;
    S0.push_back(40);
    S0.push_back(50);
    S0.push_back(60);
    std::vector<double> callPrice;
    callPrice.push_back(5);
    callPrice.push_back(6);
    callPrice.push_back(15);
    const double T=1;
    const double k=50;
    const double discount=exp(-r*T);
    
    const auto ivByAsset=IV::getAllIVByAssetCall(
        S0, callPrice, k, discount, T, .5
    );

    int n=ivByAsset.size();
    for(int i=0; i<n;++i){
        std::cout<<ivByAsset[i]<<std::endl;
    }
    for(int i=0; i<n;++i){
        const double callPriceAtVol=BSCall(S0[i], discount, k, ivByAsset[i]*sqrt(T));
        REQUIRE(callPrice[i]==Approx(callPriceAtVol));
    }    
}
TEST_CASE("Test getAllIVByStrike", "[BSIV]"){
    const double r=.03;
    std::vector<double> strike;
    strike.push_back(40);
    strike.push_back(50);
    strike.push_back(60);
    std::vector<double> callPrice;
    callPrice.push_back(15);
    callPrice.push_back(6);
    callPrice.push_back(5);
    const double T=1;
    const double S0=50;
    const double discount=exp(-r*T);
    
    const auto ivByStrike=IV::getAllIVByStrikeCall(
      strike, callPrice, S0, discount, T, .5
    );
    int n=ivByStrike.size();
    for(int i=0; i<n;++i){
      std::cout<<ivByStrike[i]<<std::endl;
    }
    
    for(int i=0; i<n;++i){
        const double callPriceAtVol=BSCall(S0, discount, strike[i], ivByStrike[i]*sqrt(T));
        REQUIRE(callPrice[i]==Approx(callPriceAtVol));
    }    
}
TEST_CASE("Test l2norm", "[calibratoptions]"){
    const double r=.03;
    const double T=1;
    const double S0=50;
    const double discount=exp(-r*T);
    const double unknownSigma=.3;
    std::vector<double> strikes={40, 50, 60};
    std::vector<double> prices={
        BSCall(S0, discount, strikes[0], unknownSigma),
        BSCall(S0, discount, strikes[1], unknownSigma),
        BSCall(S0, discount, strikes[2], unknownSigma)
    };
    auto results=calibrateoptions::l2norm(std::vector<double>({.2}), [&](const auto& strike, const auto& args){
        return BSCall(S0, discount, strike, args[0]);
    }, prices, strikes);
    REQUIRE(results[0]==Approx(unknownSigma));
    //std::cout<<"estimated sigma: "<<results[0]<<std::endl;
}
template<typename T>
auto removeFirstAndLast(std::vector<T>&& arr){
    //arr.pop_back();
    //arr.pop_front();
    std::vector<T>   sub(&arr[1],&arr[arr.size()-2]);
    return sub;
}

TEST_CASE("Test l2normvector", "[calibratoptions]"){
    const double r=.03;
    const double T=1;
    const double S0=50;
    const double discount=exp(-r*T);
    const double xMax=7.0;
    const int numU=256;
    auto cgmyCFHOC=cf(r, T, S0, 1.0);
    std::deque<double> strikes={40, 43, 50, 54, 60};
    std::vector<double> prices={
        8, 5, 3, 1.5, .5
    };
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    auto results=calibrateoptions::l2normvector(std::vector<double>({
        .2, 2, 2, .503, .2, .3, .2, .2
    }), [&](const auto& strikes, const auto& args){

        return removeFirstAndLast(optionprice::FangOostCallPrice(
            S0, strikes,
            r, T,
            numU,  
            cgmyCFHOC(
                args[0], args[1], args[2], 
                args[3], args[4], args[5], 
                args[6], args[7]
            )
        ));
    }, [](const auto& args){
        return args[0]<0||args[7]<0;
    }, prices, strikes);
    for(auto& v:results){
        std::cout<<"v: "<<v<<std::endl;
    }
    //REQUIRE(results[0]==Approx(unknownSigma));
}

