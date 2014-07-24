/*
 * convertstring.h
 *
 *  Created on: Jul 24, 2014
 *      Author: mathijs
 */

#ifndef CONVERT_H_
#define CONVERT_H_

#include <sstream>
#include <string>

template<typename T> class Convert {
public:
	static const std::string toString(const T value);

	static const T fromString(const std::string str);

};

template<typename T>
const std::string Convert<T>::toString(const T value) {
	std::ostringstream s;
	s << value;
	const std::string result(s.str());
	return result;
}

template<typename T>
const T Convert<T>::fromString(const std::string str) {
	std::istringstream s(str);
	T result;
	s >> result;
	return result;
}

#endif /* CONVERT_H_ */
