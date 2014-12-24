//
//  ArduinoCommunication.cs
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
using System.IO.Ports;
using System.Threading;

namespace Arduimmer
{
	public class ArduinoCommunication
	{
		readonly SerialSocket socket;

		internal ArduinoCommunication(string portName)
		{
			Error = ErrorCodes.NoError;
			socket = new SerialSocket(portName);
			socket.Open();
		}

		public static string SearchArduinoPortName()
		{
			// Tries to do a ping in each port until it receives a response
			foreach (string portName in GetPortNames()) {
				SerialSocket socket = null;
				bool isArduino;

				try {
					socket = new SerialSocket(portName);
					socket.Open();
					isArduino = Ping(socket);
				} finally {
					if (socket != null)
						socket.Close();
				}

				if (isArduino)
					return portName;
			}

			return null;
		}

		public static bool Ping(SerialSocket socket)
		{
			try {
				socket.Write("Hey!");
				return socket.ReadLine() == "Yes?";
			} catch {
				return false;
			}
		}

		static string[] GetPortNames()
		{
			switch (Environment.OSVersion.Platform) {
			case PlatformID.Win32NT:
			case PlatformID.Win32S:
			case PlatformID.WinCE:			// Who knows...?
			case PlatformID.Win32Windows:
				return SerialPort.GetPortNames(); 

			case PlatformID.Unix:
				return Directory.GetFiles("/dev", "ttyACM*");

			default:
				throw new NotSupportedException("OS not supported");
			}
		}

		public ErrorCodes Error {
			get;
			private set;
		}

		public bool Ping()
		{
			return Ping(socket);
		}

		public void Close()
		{
			socket.Close();
		}

		public void WriteCode(string deviceName, int[] ports, Hex code)
		{
			// Clear errros
			Error = ErrorCodes.NoError;

			// Sends Write Code command
			socket.Write("Pro!");

			// Sends device name
			socket.Write(deviceName, 10);

			// Sends the ports
			socket.Write((byte)ports.Length);
			foreach (int p in ports)
				socket.Write((byte)p);

			// Sends the code
			SendCode(code);

			// Wait the result
			bool finished = false;
			while (!finished) {
				Thread.Sleep(100);
				if (socket.DataAvailable == 0)
					continue;

				string result = socket.ReadLine();
				finished = (result == "Done!");

				if (result[0] == 'E')
					Error = (ErrorCodes)Convert.ToByte(result.Substring(1));
			}
		}

		void SendCode(Hex code)
		{
			var dataRecords = code.DataRecords;

			// Sends number of records
			socket.Write((byte)dataRecords.Count);

			// Sends records
			foreach (HexRecord record in dataRecords)
				SendDataRecord(record);
		}

		void SendDataRecord(HexRecord record)
		{
			// Sends address
			socket.Write(record.Address);

			// Sends data length
			socket.Write((uint)record.Data.Length);

			// Sends data
			socket.Write(record.Data);
		}
	}
}
