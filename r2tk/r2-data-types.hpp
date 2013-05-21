/* HEADER
 *
 * File: r2-data-types.hpp
 * Created by: Rasmus Jarl (Raze Dux) and Lars Woxberg (Rarosu)
 * Created on: August 14 2011
 *
 * License:
 *   Copyright (C) 2011 Rasmus Jarl
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>;.
 *
 * Comments:
 *	This header defines some typedefs for common primitive sizes.
 *
 *	For reference, C++ defines types as following:
 *	+ char:			at least 8 bit (?)
 *	+ short:		at least 16 bit, no bigger than long
 *	+ int:			at least as big as short, no bigger than long
 *	+ long:			at least 32 bit
 *	+ long long:	at least 64 bit
 *
 * Architecture Macros:		http://predef.sourceforge.net/prearch.html
 * Operating System Macros:	http://predef.sourceforge.net/preos.html
 * <climits> reference:		http://www.cplusplus.com/reference/clibrary/climits/
 *
 * Depends on:
 *	+ Linux AMD 64bit architecture.
 *	+ <climits> for [TYPE]_MAX defines.
 * Updates:
 *	2011-08-26 (Rarosu) - Changed the method for determining sizes of types on different systems. Using <climits> instead.
 */
#ifndef R2_DATA_TYPES_HPP
#define R2_DATA_TYPES_HPP

#include <climits>

namespace r2 {
	
	
	/* COMMENTED OUT FOR EVALUATION
	 * SEE METHOD BELOW INSTEAD
	 */
	#ifdef __linux
		#ifdef __amd64__
			typedef signed char SInt8;
			typedef signed short SInt16;
			typedef signed int SInt32;
			typedef signed long long SInt64;
			
			typedef unsigned char UInt8;
			typedef unsigned short UInt16;
			typedef unsigned int UInt32;
			typedef unsigned long long UInt64;
			
			typedef UInt8 Byte;
		#else // __amd64__
			#warning Platform is not supported - fixed size primitives could be non-consistent
		
			typedef signed char SInt8;
			typedef signed short SInt16;
			typedef signed int SInt32;
			typedef signed long long SInt64;
	
			typedef unsigned char UInt8;
			typedef unsigned short UInt16;
			typedef unsigned int UInt32;
			typedef unsigned long long UInt64;
	
			typedef UInt8 Byte;
		#endif
	#else // __linux
		typedef signed char SInt8;
		typedef signed short SInt16;
		typedef signed int SInt32;
		typedef signed long long SInt64;
	
		typedef unsigned char UInt8;
		typedef unsigned short UInt16;
		typedef unsigned int UInt32;
		typedef unsigned long long UInt64;
	
		typedef UInt8 Byte;
	#endif
	
	
	
	/* Idea credited Laurent Gomila (laurent.gom@gmail.com) of the SFML library.
	 * Use <climits> macros to determine the size of different types.
	 * 
	 * This should work on at least Windows, Linux and BSD.
	 *
	 * WARNING - These types need to be TESTED. On Windows 32/64, Linux 32/64.
	 */
	 /*
	 // 8-bit integer types
	 #if UCHAR_MAX = 0xFF
		typedef signed char SInt8;
		typedef unsigned char UInt8;
	 #else
		#warning No 8-bit integer types on this system
	 #endif
	 
	 // 16-bit integer types
	 #if USHRT_MAX == 0xFFFF
		typedef signed short SInt16;
		typedef unsigned short UInt16;
	#elif UINT_MAX = 0xFFFF
		typedef signed int SInt16;
		typedef unsigned int UInt16;
	#elif ULONG_MAX = 0xFFFF
		typedef signed long SInt16;
		typedef unsigned long UInt16;
	#else
		#warning No 16-bit integer types on this system
	#endif
	
	// 32-bit integer types
	#if USHRT_MAX = 0xFFFFFFFF
		typedef signed short SInt32;
		typedef unsigned short UInt32;
	#elif UINT_MAX = 0xFFFFFFFF
		typedef signed int SInt32;
		typedef unsigned int UInt32;
	#elif ULONG_MAX = 0xFFFFFFFF
		typedef signed long SInt32;
		typedef unsigned long UInt32;
	#else
		#warning No 32-bit integer types on this system
	#endif
	
	// 64-bit integer types
	#if USHRT_MAX = 0xFFFFFFFFFFFFFFFF
		typedef signed short SInt64;
		typedef unsigned short UInt64;
	#elif UINT_MAX = 0xFFFFFFFFFFFFFFFF
		typedef signed int SInt64;
		typedef unsigned int UInt64;
	#elif ULONG_MAX = 0xFFFFFFFFFFFFFFFF
		typedef signed long SInt64;
		typedef unsigned long UInt64;
	#else
		#warning No 64-bit integer types on this system
	#endif
	*/
	
	
	
	
	namespace Endian {
		enum Endian { Big, Little };
	}
	
	/* Get the system endianness. This is determined at compile time.
	 */
	Endian::Endian GetSystemEndianness();
	
	UInt16 SwapEndian(UInt16 p_value);
	UInt32 SwapEndian(UInt32 p_value);
	UInt64 SwapEndian(UInt64 p_value);
	
	UInt16 ToNetworkOrder(UInt16 p_value);
	UInt32 ToNetworkOrder(UInt32 p_value);
	UInt64 ToNetworkOrder(UInt64 p_value);
	
	UInt16 ToHostOrder(UInt16 p_value);
	UInt32 ToHostOrder(UInt32 p_value);
	UInt64 ToHostOrder(UInt64 p_value);
}

#endif	/* R2_DATA_TYPES_HPP */
