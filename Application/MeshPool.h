#ifndef MESH_POOL_H
#define MESH_POOL_H

#include <vector>

#include <ScopGraphics.h>

class MeshPool
{
	public:
		MeshPool();
		Scop::NonOwningPtr<Scop::Actor> RequestActor(Scop::Scene& scene);
		void ReturnToPool(Scop::NonOwningPtr<Scop::Actor> actor);
		~MeshPool() = default;

	private:
		std::vector<std::pair<bool, Scop::NonOwningPtr<Scop::Actor>>> m_actors;
};

#endif
