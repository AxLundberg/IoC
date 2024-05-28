#include "Container.h"
#include "Singletons.h"

namespace ioc
{
	ioc::Container& Get() noexcept
	{
		static ioc::Container container;
		return container;
	}

	Singletons& Sing()
	{
		static Singletons sing;
		return sing;
	}
}


