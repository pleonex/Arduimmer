//-----------------------------------------------------------------------
// <copyright file="Hex.cs" company="none">
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
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Arduimmer
{
	public class Hex
	{
		private const char Mark = ':';

		public List<HexRecord> records;

		public Hex()
		{
			this.records = new List<HexRecord>();
		}

		public static Hex FromFile(string hexPath)
		{
			return Hex.FromText(File.ReadAllText(hexPath, Encoding.ASCII));
		}

		public static Hex FromText(string hexText)
		{
			if (string.IsNullOrEmpty(hexText))
				throw new ArgumentNullException();

			Hex hex = new Hex();
			foreach (string line in hexText.Split(new char[] {'\n'}, StringSplitOptions.RemoveEmptyEntries))
				hex.AddRecord(line);

			return hex;
		}

		public void AddRecord(string line)
		{
			if (string.IsNullOrEmpty(line))
				return;

			// Check if it's a comment or something else
			if (line[0] != Mark)
				return;

			// TODO: Parse and add the record
		}

	}
}

