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
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Arduimmer
{
	public class Hex
	{
		private List<HexRecord> records;

		public Hex()
		{
			this.records = new List<HexRecord>();
		}

		public ReadOnlyCollection<HexRecord> Records {
			get { return new ReadOnlyCollection<HexRecord>(this.records); }
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

			// Split lines
			hexText = hexText.Replace("\r", "");
			string[] lines = hexText.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);

			uint lba = 0;
			foreach (string line in lines) {
				if (string.IsNullOrEmpty(line) || line[0] != HexRecord.Mark)
					continue;

				HexRecord record = HexRecord.FromAsciiString(line);
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

		public void AddRecord(HexRecord record)
		{
			this.records.Add(record);
		}
	}
}

