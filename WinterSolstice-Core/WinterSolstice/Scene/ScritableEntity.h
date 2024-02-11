#pragma once 
#include "Entity.h"
namespace WinterSolstice {
	namespace Raiden {
		class ScritableEntity
		{
		public:
			virtual ~ScritableEntity() {};
			template<typename T>
			T& GetComponent()
			{
				return m_Entity.GetComponent<T>();
			}
		protected:
			virtual void OnCreate() {}
			virtual void OnDestory() {}
			virtual void OnUpdate(Kiana::Timestep ts) {}
		private:
			Entity m_Entity;
			friend class Scene;
		};
	}
}