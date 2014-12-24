//
//  Hex.cs
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
using System.Collections.ObjectModel;
using System.Collections.Generic;

namespace Arduimmer
{
	public class Hex
	{
		readonly HexRecord[] records;
		readonly HexRecord[] dataRecords;

		public Hex(HexRecord[] records)
		{
			this.records = records;
			dataRecords = GroupDataRecords(records);
		}

		public ReadOnlyCollection<HexRecord> Records {
			get { return new ReadOnlyCollection<HexRecord>(records); }
		}

		public ReadOnlyCollection<HexRecord> DataRecords {
			get { return new ReadOnlyCollection<HexRecord>(dataRecords); }
		}

		static HexRecord[] GroupDataRecords(HexRecord[] records)
		{
			var dataRecords = new List<HexRecord>();

			uint upperLoadBaseAddress = 0;
			uint startAddress = 0xFFFFFFFF;
			var currentData = new List<byte>();
			foreach (HexRecord entry in records) {
				// Set the upper part of the address
				if (entry.RecordType == RecordType.ExtendedLinearAddr)
					upperLoadBaseAddress = entry.Address;

				if (entry.RecordType != RecordType.Data)
					continue;

				// Calculate the current entry addres
				uint address = upperLoadBaseAddress + entry.Address;

				// Initialize the buffer address if so
				if (startAddress == 0xFFFFFFFF)
					startAddress = address;

				// If the address are not contiguous, create data record and start again
				if (address != startAddress + currentData.Count) {
					dataRecords.Add(new HexRecord() { 
						Address    = startAddress, 
						Data       = currentData.ToArray(),
						RecordType = RecordType.Data
					});

					// Start again
					currentData.Clear();
					startAddress = address;
				}

				// Add the content of this entry to the buffer
				currentData.AddRange(entry.Data);
			}

			// Create the record for the buffer
			dataRecords.Add(new HexRecord() { 
				Address    = startAddress, 
				Data       = currentData.ToArray(),
				RecordType = RecordType.Data
			});

			return dataRecords.ToArray();
		}
	}
}

