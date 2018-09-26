/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>


namespace cv { class Mat; }



namespace CppFW
{
	class CVMatTree;
	class CallbackStepper;
	class Callback;

	class CVMatTreeStructBin
	{
		std::ostream* ostream = nullptr;
		std::istream* istream = nullptr;

		// writer functions
		void writeHeader();
		void writeMatP  (const cv::Mat& mat);
		void writeDir   (const CVMatTree& node);
		void writeList  (const CVMatTree& node);
		void writeString(const std::string& str);

		void handleNodeWrite(const CVMatTree& node);
		
		// reader functions
		bool readHeader();
		bool readMatP  (cv::Mat& mat);
		bool readDir   (CVMatTree& node, CallbackStepper* callbackStepper);
		bool readList  (CVMatTree& node, CallbackStepper* callbackStepper);
		bool readString(std::string& str);

		bool handleNodeRead(CVMatTree& node, CallbackStepper* callbackStepper);

		
		CVMatTreeStructBin(std::ostream& stream) : ostream(&stream) {}
		CVMatTreeStructBin(std::istream& stream) : istream(&stream) {}
		
	public:
		static bool writeBin(      std::ostream& stream , const CVMatTree& tree);
		static bool writeBin(const std::string& filename, const CVMatTree& tree);
		static bool writeBin(const std::string& filename, const cv::Mat& mat);
		
		static CVMatTree readBin(const std::string& filename, Callback* callback = nullptr);
		static CVMatTree readBin(std::istream& stream, CallbackStepper* callbackStepper = nullptr);

		static void writeMatlabReadCode (const char* filename);
		static void writeMatlabWriteCode(const char* filename);
	};

}
