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
using System.Linq;
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
			var entries = GetEntries(hexText);
			HexRecord[] records = entries
				.Select(entry => HexRecordParser.FromString(entry))
				.Where(record => record != null)
				.ToArray();
				
			return new Hex(records);
		}

		static string[] GetEntries(string hexText)
		{
			// Split lines
			hexText = hexText.Replace("\r", "");
			char[] separator = { '\n' };
			return hexText.Split(separator, StringSplitOptions.RemoveEmptyEntries);
		}
	}
}
