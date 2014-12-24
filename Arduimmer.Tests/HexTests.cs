//
//  HexTests.cs
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
using System.Linq;
using NUnit.Framework;

namespace Arduimmer.Tests
{
	[TestFixture]
	public class HexTests
	{
		[Test]
		public void GetOriginalRecords()
		{
			const string data = ":0100000001FE\n:00000001FF";
			var hex = HexParser.FromText(data);
			Assert.AreEqual(2, hex.Records.Count);
			Assert.AreEqual(RecordType.Data, hex.Records[0].RecordType);
			Assert.AreEqual(RecordType.Eof, hex.Records[1].RecordType);
		}

		[Test]
		public void GroupDataRecordsDifferentSegments()
		{
			const string data = ":0100000001FE\n:0100040002F9\n:00000001FF";
			var hex = HexParser.FromText(data);
			HexRecord[] dataRecords = hex.DataRecords.ToArray();
			Assert.AreEqual(2, dataRecords.Length);

			Assert.AreEqual(RecordType.Data, dataRecords[0].RecordType);
			Assert.AreEqual(0x00, dataRecords[0].Address);
			Assert.AreEqual(0x01, dataRecords[0].Data[0]);

			Assert.AreEqual(RecordType.Data, dataRecords[1].RecordType);
			Assert.AreEqual(0x04, dataRecords[1].Address);
			Assert.AreEqual(0x02, dataRecords[1].Data[0]);
		}

		[Test]
		public void GroupDataRecordsSameSegments()
		{
			const string data = ":0100000001FE\n:020001000201FA\n:0100100001EE\n:00000001FF";
			byte[] data1 = { 0x01, 0x02, 0x01 };
			byte[] data2 = { 0x01 };

			var hex = HexParser.FromText(data);
			HexRecord[] dataRecords = hex.DataRecords.ToArray();
			Assert.AreEqual(2, dataRecords.Length);

			Assert.AreEqual(RecordType.Data, dataRecords[0].RecordType);
			Assert.AreEqual(0x00, dataRecords[0].Address);
			Assert.AreEqual(data1, dataRecords[0].Data);

			Assert.AreEqual(RecordType.Data, dataRecords[1].RecordType);
			Assert.AreEqual(0x10, dataRecords[1].Address);
			Assert.AreEqual(data2, dataRecords[1].Data);
		}

		[Test]
		public void GroupDataRecordsSameSegmentsWithUlba()
		{
			const string data = ":0100000001FE\n:020001000201FA\n" + 
				":02000004028078\n:0100100001EE\n:00000001FF";
			byte[] data1 = { 0x01, 0x02, 0x01 };
			byte[] data2 = { 0x01 };

			var hex = HexParser.FromText(data);
			HexRecord[] dataRecords = hex.DataRecords.ToArray();
			Assert.AreEqual(2, dataRecords.Length);

			Assert.AreEqual(RecordType.Data, dataRecords[0].RecordType);
			Assert.AreEqual(0x00, dataRecords[0].Address);
			Assert.AreEqual(data1, dataRecords[0].Data);

			Assert.AreEqual(RecordType.Data, dataRecords[1].RecordType);
			Assert.AreEqual(0x02800010, dataRecords[1].Address);
			Assert.AreEqual(data2, dataRecords[1].Data);
		}
	}
}

