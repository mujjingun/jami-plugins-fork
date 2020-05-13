#pragma once

#include <string>
#include <set>

namespace jami {
namespace Preference {

std::set<std::string> parsePreferenceSetValue(const std::string& s) {
    size_t startIndex{1};
    std::set<std::string> a;
    for(size_t i{1}; i< s.size()-1; ++i ){
        if(s[i] == ',') {
            a.insert(s.substr(startIndex, i-startIndex));
            startIndex = i+1;
        } else if ( i == s.size()-2) {
            a.insert(s.substr(startIndex, s.size()-1-startIndex));
        }
    }
    
    return a;
}
}
}

