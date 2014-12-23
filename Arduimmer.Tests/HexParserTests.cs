//
//  HexParserTests.cs
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

namespace Arduimmer.Tests
{
	[TestFixture]
	public class HexParserTests
	{
		[Test]
		public void Parse1Line()
		{
			const string data = ":00000001FF";
			Assert.AreEqual(RecordType.Eof, HexParser.FromText(data).Records[0].RecordType);
		}

		[Test]
		public void Parse2Lines()
		{
			const string data = ":0100000001FE\n:00000001FF";
			var hex = HexParser.FromText(data);
			Assert.AreEqual(2, hex.Records.Count);
			Assert.AreEqual(RecordType.Data, hex.Records[0].RecordType);
			Assert.AreEqual(RecordType.Eof, hex.Records[1].RecordType);
		}

		[Test]
		public void ParseWithComment()
		{
			const string data = ":0100000001FE\n# Hola\n:00000001FF";
			var hex = HexParser.FromText(data);
			Assert.AreEqual(2, hex.Records.Count);
			Assert.AreEqual(RecordType.Data, hex.Records[0].RecordType);
			Assert.AreEqual(RecordType.Eof, hex.Records[1].RecordType);
		}

		[Test]
		public void ParseWithReturn()
		{
			const string data = ":0100000001FE\r\n:00000001FF\r\n";
			var hex = HexParser.FromText(data);
			Assert.AreEqual(2, hex.Records.Count);
			Assert.AreEqual(RecordType.Data, hex.Records[0].RecordType);
			Assert.AreEqual(RecordType.Eof, hex.Records[1].RecordType);
		}
	}
}

