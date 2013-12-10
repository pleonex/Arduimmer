//-----------------------------------------------------------------------
// <copyright file="ArduinoCommunication.cs" company="none">
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
// <date>07/12/2013</date>
//-----------------------------------------------------------------------
using System;
using System.IO.Ports;

namespace Arduimmer
{
	public class ArduinoCommunication
	{
		private const int      BaudRate     = 9600;
		private const Parity   ParityMode   = Parity.None;
		private const int      DataBits     = 8;
		private const StopBits StopBitsMode = StopBits.One;
		private const int      ReadTimeOut  = 1000;
		private const string   NewLine      = "\r\n";

		private SerialPort port;

		public ArduinoCommunication(string portName)
		{
			this.PortName = portName;
			this.port = new SerialPort() { 
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

		protected int DataAvailable {
			get { return this.port.BytesToRead; }
		}

		public void Open()
		{
			this.port.Open();
		}

		public void Close()
		{
			if (this.port.BytesToRead > 0)
				Console.WriteLine("Trash: {0}", this.port.ReadExisting());
			this.port.Close();
		}

		protected void Write(string s)
		{
			this.port.Write(s);
		}

		protected void Write(byte v)
		{
			this.Write(BitConverter.ToString(new byte[] { v }));
		}

		protected void Write(byte[] v)
		{
			this.Write(BitConverter.ToString(v).Replace("-", ""));
		}

		protected void Write(uint v)
		{
			this.Write(BitConverter.GetBytes(v));
		}

		protected string ReadLine()
		{
			return this.port.ReadLine();
		}

		protected string ReadAll()
		{
			return this.port.ReadExisting();
		}
	}
}

