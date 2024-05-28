#ifndef CONTAINER_H
#define CONTAINER_H

#include <any>
#include <tuple>
#include <format>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace ioc
{
	template<class T>
	concept Parameterized = requires() {
		{ typename T::IocParams{} };
	};
	template<class T>
	concept NotParameterized = !Parameterized<T>;

	class Container
	{
	public:
		// types
		template<class T>
		using Generator = std::function<std::shared_ptr<T>()>;
		template<class T>
		using ParameterizedGenerator = std::function<std::shared_ptr<T>(typename T::IocParams)>;
	public:
		// functions
		template<Parameterized T>
		void Register(ParameterizedGenerator<T> gen)
		{
			mServiceMap[typeid(T)] = gen;
		}
		template<NotParameterized T>
		void Register(Generator<T> gen)
		{
			mServiceMap[typeid(T)] = gen;
		}
		template<Parameterized T>
		std::shared_ptr<T> Resolve(typename T::IocParams&& params = {}) const
		{
			return Resolve_<T, ParameterizedGenerator<T>>(std::forward<typename T::IocParams>(params));
		}
		template<NotParameterized T>
		std::shared_ptr<T> Resolve() const
		{
			return Resolve_<T, Generator<T>>();
		}
	private:
		// functions
		template<class T, class G, typename...Ps>
		std::shared_ptr<T> Resolve_(Ps&&...arg) const
		{
			// TODO: pull this out of template/header
			if (const auto i = mServiceMap.find(typeid(T)); i != mServiceMap.end())
			{
				const auto& entry = i->second;
				try {
					return std::any_cast<G>(entry)(std::forward<Ps>(arg)...);
				}
				catch (const std::bad_any_cast&) {
					throw std::runtime_error(std::format(
						"Failed to resolve IoC mapped type\nFrom: [{}]\n to: [{}]",
						entry.type().name(), typeid(G).name()
					));
				}
			}
			else
			{
				throw std::runtime_error(std::format(
					"Failed to find generator for type [{}] in IoC container",
					typeid(T).name()
				));
			}
		}
	private:
		// data
		std::unordered_map<std::type_index, std::any> mServiceMap;
	};


	Container& Get() noexcept;
}
#endif // CONTAINER_H
