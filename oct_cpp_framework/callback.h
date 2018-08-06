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
