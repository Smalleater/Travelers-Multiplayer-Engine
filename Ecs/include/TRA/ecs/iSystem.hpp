#ifndef TRA_ECS_I_SYSTEM_HPP
#define TRA_ECS_I_SYSTEM_HPP

namespace tra::ecs
{
	class Engine;

	struct ISystem
	{
		virtual void update(Engine* _engine) = 0;
	};
}

#endif
