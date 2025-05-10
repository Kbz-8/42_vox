#include <algorithm>

#include <MeshPool.h>
#include <World.h>

MeshPool::MeshPool() : m_actors(4 * (RENDER_DISTANCE * RENDER_DISTANCE)) {}

Scop::NonOwningPtr<Scop::Actor> MeshPool::RequestActor(Scop::Scene& scene)
{
	for(auto& [free, actor] : m_actors)
	{
		if(free)
		{
			free = false;
			actor->SetVisibility(true);
			return actor;
		}
	}

	std::vector<Scop::Vertex> mesh_data(CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z);
	std::vector<std::uint32_t> indices(CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z);
	std::shared_ptr<Scop::Mesh> mesh = std::make_shared<Scop::Mesh>();
	mesh->AddSubMesh({ std::move(mesh_data), std::move(indices) });
	m_actors.push_back(std::make_pair(false, &scene.CreateActor(mesh)));
	return m_actors.back().second;
}

void MeshPool::ReturnToPool(Scop::NonOwningPtr<Scop::Actor> actor)
{
	auto it = std::find_if(m_actors.begin(), m_actors.end(), [actor](const auto& lhs) { return lhs.second == actor; });
	if(it == m_actors.end())
		return;
	it->first = true;
	it->second->SetVisibility(false);
}
