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
			// DOES NOT RETURN /dev/ttyACM0 !!1!
			//string[] portNames = SerialPort.GetPortNames(); 
			string[] portNames = { "/dev/ttyACM0", "/dev/ttyACM1" };

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
			// 1º Enter Programming mode
			this.EnterProgrammingMode();

			// 2º Erase chip
			this.EraseChip();

			// 3º Send & verify code and data EEPROM
			this.WriteCode(code);
			this.VerifyCode(code);

			// 4º Send & verify configuration bits
			this.WriteConfBits(code);
			this.VerifyConfBits(code);

			// 5º Exit Programming mode
			this.ExitProgrammingMode();
		}

		private void EnterProgrammingMode()
		{
			if (!this.Ping())
				throw new IOException("Can not communicate with Arduino");

			this.Write("Goo!");
		}

		private void ExitProgrammingMode()
		{
			if (!this.Ping())
				throw new IOException("Can not communicate with Arduino");

			this.Write("End!");
		}

		private bool EraseChip()
		{
			if (!this.Ping())
				throw new IOException("Can not communicate with Arduino");

			this.Write("Era!");
			Thread.Sleep(1000);
			return this.ReadLine() == "Erase done";
		}

		private void WriteCode(Hex code)
		{
			// Sends the command
			this.Write("Cod!");

			uint nextAddress = 0xFFFFFFFF;
			foreach (HexRecord record in code.Records) {
				if (nextAddress == 0xFFFFFFFF)
					nextAddress = record.Address;

				if (record.RecordType == RecordType.Eof || (record.Address & 0xFF0000) != 0x00) {
					// If there is not more data or the data is not in the code memory...
					this.Write((uint)0xFFFFFFFF);
				} else {
					// If the address is no contiguous...
					//  Arduino must finish the current programming
					//  and start again
					if (nextAddress != record.Address) {
						this.Write((uint)0xFFFFFFFF);

						// Waits for Aduino
						bool ready = false;
						for (int i = 0; i < 5 && !ready; i++) {
							Thread.Sleep(500);
							ready = this.Ping();
						}

						if (!ready)
							throw new Exception("Arduino does not respond");

						// Let's start again
						this.Write("Cod!");
						nextAddress = record.Address;
					} 

					// Writes code to the buffer
					this.Write((uint)record.Address);
					this.Write((byte)record.Data.Length);
					this.Write(record.Data);
					nextAddress += (uint)record.Data.Length;
				}

				Thread.Sleep(100);	// Waits for Arduino

				// Checks if Arduino wants to show its buffer
				if (this.DataAvailable > 0)
					Console.WriteLine(this.ReadAll());
			}
		}

		private void VerifyCode(Hex code)
		{
			throw new NotImplementedException();
		}

		private void WriteConfBits(Hex code)
		{
			throw new NotImplementedException();
		}

		private void VerifyConfBits(Hex code)
		{
			throw new NotImplementedException();
		}
	}
}

