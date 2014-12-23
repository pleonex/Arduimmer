//
//  HexParser.cs
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
using System.IO;
using System.Text;

namespace Arduimmer
{
	public static class HexParser
	{
		public static Hex FromFile(string hexPath)
		{
			return FromText(File.ReadAllText(hexPath, Encoding.ASCII));
		}

		public static Hex FromText(string hexText)
		{
			var hex = new Hex();

			// Split lines
			hexText = hexText.Replace("\r", "");
			char[] separator = { '\n' };
			string[] lines = hexText.Split(separator, StringSplitOptions.RemoveEmptyEntries);

			uint lba = 0;
			foreach (string line in lines) {
				if (string.IsNullOrEmpty(line) || line[0] != HexRecordParser.Mark)
					continue;

				HexRecord record = HexRecordParser.FromString(line);
				if (record.RecordType == RecordType.Data) {
					record.Address = (lba + record.Address) % 0xFFFFFFFF;
				} else if (record.RecordType == RecordType.ExtendedLinearAddr) {
					lba = (uint)((record.Data[0] << 24) | (record.Data[1] << 16));
					continue;
				} else if (record.RecordType != RecordType.Eof)
					throw new FormatException("The HEX file contains unknown fields.");

				hex.AddRecord(record);
			}

			return hex;
		}
	}
}
