//-----------------------------------------------------------------------
// <copyright file="PicProgrammer.cs" company="none">
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
// <date>09/12/2013</date>
//-----------------------------------------------------------------------
using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace Arduimmer
{
	public class PicProgrammer : ArduinoCommunication
	{
		public PicProgrammer(string portName)
			: base(portName)
		{
		}

		public static PicProgrammer SearchArduino()
		{
			// Gets a list of port names
			string[] portNames = null;
			switch (Environment.OSVersion.Platform) {
			case PlatformID.Win32NT:
			case PlatformID.Win32S:
			case PlatformID.WinCE:			// Who knows...?
			case PlatformID.Win32Windows:
				portNames = SerialPort.GetPortNames(); 
				break;

			case PlatformID.Unix:
				portNames = System.IO.Directory.GetFiles("/dev", "ttyACM*");
				break;

			default:
				throw new NotSupportedException("OS not supported");
			}

			// Tries to do a ping in each port until it receives a response
			foreach (string portName in portNames) {
				PicProgrammer arduino = new PicProgrammer(portName);
				try {
					arduino.Open();
					bool isArduino = arduino.Ping();
					arduino.Close();

					if (isArduino)
						return arduino;
				} catch { }
			}

			return null;
		}

		public bool Ping()
		{
			try {
				this.Write("Hey!");
				return this.ReadLine() == "Yes?";
			} catch {
				return false;
			}
		}

		public ushort GetDeviceId()
		{
			this.Write("Dev?");
			return Convert.ToUInt16(this.ReadLine(), 16);
		}

		public void CodeDevice(Hex code)
		{
			Console.WriteLine("Programming sequence started");

			// 1º Erase chip
			this.EraseChip();

			// 2º Send & verify code
			Console.WriteLine("\t* Starting to write code");
			this.WriteCode(code);
			Console.WriteLine("\t* Code written correctly");

			// 3º Send & verify configuration bits
			Console.WriteLine("\t* Starting to write conf bits");
			this.WriteConfBits(code);
			Console.WriteLine("\t* Conf bits written correctly");

			Console.WriteLine("Device programmed correctly ;)");
		}

		private void EraseChip()
		{
			// Check if Arduino is ready
			if (!this.Ping())
				throw new IOException("ERROR Can not communicate with Arduino!");

			// Send Erase Chip command and wait for response
			this.Write("Era!");
			Thread.Sleep(1000);
			Console.WriteLine("\t* {0}", this.ReadLine());
		}

		private void WriteCode(Hex code)
		{
			// Sends Write Code command
			this.Write("Cod!");

			string info = string.Empty;
			uint nextAddress = 0xFFFFFFFF;
			foreach (HexRecord record in code.Records) {
				// Initialize next address
				if (nextAddress == 0xFFFFFFFF)
					nextAddress = record.Address;

				if (record.RecordType == RecordType.Eof || (record.Address & 0xFF0000) != 0x000000) {
					// If there is not more data or the data is not in the code memory...
					//  sends stop command (null address)
					this.Write((uint)0xFFFFFFFF);
				} else {
					if (nextAddress != record.Address) {
						// If the address is no contiguous, Arduino must finish the current
						//  programming sequence and start another one
						this.Write((uint)0xFFFFFFFF);

						Thread.Sleep(100);	// Waits for Arduino

						// Checks if Arduino wants to show info
						if (this.DataAvailable > 0) {
							info = this.ReadAll();
							Console.Write(info);
							if (info.Contains("ERROR"))
								throw new IOException();
						}

						// Let's start again
						this.Write("Cod!");
						nextAddress = record.Address;
					} 

					// Writes code into the buffer
					this.Write((uint)record.Address);
					this.Write((byte)record.Data.Length);
					this.Write(record.Data);
					nextAddress += (uint)record.Data.Length;
				}

				Thread.Sleep(100);	// Waits for Arduino

				// Checks if Arduino wants to show info
				if (this.DataAvailable > 0) {
					info = this.ReadAll();
					Console.Write(info);
					if (info.Contains("ERROR"))
						throw new IOException();
				}
			}
		}

		private void WriteConfBits(Hex code)
		{
			// Sends Write Configuration Bits command
			this.Write("Cnf!");

			string info = string.Empty;
			foreach (HexRecord record in code.Records) {
				if (record.RecordType == RecordType.Eof) {
					// If there is not more data, sends null address
					this.Write((uint)0xFFFFFFFF);
				} else if ((record.Address & 0x00FF0000) == 0x00300000) {
					// Only if the data is in the Configuration bits region... 
					//  writes configuration bits into the buffer
					this.Write((uint)record.Address);
					this.Write((byte)record.Data.Length);
					this.Write(record.Data);
				} else {
					// Else continue checking records
					continue;
				}

				Thread.Sleep(100);	// Waits for Arduino

				// Checks if Arduino wants to show info
				if (this.DataAvailable > 0) {
					info = this.ReadAll();
					Console.Write(info);
					if (info.Contains("ERROR"))
						throw new IOException();
				}
			}
		}
	}
}

