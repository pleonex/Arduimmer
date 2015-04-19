//
//  Program.cs
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
using System.Diagnostics;
using System.IO;

namespace Arduimmer
{
	public static class MainClass
	{
		public static void Main(string[] args)
		{
			Stopwatch watch = new Stopwatch();
			watch.Start();

			ShowHeader();

			if (args.Length < 4) {
				Console.WriteLine("ERROR Invalid number of arguments!");
				ShowHelp();
			} else if (!File.Exists(args[0])) {
				Console.WriteLine("ERROR File not found. {0}", args[0]);
			} else {
				CodeDevice(args);
			}

			watch.Stop();
			Console.WriteLine();
			Console.WriteLine("It took {0}", watch.Elapsed);
			Console.WriteLine("Good ICSProgramming!");
			Console.WriteLine("Press any key to quit. . .");
			Console.ReadKey(true);
		}

		static void ShowHeader()
		{
			Console.WriteLine("Arduimmer  Copyright (C) 2015  Benito Palacios (aka pleonex)");
			Console.WriteLine("This program comes with ABSOLUTELY NO WARRANTY.");
			Console.WriteLine("This is free software, and you are welcome to redistribute it");
			Console.WriteLine("under certain conditions.");
			Console.WriteLine();
		}

		static void ShowHelp()
		{
			Console.WriteLine("USAGE: Arduimmer.exe pathToHexFile deviceName pins");
			Console.WriteLine();
			Console.WriteLine("SUPPORTED DEVICES:");
			Console.WriteLine();
			Console.WriteLine("    PIC18F4520    Device name: pic18f");
			Console.WriteLine("                  Pins:        data clock master vpp");
			Console.WriteLine();
			Console.WriteLine("    CC2530        Device name: cc25");
			Console.WriteLine("                  Pins:        data clock reset");
		}

		static void CodeDevice(string[] args)
		{
			var portName = SearchArduinoPort();
			if (portName == null)
				return;

			string hexFilePath = args[0];
			var hex = HexParser.FromFile(hexFilePath);

			string deviceName  = args[1];
			CodeDevice(portName, deviceName, hex, args);
		}

		static string SearchArduinoPort()
		{
			var portName = Programmer.SearchArduinoPortName();
			if (portName == null)
				Console.WriteLine("ERROR Can not find Arduino device.");
			else 
				Console.WriteLine("Arduino found at port: {0}", portName);

			Console.WriteLine();
			return portName;
		}

		static void CodeDevice(string portName, string deviceName, Hex code, string[] args)
		{
			ErrorCode error;
			int[] pins;
			switch (deviceName) {
			case "pic18f":
				pins = GetPins(args, 2, 4);
				if (pins == null)
					return;

				error = ProgrammerFactory.ProgramPic(portName, code, deviceName,
					pins[0], pins[1], pins[2], pins[3]);
				break;

			case "cc25":
				pins = GetPins(args, 2, 3);
				if (pins == null)
					return;

				error = ProgrammerFactory.ProgramCC2530(portName, code, deviceName,
					pins[0], pins[1], pins[2]);
				break;

			default:
				Console.WriteLine("ERROR Device not supported.");
				ShowHelp();
				return;
			}

			if (error != ErrorCode.NoError)
				Console.WriteLine("ERROR " + error.ToString());
		}

		static int[] GetPins(string[] args, int startIdx, int numPins)
		{
			if (args.Length < startIdx + numPins) {
				Console.WriteLine("ERROR Invalid number of arguments");
				ShowHelp();
				return null;
			}

			var pins = new int[numPins];
			for (int i = 0; i < numPins; i++)
				pins[i] = Convert.ToByte(args[startIdx + i]);

			return pins;
		}
	}
}
