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
using NUnit.Framework;
using System;

namespace Arduimmer.Tests
{
	[TestFixture]
	public class HexRecordParserTests
	{
		[Test]
		public void Mark()
		{
			Assert.AreEqual(':', HexRecordParser.Mark);
		}

		[Test]
		public void IncorrectMark()
		{
			Assert.IsNull(HexRecordParser.FromString("#00000001FF"));
		}

		[Test]
		public void CorrectLength()
		{
			Assert.AreEqual(2, HexRecordParser.FromString(":02111100333376").Data.Length);
		}

		[Test]
		public void CorrectAddress()
		{
			Assert.AreEqual(0xCAFE, HexRecordParser.FromString(":00CAFE0038").Address);
		}

		[Test]
		public void SupportDataType()
		{
			var expected = new byte[] { 0xCA, 0xFE, 0xDA, 0xDA };
			const string command = ":04CACA00CAFEDADAEC";
			var record = HexRecordParser.FromString(command);

			Assert.AreEqual(RecordType.Data, record.RecordType);
			Assert.AreEqual(expected, record.Data);
		}

		[Test]
		public void SupportEOF()
		{
			var record = HexRecordParser.FromString(":00000001FF");
			Assert.AreEqual(RecordType.Eof, record.RecordType);
		}

		[Test]
		public void NotSupportExtendedSegAddr()
		{
			const string command = ":00000002FE";
			Assert.Throws<NotSupportedException>(() => HexRecordParser.FromString(command));
		}

		[Test]
		public void NotSupportStartSegAddr()
		{
			const string command = ":00000003FD";
			Assert.Throws<NotSupportedException>(() => HexRecordParser.FromString(command));
		}

		[Test]
		public void NotSupportExtendedLinearAddr()
		{
			const string command = ":02000004028078";
			Assert.AreEqual(0x02800000, HexRecordParser.FromString(command).Address);
		}

		[Test]
		public void NotSupportStartLinearAddr()
		{
			const string command = ":00000005FB";
			Assert.Throws<NotSupportedException>(() => HexRecordParser.FromString(command));
		}

		[Test]
		public void ValidChecksum()
		{
			const string command = ":0200000011ED";
			Assert.DoesNotThrow(() => HexRecordParser.FromString(command));
		}

		[Test]
		public void InvalidChecksum()
		{
			const string command = ":0200000011AB";
			Assert.IsNull(HexRecordParser.FromString(command));
		}
	}
}

