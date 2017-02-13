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

		Callback createSubTask(double fracSubtask, double startFrac)   { return Callback(this, fracSubtask, startFrac); }
	};

}
