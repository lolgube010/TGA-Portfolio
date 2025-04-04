// compute shader!! waooow!!
#include "PortfolioCommon.hlsli"
#include "Common.hlsli"

// Input buffer from mesh voxelizer (it's the same buff, we just run this after that).
RWStructuredBuffer<VoxelInstance> voxelBuff : register(u0);
//RWStructuredBuffer<SmokeData> smokeBuff : register(u1);

// this compute buffer's basically just responsible for getting the 3d position of each voxel and sending that back to us
[numthreads(128, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	uint3 voxelPos = ArrayToVoxel(id.x); // position in voxel space
	uint index = id.x; // array index

	// this voxel is already filled silly, we can ignore it since we don't do any dissipation yet.
	if (IsVoxelFilled(voxelBuff[index].myVoxelState))
	{
		return;
	}

	VoxelInstance data; // data that gets sent to our voxelBuff (to be read by the vs&ps)

	// is our voxel inside wall?
	if (voxelBuff[index].myVoxelState == VOXEL_OCCUPIED)
	{
		data.color = float3(0, 0, 0);
		data.myVoxelState = VOXEL_OCCUPIED;
		voxelBuff[index] = data;
		return;
	}

	data.color = getRandom(index);

	float3 worldPos = momoVoxelToWorld(voxelPos);

	// world to voxel-space
	//uint3 seedPos = AcerolaWorldToVoxel(smokePos);
	uint3 seedPos = momoWorldToVoxel(smokePos);

	if (uint3Equals(voxelPos, seedPos))
	{
		data.myVoxelState = VOXEL_SEED;
		voxelBuff[index] = data;
		return;
	}
	if (drawSmokeSeed)
	{
		data.myVoxelState = VOXEL_EMPTY;
		voxelBuff[index] = data;
		return; // if draw smoke seed, return here since we dgaf about anything else except the smoke seed
	}

//#define skipPropogation
#ifdef skipPropogation
	float3 relativePos = (worldPos - smokePos);
	float3 normalizedPos = relativePos / (smokeRadius * EaseFlatten(smokeTimer, 0.25f));
	//float res = sqrt(relativePos.x * relativePos.x + relativePos.y * relativePos.y + relativePos.z * relativePos.z); // original line, keeping for debbuging ig
	float distanceSquared = dot(normalizedPos, normalizedPos);
	if (distanceSquared <= 1)
	{
		data.myVoxelState = VOXEL_FILLED;
	}
	else
	{
		data.myVoxelState = VOXEL_EMPTY;
	}
	voxelBuff[index] = data;
	return;
#endif

#pragma region neighbourChecking
	VoxelInstance neighbours[6];
	neighbours[0] = voxelBuff[VoxelToArray(float3(voxelPos.x, voxelPos.y, voxelPos.z + 1))]; // in front
	neighbours[1] = voxelBuff[VoxelToArray(float3(voxelPos.x, voxelPos.y, voxelPos.z - 1))]; // behind
	neighbours[2] = voxelBuff[VoxelToArray(float3(voxelPos.x, voxelPos.y + 1, voxelPos.z))]; // above
	neighbours[3] = voxelBuff[VoxelToArray(float3(voxelPos.x, voxelPos.y - 1, voxelPos.z))]; // below
	neighbours[4] = voxelBuff[VoxelToArray(float3(voxelPos.x - 1, voxelPos.y, voxelPos.z))]; // left
	neighbours[5] = voxelBuff[VoxelToArray(float3(voxelPos.x + 1, voxelPos.y, voxelPos.z))]; // right

	bool isNeighbourFilled = false;
	bool doesNeighbourHaveFuel = false;
	uint closestToSeed = 0;
	for (int i = 0; i < 6; ++i) // for every neighbour
	{
		if (IsVoxelFilled(neighbours[i].myVoxelState))
		{
			isNeighbourFilled = true;
			if (neighbours[i].myVoxelState > VOXEL_FILLED)
			{
				doesNeighbourHaveFuel = true;
				if (neighbours[i].myVoxelState > closestToSeed) // always pick the larger number
				{
					closestToSeed = neighbours[i].myVoxelState;
				}
			}
		}
	}
#pragma endregion

	if (isNeighbourFilled && doesNeighbourHaveFuel) // don't run this if voxelstate == 2, since this decrements
	{
		// fill
		float3 relativePos = (worldPos - smokePos);
		float3 normalizedPos = relativePos / (smokeRadius * EaseFlatten(smokeTimer, voxelGrowthSteepness));
		//float res = sqrt(relativePos.x * relativePos.x + relativePos.y * relativePos.y + relativePos.z * relativePos.z); // original line, keeping for debbuging ig
		float distanceSquared = dot(normalizedPos, normalizedPos);


		if (distanceSquared <= 1)
		{
			//uint currentFuel;
			//uint newFuel;
			//uint originalFuel;

			//// Initial check to grab current fuel
			//InterlockedCompareExchange(smokeBuff[0].fuel, 0, 0, currentFuel);
			//if (currentFuel == 0) return; // Fuel’s gone, exit early

			//// Try to decrement atomically
			//do
			//{
			//	currentFuel = smokeBuff[0].fuel; // Read current value (non-atomic, but we’ll verify)
			//	if (currentFuel == 0) return; // Fuel depleted during loop
			//	newFuel = currentFuel - 1; // Propose new value

			//	// Try to swap: if fuel still equals currentFuel, set it to newFuel
			//	InterlockedCompareExchange(smokeBuff[0].fuel, currentFuel, newFuel, originalFuel);
			//} while (originalFuel != currentFuel); // Retry if fuel changed (originalFuel is what it was before swap)

			// If we exit the loop, we successfully decremented fuel from currentFuel to newFuel
			// Proceed (e.g., fill voxel)
			data.myVoxelState = closestToSeed - 1;
		}
		else // your neighbour has fuel, and is filled, but you are outside the range of the smoke.
		{
			data.myVoxelState = VOXEL_EMPTY;
			// todo- check, does smoke still have total fuel?
		}
		voxelBuff[index] = data;

	}
	else if (isNeighbourFilled && !doesNeighbourHaveFuel) // neighbour is filled, but *you* do not have fuel. does the smoke have total fuel?
	{

	}
	else if (!isNeighbourFilled && !doesNeighbourHaveFuel)
	{
		data.myVoxelState = VOXEL_EMPTY;
		voxelBuff[index] = data;
	}
}