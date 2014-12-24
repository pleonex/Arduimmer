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
using System.IO;

namespace Arduimmer
{
	public static class MainClass
	{
		public static void Main(string[] args)
		{
			ShowHeader();

			if (args.Length != 1) {
				Console.WriteLine("ERROR Invalid number of arguments!");
				ShowHelp();
			} else if (!File.Exists(args[0])) {
				Console.WriteLine("ERROR File not found. {0}", args[0]);
			} else {
				CodeDevice(args[0]);
			}

			Console.WriteLine();
			Console.WriteLine("Good PICing!");
			Console.WriteLine("Press any key to quit. . .");
			Console.ReadKey(true);
		}

		static void ShowHeader()
		{
			Console.WriteLine("Arduimmer  Copyright (C) 2013  Benito Palacios (aka pleonex)");
			Console.WriteLine("This program comes with ABSOLUTELY NO WARRANTY.");
			Console.WriteLine("This is free software, and you are welcome to redistribute it");
			Console.WriteLine("under certain conditions.");
			Console.WriteLine();
		}

		static void ShowHelp()
		{
			Console.WriteLine("USAGE: Arduimmer.exe pathToHexFile");
		}

		static void CodeDevice(string hexPath)
		{
			var portName = Programmer.SearchArduinoPortName();
			if (portName == null) {
				Console.WriteLine("ERROR Can not find Arduino device.");
				return;
			}

			Console.WriteLine("Arduino found at port: {0}", portName);
			Console.WriteLine();

			throw new NotImplementedException();
		}
	}
}
