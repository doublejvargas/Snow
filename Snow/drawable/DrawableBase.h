#pragma once
#include "Drawable.h"
#include "bindable/IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	// Only the first instance of a type of drawable will initialize the static bindables, the remaining instances share the binds and do not need to initialize again.
	// This method for checking if static binds are initialized makes the assumption that the first instance will ALWAYS initialize the static bindables.
	static bool IsStaticInitialized() noexcept
	{
		return !_staticBinds.empty();
	}

	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		_staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert(_pIndexBuffer == nullptr);
		_pIndexBuffer = ibuf.get();
		_staticBinds.push_back(std::move(ibuf));
	}

	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && _pIndexBuffer == nullptr); // sanity check to ensure index buffer hasn't been set for particular instance
		for (const auto& b : _staticBinds)
		{
			// Clever way of checking the if the type is IndexBuffer and at the same time assign it to a variable
			//  my guess is that when the type is not IndexBuffer the dynamic cast will return nullptr and the if statement will be skipped?
			//  dynamic cast is for when we know the type of x in dynamic_cast(x) at runtime only
			//  static cast is for when we know the type of x at compile time.
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get())) 
			{
				_pIndexBuffer = p;
				return;
			}
		}

		assert("Failed to find index buffer in static bindables" && _pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return _staticBinds;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> _staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::_staticBinds;