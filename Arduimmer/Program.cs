//-----------------------------------------------------------------------
// <copyright file="Program.cs" company="none">
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
// <date>27/11/2013</date>
//-----------------------------------------------------------------------
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

		private static void ShowHeader()
		{
			Console.WriteLine("Arduimmer  Copyright (C) 2013  Benito Palacios (aka pleonex)");
			Console.WriteLine("This program comes with ABSOLUTELY NO WARRANTY.");
			Console.WriteLine("This is free software, and you are welcome to redistribute it");
			Console.WriteLine("under certain conditions.");
			Console.WriteLine();
		}

		private static void ShowHelp()
		{
			Console.WriteLine("USAGE: Arduimmer.exe pathToHexFile");
		}

		private static void CodeDevice(string hexPath)
		{
			Hex code = Hex.FromFile(hexPath);
			PicProgrammer programmer = PicProgrammer.SearchArduino();
			if (programmer == null) {
				Console.WriteLine("ERROR Can not find Arduino device.");
				return;
			}

			programmer.Open();
			Console.WriteLine("Arduino found at port: {0}", programmer.PortName);
			Console.WriteLine("PIC ID: {0:X}h", programmer.GetDeviceId());
			Console.WriteLine();

			try {
				programmer.CodeDevice(code);
			} catch { }

			programmer.Close();
		}
	}
}
