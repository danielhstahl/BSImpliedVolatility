#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "BSImpliedVolatility.h"
#include "BlackScholes.h"

TEST_CASE("Test IV Alone", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=1;
    const double k=50;
    const double discount=exp(-r*T);
    const double callPrice=3.0;
    const double iv=IV::getIV(S0, k, discount, callPrice, .1);
    std::cout<<"iv: "<<iv<<std::endl;
    const double callPriceAtVol=BSCall(S0, discount, k, iv);
    REQUIRE(callPriceAtVol==Approx(callPrice));
}

TEST_CASE("Test IV edge case", "[BSIV]"){
    const double r=.03;
    const double S0=50;
    const double T=.25;
    const double k=42.7673;
    const double discount=exp(-r*T);
    const double callPrice=-5;
    const double iv=IV::getIV(S0, k, discount, callPrice, .1);
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
    const double iv=IV::getIV(S0, k, discount, callPrice, 5.0);
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
    
    const auto ivByAsset=IV::getAllIVByAsset(
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
    
    const auto ivByStrike=IV::getAllIVByStrike(
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

