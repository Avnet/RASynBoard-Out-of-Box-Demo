/*
 * versionString.h
 *
 *  Created on: Jul 7, 2023
 *      Author: 051520
 */

#ifndef VERSION_STRING_H_
#define VERSION_STRING_H_

// Increment Major Version when using a new library that breaks any ML model backwards compatability
#define MAJOR_VERSION "1"

// Increment Minor Version when new features are added
#define MINOR_VERSION "0"

// Increment Build Version when Major and Minor versions do not change
// If Major or Minor versions change, reset BUILD_VERSION to 0
#define BUILD_VERSION "0"
#define DOT "."

#define VERSION_STRING MAJOR_VERSION DOT MINOR_VERSION DOT BUILD_VERSION
#define RELEASE_DATE "7/7/23"

#endif /* VERSION_STRING_H_ */
