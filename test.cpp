#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "BSImpliedVolatility.h"
#include "BlackScholes.h"
#include "CalibrateOptions.h"
#include "utils.h"


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
TEST_CASE("Test getAllIVByStrikeCall", "[BSIV]"){
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
TEST_CASE("Test getAllIVByStrikePut", "[BSIV]"){
    const double r=.03;
    std::vector<double> strike;
    strike.push_back(40);
    strike.push_back(50);
    strike.push_back(60);
    std::vector<double> putPrice;
    putPrice.push_back(1);
    putPrice.push_back(6);
    putPrice.push_back(15);
    const double T=1;
    const double S0=50;
    const double discount=exp(-r*T);
    
    const auto ivByStrike=IV::getAllIVByStrikePut(
      strike, putPrice, S0, discount, T, .5
    );
    int n=ivByStrike.size();
    for(int i=0; i<n;++i){
      std::cout<<ivByStrike[i]<<std::endl;
    }
    
    for(int i=0; i<n;++i){
        const double putPriceAtVol=BSPut(S0, discount, strike[i], ivByStrike[i]*sqrt(T));
        REQUIRE(putPrice[i]==Approx(putPriceAtVol));
    }    
}
TEST_CASE("Test l2normneldermead", "[calibratoptions]"){
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
    auto results=calibrateoptions::l2normNelderMead(std::vector<double>({.2}), [&](const auto& strike, const auto& args){
        return BSCall(S0, discount, strike, args[0]);
    }, prices, strikes);
    std::cout<<"fnVal: "<<std::get<1>(results)<<std::endl;
    REQUIRE(std::get<0>(results)[0]==Approx(unknownSigma));
    //std::cout<<"estimated sigma: "<<results[0]<<std::endl;
}
/*TEST_CASE("Test l2normneldermead with varying sigma", "[calibratoptions]"){
    const double r=.03;
    const double T=1;
    const double S0=50;
    const double discount=exp(-r*T);
    //const double unknownSigma=.3;
    std::vector<double> strikes={40, 50, 60};
    std::vector<double> prices={
        BSCall(S0, discount, strikes[0], .4),
        BSCall(S0, discount, strikes[1], .3),
        BSCall(S0, discount, strikes[2], .2)
    };
    auto results=calibrateoptions::l2normNelderMead(std::vector<double>({.2}), [&](const auto& strike, const auto& args){
        return BSCall(S0, discount, strike, args[0]);
    }, prices, strikes);
    std::cout<<"fnVal: "<<std::get<1>(results)<<std::endl;
    //REQUIRE(std::get<0>(results)[0]==Approx(.3));
    //std::cout<<"estimated sigma: "<<results[0]<<std::endl;
}*/
TEST_CASE("Test l2normcuckoo", "[calibratoptions]"){
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
    std::vector<swarm_utils::upper_lower<double> > ul;
    ul.push_back(swarm_utils::upper_lower<double>(0.01, .8));
    auto results=calibrateoptions::l2normCuckooVector([&](const auto& strike, const auto& args){
        return futilities::for_each(0, (int)strike.size(), [&](const auto& index){
            return BSCall(S0, discount, strike[index], args[0]);
        });
    }, ul, prices, strikes, 1000, 42);
    std::cout<<"fnVal: "<<std::get<1>(results)<<std::endl;
    REQUIRE(std::get<0>(results)[0]==Approx(unknownSigma));
}


