//
//  HexRecordParser.cs
//
//  Author:
//       Benito Palacios Sánchez <benito356@gmail.com>
//
//  Copyright (c) 2014 Benito Palacios Sánchez
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
using System;
using System.Linq;

namespace Arduimmer
{
	public static class HexRecordParser
	{
		public static char Mark {
			get { return ':'; }
		}

		public static HexRecord FromString(string str)
		{
			// Check if it's a comment or something else (RECORD MARK)
			if (str[0] != Mark)
				return null;

			var record = new HexRecord();

			// Get record length (RECLEN)
			byte length = Convert.ToByte(str.Substring(1, 2), 16);

			// Get data address (LOAD OFFSET)
			record.Address = Convert.ToUInt16(str.Substring(3, 4), 16);

			// Get type (RECTYPE)
			record.RecordType = (RecordType)Convert.ToByte(str.Substring(7, 2), 16);
			if (record.RecordType != RecordType.Data && record.RecordType != RecordType.Eof)
				throw new NotSupportedException();

			// Get data
			record.Data = new byte[length];
			for (int i = 0; i < length; i++)
				record.Data[i] = Convert.ToByte(str.Substring(9 + i * 2, 2), 16);

			if (!ValidateChecksum(str))
				throw new FormatException("Invalid checksum");

			return record;
		}

		static bool ValidateChecksum(string command)
		{
			string data = command.Substring(1);

			// The sum of all bytes must be 0 (includye checksum)
			return (byte)Enumerable.Range(0, data.Length / 2)
				.Select(i => data.Substring(i * 2, 2))
				.Sum(b => Convert.ToSByte(b, 16)) == 0;
		}
	}
}

