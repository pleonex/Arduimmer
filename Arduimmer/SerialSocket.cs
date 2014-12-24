//
//  SerialSocket.cs
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
using System.IO.Ports;

namespace Arduimmer
{
	public class SerialSocket
	{
		const int      BaudRate     = 9600;
		const Parity   ParityMode   = Parity.None;
		const int      DataBits     = 8;
		const StopBits StopBitsMode = StopBits.One;
		const int      ReadTimeOut  = 1000;
		const string   NewLine      = "\r\n";

		readonly SerialPort port;

		public SerialSocket(string portName)
		{
			PortName = portName;
			port = new SerialPort { 
				PortName    = portName, 
				BaudRate    = BaudRate, 
				Parity      = ParityMode, 
				DataBits    = DataBits, 
				StopBits    = StopBitsMode,
				NewLine     = NewLine,
				ReadTimeout = ReadTimeOut,
				RtsEnable   = true,
			    DtrEnable   = true
			};
		}

		public string PortName {
			get;
			private set;
		}

		public int DataAvailable {
			get { return port.BytesToRead; }
		}

		public void Open()
		{
			port.Open();
		}

		public void Close()
		{
			if (port.BytesToRead > 0)
				Console.WriteLine("Trash: {0}", port.ReadExisting());

			port.Close();
		}

		public void Write(string s)
		{
			port.Write(s);
		}

		public void Write(byte v)
		{
			byte[] data = { v };
			Write(BitConverter.ToString(data));
		}

		public void Write(byte[] v)
		{
			Write(BitConverter.ToString(v).Replace("-", ""));
		}

		public void Write(uint v)
		{
			Write(BitConverter.GetBytes(v));
		}

		public string ReadLine()
		{
			return port.ReadLine();
		}

		public string ReadAll()
		{
			return port.ReadExisting();
		}
	}
}

