//-----------------------------------------------------------------------
// <copyright file="HexRecord.cs" company="none">
// Copyright (C) 2013
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details. 
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see "http://www.gnu.org/licenses/". 
// </copyright>
// <author>pleoNeX</author>
// <email>benito356@gmail.com</email>
// <date>27/11/2013</date>
//-----------------------------------------------------------------------
using System;

public enum RecordType {
	Data               = 00,	// 8, 16, 32
	Eof                = 01,	// 8, 16, 32
	ExtendedSegAddr    = 02,	// 16, 32
	StartSegAddr       = 03,	// 16, 32
	ExtendedLinearAddr = 04,	// 32
	StartLinearAddr    = 05,	// 32
};

namespace Arduimmer
{
	public class HexRecord
	{
		public static char Mark {
			get { return ':'; }
		}

		public ushort Address {
			get;
			set;
		}

		public RecordType RecordType {
			get;
			set;
		}

		public byte[] Data {
			get;
			set;
		}

		public static HexRecord FromAsciiString(string str)
		{
			// Check if it's a comment or something else (RECORD MARK)
			if (str[0] != Mark)
				return null;

			// TODO: Parse and add the record
			HexRecord record = new HexRecord();

			// Get record length (RECLEN)
			byte length = Convert.ToByte(str.Substring(1, 2), 16);

			// Get data address (LOAD OFFSET)
			record.Address = Convert.ToUInt16(str.Substring(3, 4), 16);

			// Get type (RECTYPE)
			record.RecordType = (RecordType)Convert.ToByte(str.Substring(7, 1), 16);

			// Get data
			record.Data = new byte[length];
			for (int i = 0; i < length; i++)
				record.Data[i] = Convert.ToByte(str.Substring(8 + i * 2, 2), 16);

			// TODO: Check shecksum

			return record;
		}
	}
}

