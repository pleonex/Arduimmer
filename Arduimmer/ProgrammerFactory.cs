//
//  ProgrammerFactory.cs
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
namespace Arduimmer
{
	public static class ProgrammerFactory
	{
		public static ErrorCodes ProgramPic(string portName, Hex code, string deviceName,
			int dataPin, int clockPin, int masterPin, int vppPin)
		{
			int[] ports = { dataPin, clockPin, masterPin, vppPin };
			return ProgramDevice(portName, code, deviceName, ports);
		}

		public static ErrorCodes ProgramCC2530(string portName, Hex code, string deviceName,
			int dataPin, int clockPin, int resetPin)
		{
			int[] ports = { dataPin, clockPin, resetPin };
			return ProgramDevice(portName, code, deviceName, ports);
		}

		static ErrorCodes ProgramDevice(string portName, Hex code, string deviceName, int[] ports)
		{
			var programmer = new Programmer(portName);
			if (!programmer.Ping())
				return ErrorCodes.DisconnectedDevice;
				
			programmer.WriteCode(deviceName, ports, code);

			programmer.Close();
			return programmer.Error;
		}
	}
}

