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

#include <cstddef>

namespace CppFW
{

	class Callback
	{
		Callback* parent = nullptr;
		double factor = 1;
		double add    = 0;

		Callback(Callback* parent, double factor, double add) : parent(parent), factor(factor), add(add) {}

	public:
		Callback() = default;
		virtual ~Callback()                                            {}
		virtual bool callback(double frac)                             { if(parent) return parent->callback(frac*factor + add); return true; }
		virtual void calcFail(int /*step*/ = -1)                       {}

		Callback createSubTask(double fracSubtask, double startFrac)
		{
			if(parent)
				return Callback(parent, fracSubtask*factor, startFrac*factor + add);
			return Callback(this, fracSubtask, startFrac);
		}
	};


	class CallbackSubTaskCreator
	{
		Callback* callback;

		double taskSize;
		double actTask = 0;
	public:
		CallbackSubTaskCreator(Callback* callback, std::size_t numTasks) : callback(callback), taskSize(1./static_cast<double>(numTasks)) {}
		Callback getSubTaskCallback(std::size_t combinedTasks = 1)
		{
			if(!callback)
				return Callback();

			double actCallbackPos = actTask*taskSize;
			callback->callback(actCallbackPos);
			actTask += combinedTasks;
			return callback->createSubTask(taskSize*combinedTasks, actCallbackPos);
		}
	};


	class CallbackStepper
	{
		Callback* callback;

		double taskSize;
		double actTask = 0;

		bool callCallback()
		{
			if(callback)
			{
				double actCallbackPos = actTask*taskSize;
				return callback->callback(actCallbackPos);
			}
			return true;
		}

	public:
		CallbackStepper(Callback* callback, std::size_t numTasks) : callback(callback), taskSize(1./static_cast<double>(numTasks)) {}
		bool operator++()
		{
			actTask += 1;
			return callCallback();
		}

		bool setStep(std::size_t stepNr)
		{
			actTask = static_cast<double>(stepNr);
			return callCallback();
		}
	};

}
