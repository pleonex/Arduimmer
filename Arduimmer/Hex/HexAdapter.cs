//
//  HexAdapter.cs
//
//  Author:
//       Benito Palacios Sánchez <benito356@gmail.com>
//
//  Copyright (c) 2015 Benito Palacios Sánchez
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
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace Arduimmer
{
	public static class HexAdapter
	{
		private const int MaxBufferSize = 0x10;

		public static IList<HexRecord> AdaptCode(HexRecord[] records)
		{
			var dataRecords = GroupDataRecords(records);
			var normalized  = NormalizeAddress(dataRecords);
			return normalized;
		}

		private static IList<HexRecord> GroupDataRecords(HexRecord[] records)
		{
			var dataRecords = new List<HexRecord>();
			uint upperLoadBaseAddress = 0;

			foreach (HexRecord entry in records) {
				// Set the upper part of the address
				if (entry.RecordType == RecordType.ExtendedLinearAddr)
					upperLoadBaseAddress = entry.Address;

				if (entry.RecordType != RecordType.Data)
					continue;

				// Calculate the full entry addres
				uint address = upperLoadBaseAddress + entry.Address;

				// Add to the list
				dataRecords.Add(new HexRecord { 
					Address    = address, 
					Data       = entry.Data,
					RecordType = RecordType.Data
				});
			}

			return dataRecords;
		}

		private static IList<HexRecord> NormalizeAddress(IList<HexRecord> records)
		{
			int programSize = (int)records.Max(r => r.Address + r.Data.Length);
			byte[] program  = new byte[programSize];

			// Build the entire program (in micros it's only a few kb's, I hope)
			foreach (HexRecord entry in records)
				entry.Data.CopyTo(program, entry.Address);

			// Now, divide the full program into chunks of 0x10 bytes
			var programRecords = new List<HexRecord>(programSize / MaxBufferSize);
			for (int i = 0; i < programSize; i += MaxBufferSize) {
				programRecords.Add(new HexRecord {
					Address = (uint)i,
					Data = program.Skip(i).Take(MaxBufferSize).ToArray(),
					RecordType = RecordType.Data
				});
			}

			return programRecords;
		}
	}
}

