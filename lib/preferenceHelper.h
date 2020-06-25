/*
 *  Copyright (C) 2004-2020 Savoir-faire Linux Inc.
 *
 *  Author: Aline Gondim Santos <aline.gondimsantos@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

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

