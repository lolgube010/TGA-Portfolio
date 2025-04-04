#pragma once
#include <vector>
// TODO- RENAME AND PUSH SOMEWHERE ELSE!!

namespace TerrainManip
{
	// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
	// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
	typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
#pragma warning(disable:4146)
	inline uint32_t pcg32_random_r(pcg32_random_t* rng)
	{
		uint64_t oldstate = rng->state;
		// Advance internal state
		rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
		// Calculate output function (XSH RR), uses old state for max ILP
		uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	// Returns a random float between 0.0 and 1.0
	inline float RandomFloat()
	{
		thread_local pcg32_random_t state;
		return (float)ldexp(pcg32_random_r(&state), -32);
	}

	inline void AddNoise(std::vector<float>& buffer, float amount)
	{
		for (float& i : buffer)
		{
			i += (2.0f * RandomFloat() - 1.0f) * amount;
		}
	}

	// same as a subdivision
	inline std::vector<float> Upsample2X(const std::vector<float>& input, int resolution)
	{
		std::vector<float> output(2 * resolution * 2 * resolution);

		// corners
		output[0] = input[0];
		output[2 * resolution * (2 * resolution - 1)] = input[resolution * (resolution - 1)];
		output[2 * resolution - 1] = input[resolution - 1];
		output[2 * resolution * (2 * resolution - 1) + (2 * resolution - 1)] = input[resolution * (resolution - 1) + (resolution - 1)];

		for (int i = 0; i < resolution - 1; i++)
		{
			// edge where j = 0
			{
				float in0 = input[resolution * (i + 0)];
				float in1 = input[resolution * (i + 1)];

				output[2 * resolution * (2 * i + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// interior
			for (int j = 0; j < resolution - 1; j++)
			{
				float in00 = input[resolution * (i + 0) + j + 0];
				float in01 = input[resolution * (i + 1) + j + 0];
				float in10 = input[resolution * (i + 0) + j + 1];
				float in11 = input[resolution * (i + 1) + j + 1];

				output[2 * resolution * (2 * i + 1) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 3.0f * in00 + 3.0f * 1.0f * in01 + 1.0f * 3.0f * in10 + 1.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 1.0f * in00 + 3.0f * 3.0f * in01 + 1.0f * 1.0f * in10 + 1.0f * 3.0f * in11);
				output[2 * resolution * (2 * i + 1) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 3.0f * in00 + 1.0f * 1.0f * in01 + 3.0f * 3.0f * in10 + 3.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 1.0f * in00 + 1.0f * 3.0f * in01 + 3.0f * 1.0f * in10 + 3.0f * 3.0f * in11);
			}

			// edge where j = is resolution-1
			{
				float in0 = input[resolution * (i + 0) + resolution - 1];
				float in1 = input[resolution * (i + 1) + resolution - 1];

				output[2 * resolution * (2 * i + 1) + 2 * resolution - 1] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2) + 2 * resolution - 1] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		for (int j = 0; j < resolution - 1; j++)
		{
			// edge where i = 0
			{
				float in0 = input[j + 0];
				float in1 = input[j + 1];

				output[(2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[(2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// edge where i = is resolution-1
			{
				float in0 = input[resolution * (resolution - 1) + j + 0];
				float in1 = input[resolution * (resolution - 1) + j + 1];

				output[2 * resolution * (2 * resolution - 1) + (2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * resolution - 1) + (2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		return output;
	}
}