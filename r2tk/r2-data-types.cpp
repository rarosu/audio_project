/* SOURCE
 *
 * File: r2-data-types.cpp
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
 *
 * Updates:
 */

#include "r2-data-types.hpp"

namespace r2 {
	Endian::Endian GetSystemEndianness() {
		// if it is big-endian, then the most significant byte (c[0]) will hold 0x01
		// if it is little-endian, the most significant byte (c[0]) will hold 0x04 
		union {
			UInt32 i;
			Byte c[4];
		} endian_test = { 0x01020304u };
		
		return ( (endian_test.c[0] == 0x01) ? Endian::Big : Endian::Little ); 
	}

	
	UInt16 SwapEndian(UInt16 p_value) {
		return ((p_value & 0xff00u) >> 8) |
			   ((p_value & 0x00ffu) << 8);
	}
	
	UInt32 SwapEndian(UInt32 p_value) {
		return ((p_value & 0xff000000u) >> 24) |
			   ((p_value & 0x00ff0000u) >> 8)  |
			   ((p_value & 0x0000ff00u) << 8)  |
			   ((p_value & 0x000000ffu) << 24);
	}
	
	UInt64 SwapEndian(UInt64 p_value) {
		return ((p_value & 0xff00000000000000ull) >> 56) |
			   ((p_value & 0x00ff000000000000ull) >> 40) |
			   ((p_value & 0x0000ff0000000000ull) >> 24) |
			   ((p_value & 0x000000ff00000000ull) >> 8)  |
			   ((p_value & 0x00000000ff000000ull) << 8)  |
			   ((p_value & 0x0000000000ff0000ull) << 24) |
			   ((p_value & 0x000000000000ff00ull) << 40) |
			   ((p_value & 0x00000000000000ffull) << 56);
	}
	



	UInt16 ToNetworkOrder(UInt16 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}

		return p_value;
	}
	
	UInt32 ToNetworkOrder(UInt32 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}

		return p_value;
	}
	
	UInt64 ToNetworkOrder(UInt64 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}

		return p_value;
	}
	



	UInt16 ToHostOrder(UInt16 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}

		return p_value;
	}
	
	UInt32 ToHostOrder(UInt32 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}

		return p_value;
	}
	
	UInt64 ToHostOrder(UInt64 p_value) {
		if (GetSystemEndianness() == Endian::Little) {
			p_value = SwapEndian(p_value);
		}
		
		return p_value;
	}
	
	
}