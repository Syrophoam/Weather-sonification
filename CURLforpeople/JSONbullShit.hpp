//
//  JSONbullShit.hpp
//  CURLforpeople
//
//  Created by syro Fullerton on 26/08/2024.


#ifndef JSONbullShit_hpp
#define JSONbullShit_hpp

//int input
std::string parse(std::string JSON, int val[]){
    std::string res;
    int valIndex = 0;
    int indCarry = 0;
    unsigned long strLen = strlen(JSON.c_str());
    
    int numInputs = sizeof(val) / sizeof(int);
    
    for (int i = 0; i < strLen; i++) {
        if((JSON[i] == '%') &&  (JSON[i+1] == 'i')){
            
            if (valIndex == numInputs) {
                break;
            }
            res += JSON.substr(indCarry,i-indCarry);
            res += std::to_string(val[valIndex]);
            valIndex++;
            indCarry = i+2;
        }
    }
    res += JSON.substr(indCarry,strLen);
    return res;
}


#endif /* JSONbullShit_hpp */
