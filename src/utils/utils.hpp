// 
// File:   utils.h
// Author: weimer
//
// Created on January 28, 2008, 3:59 PM
//

#ifndef _UTILS_H
#define	_UTILS_H
template<class T> inline std::string to_string(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

#endif	/* _UTILS_H */

