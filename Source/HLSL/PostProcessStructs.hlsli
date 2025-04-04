struct PostProcessVertexInput
{
    unsigned int index : SV_VertexID;
};

struct PostProcessVertexToPixel
{
    float4 position : SV_POSITION;
    float2 UV : UV;
};

struct PostProcessPixelOutput
{
    float4 color : SV_TARGET;
};

// what the fuck is this
// static const float GaussianKernel5[5] ={ 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };
