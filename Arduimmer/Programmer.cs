//
//  Programmer.cs
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
	public class Programmer
	{
		const int MaxInterationWaiting = 100;
		const string Valid = "Valid!";
		const string Ready = "Ready!";
		const string Ack   = "ACK!";
		const string Done  = "Done!";
		readonly SerialSocket socket;

		internal Programmer(string portName)
		{
			Error = ErrorCode.NoError;
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

		public ErrorCode Error {
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
			Error = ErrorCode.NoError;

			// Sends Write Code command
			socket.Write("Pro!");

			// Sends device name
			socket.Write(deviceName, 10);
			if (!CheckResult(Valid))
				return;

			// Sends the ports
			socket.Write((byte)ports.Length);
			foreach (int p in ports)
				socket.Write((byte)p);

			// Wait untit it's ready or there has been an error
			if (!CheckResult(Ready))
				return;

			// Sends the code
			SendCode(code);

			CheckResult(Done);
		}

		void SendCode(Hex code)
		{
			var dataRecords = code.DataRecords;

			// Sends number of records
			socket.Write((uint)dataRecords.Count);

			// Sends records
			int i = 0;
			int x = Console.CursorLeft;
			int y = Console.CursorTop;
			foreach (HexRecord record in dataRecords) {
				Console.SetCursorPosition(x, y);
				Console.WriteLine("Sending record {0} of {1} ({2:F1}%)",
					++i, dataRecords.Count, i * 100.0 / dataRecords.Count);
				SendDataRecord(record);
				if (!CheckResult(Ack))
					break;
			}
		}

		void SendDataRecord(HexRecord record)
		{
			// Sends address
			socket.Write(record.Address);

			// Sends data length
			socket.Write((ushort)record.Data.Length);

			// Sends data
			socket.Write(record.Data);
		}

		bool CheckResult(string expected)
		{
			// Wait for the result
			for (int i = 0; i < MaxInterationWaiting && socket.DataAvailable == 0; i++)
				Thread.Sleep(50);

			if (socket.DataAvailable == 0) {
				Error = ErrorCode.TimeOut;
				return false;
			}

			// Check all the lines from the input
			bool result = false;
			while (socket.DataAvailable != 0) {
				string line = socket.ReadLine();
				if (line[0] == 'E') {
					Error = (ErrorCode)Convert.ToByte(line.Substring(1, 2), 16);
					break;
				} else if (line == expected) {
					result = true;
					break;
				} else
					Console.WriteLine("## DEBUG ## " + line);
			}

			return (Error == ErrorCode.NoError) ? result : false;
		}
	}
}
