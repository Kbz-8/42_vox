#ifndef NOISE_H
#define NOISE_H

#include <ScopMaths.h>

class Noise
{
	public:
		Noise() = default;

		[[nodiscard]] std::uint32_t GetHeight(Scop::Vec2i pos);

		~Noise() = default;

	private:
};

#endif
