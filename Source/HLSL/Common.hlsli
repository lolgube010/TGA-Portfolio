

// yo.
// yoyo.
// listen here.
// this hlsli is exclusively for common defines and functions. nothing more.
// CommonBuffers.hlsli for buffers.



// resharper shit
// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppLocalVariableMayBeConst

////////////// glsl stuff

// https://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod explanation 
float mod_GLSL(const float x, const float y)
{
    return x - y * floor(x / y);
}
float mod_HLSL(const float x, const float y)
{
    return x - y * trunc(x / y);
}

#define vec2 float2
#define vec3 float3
#define vec4 float4
#define mat2 float2x2
#define mat3 float3x3
#define mat4 float4x4
#define bvec2 bool2
#define bvec3 bool3
#define bvec4 bool4
#define ivec2 int2
#define ivec3 int3
#define ivec4 int4
#define uvec2 uint2
#define uvec3 uint3
#define uvec4 uint4

// https://anteru.net/blog/2016/mapping-between-HLSL-and-GLSL/
#define gl_ClipDistance SV_ClipDistance
#define gl_FragDepth SV_Depth
#define gl_Position SV_Position
#define gl_Position SV_Position
#define gl_FragCoord SV_Position
#define gl_FragColor SV_Target
#define dFdx ddx
#define dFdxCoarse ddx_coarse
#define dFdxFine ddx_fine
#define dFdy ddy
#define dFdyCoarse ddy_coarse
#define dFdyFine ddy_fine
#define fract frac
#define mix lerp
#define fma mad

////////////// unity stuff
// mostly stolen from
// https://github.com/Unity-Technologies/Graphics/blob/master/Packages/com.unity.render-pipelines.core/ShaderLibrary/Common.hlsl#L888
// and
// https://github.com/Unity-Technologies/Graphics/blob/master/Packages/com.unity.render-pipelines.core/ShaderLibrary/Macros.hlsl#L127

#define PI          3.14159265358979323846
#define TWO_PI      6.28318530717958647693
#define FOUR_PI     12.5663706143591729538
#define INV_PI      0.31830988618379067154
#define INV_TWO_PI  0.15915494309189533577
#define INV_FOUR_PI 0.07957747154594766788
#define HALF_PI     1.57079632679489661923
#define INV_HALF_PI 0.63661977236758134308
#define LOG2_E      1.44269504088896340736
#define INV_SQRT2   0.70710678118654752440
#define PI_DIV_FOUR 0.78539816339744830961

#define TAU TWO_PI // added by momo

#define FLT_INF  asfloat(0x7F800000)
#define FLT_NAN  asfloat(0xffc00000)
#define FLT_EPS  5.960464478e-8  // 2^-24, machine epsilon: 1 + EPS = 1 (half of the ULP for 1.0f)
#define FLT_MIN  1.175494351e-38 // Minimum normalized positive floating-point number
#define FLT_MAX  3.402823466e+38 // Maximum representable floating-point number
#define HALF_EPS 4.8828125e-4    // 2^-11, machine epsilon: 1 + EPS = 1 (half of the ULP for 1.0f)
#define HALF_MIN 6.103515625e-5  // 2^-14, the same value for 10, 11 and 16-bit: https://www.khronos.org/opengl/wiki/Small_Float_Formats
#define HALF_MIN_SQRT 0.0078125  // 2^-7 == sqrt(HALF_MIN), useful for ensuring HALF_MIN after x^2
#define HALF_MAX 65504.0
#define UINT_MAX 0xFFFFFFFFu
#define INT_MAX  0x7FFFFFFF

#define TEMPLATE_3_FLT_HALF(FunctionName, Parameter1, Parameter2, Parameter3, FunctionBody) \
    min16float  FunctionName(min16float  Parameter1, min16float  Parameter2, min16float  Parameter3) { FunctionBody; } \
    min16float2 FunctionName(min16float2 Parameter1, min16float2 Parameter2, min16float2 Parameter3) { FunctionBody; } \
    min16float3 FunctionName(min16float3 Parameter1, min16float3 Parameter2, min16float3 Parameter3) { FunctionBody; } \
    min16float4 FunctionName(min16float4 Parameter1, min16float4 Parameter2, min16float4 Parameter3) { FunctionBody; } \
    float  FunctionName(float  Parameter1, float  Parameter2, float  Parameter3) { FunctionBody; } \
    float2 FunctionName(float2 Parameter1, float2 Parameter2, float2 Parameter3) { FunctionBody; } \
    float3 FunctionName(float3 Parameter1, float3 Parameter2, float3 Parameter3) { FunctionBody; } \
    float4 FunctionName(float4 Parameter1, float4 Parameter2, float4 Parameter3) { FunctionBody; }


// [start, end] -> [0, 1] : (x - start) / (end - start) = x * rcpLength - (start * rcpLength)
TEMPLATE_3_FLT_HALF(Remap01, x, rcpLength, startTimesRcpLength, return saturate(x * rcpLength - startTimesRcpLength))

// [start, end] -> [1, 0] : (end - x) / (end - start) = (end * rcpLength) - x * rcpLength
TEMPLATE_3_FLT_HALF(Remap10, x, rcpLength, endTimesRcpLength, return saturate(endTimesRcpLength - x * rcpLength))

float DegToRad(const float deg)
{
    return deg * (PI / 180.0);
}

float RadToDeg(const float rad)
{
    return rad * (180.0 / PI);
}

bool IsPower2(const uint x)
{
    return (x & (x - 1)) == 0;
}

// smoothstep that assumes that 'x' lies within the [0, 1] interval.
float Smoothstep01(const float x)
{
    return x * x * (3 - (2 * x));
}

float Smootherstep01(const float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float Smootherstep(const float a, const float b, const float t)
{
    float r = rcp(b - a);
    float x = Remap01(t, r, a * r);
    return Smootherstep01(x);
}

float3 NLerp(const float3 A, const float3 B, const float t)
{
    return normalize(lerp(A, B, t));
}

float Length2(const float3 v)
{
    return dot(v, v);
}

// this might be the source, i don't really remember : https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
float4x4 Inverse(const float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

// Input [0, 1] and output [0, PI/2]
// 9 VALU
float FastACosPos(const float inX)
{
    float x = abs(inX);
    float res = (0.0468878 * x + -0.203471) * x + 1.570796; // p(x)
    res *= sqrt(1.0 - x);

    return res;
}

// Ref: https://seblagarde.wordpress.com/2014/12/01/inverse-trigonometric-functions-gpu-optimization-for-amd-gcn-architecture/
// Input [-1, 1] and output [0, PI]
// 12 VALU
float FastACos(const float inX)
{
    float res = FastACosPos(inX);

    return (inX >= 0) ? res : PI - res; // Undo range reduction
}

// Same cost as Acos + 1 FR
// Same error
// input [-1, 1] and output [-PI/2, PI/2]
float FastASin(const float x)
{
    return HALF_PI - FastACos(x);
}

// max absolute error 1.3x10^-3
// Eberly's odd polynomial degree 5 - respect bounds
// 4 VGPR, 14 FR (10 FR, 1 QR), 2 scalar
// input [0, infinity] and output [0, PI/2]
float FastATanPos(const float x)
{
    float t0 = (x < 1.0) ? x : 1.0 / x;
    float t1 = t0 * t0;
    float poly = 0.0872929;
    poly = -0.301895 + poly * t1;
    poly = 1.0 + poly * t1;
    poly = poly * t0;
    return (x < 1.0) ? poly : HALF_PI - poly;
}

// 4 VGPR, 16 FR (12 FR, 1 QR), 2 scalar
// input [-infinity, infinity] and output [-PI/2, PI/2]
float FastATan(const float x)
{
    float t0 = FastATanPos(abs(x));
    return (x < 0.0) ? -t0 : t0;
}

float FastAtan2(const float y, const float x)
{
    return FastATan(y / x) + float(y >= 0.0 ? PI : -PI) * (x < 0.0);
}

// Normalize that account for vectors with zero length
float3 SafeNormalize(const float3 inVec)
{
    float dp3 = max(FLT_MIN, dot(inVec, inVec));
    return inVec * rsqrt(dp3);
}

half3 SafeNormalize(const half3 inVec)
{
    half dp3 = max(HALF_MIN, dot(inVec, inVec));
    return inVec * rsqrt(dp3);
}

bool IsNormalized(const float3 inVec)
{
    float squaredLength = dot(inVec, inVec);
    return 0.9998 < squaredLength && squaredLength < 1.0002001;
}

bool IsNormalized(const half3 inVec)
{
    half squaredLength = dot(inVec, inVec);
    return 0.998 < squaredLength && squaredLength < 1.002;
}

////////////// my stuff
float GetWave(float2 uv, float amplitude, float time)
{
    float2 uvCenter = uv * 2 - 1;
    float radius = length(uvCenter);
    float wave = cos((radius - time * 0.1) * TAU * 5) * 0.5 + 0.5;
    wave *= 1 - radius;
    wave *= amplitude;
    return wave;
}

float3 expandNormal(float4 normalTexture)
{
    float3 normal = normalTexture.xyy;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

// unused, no idea where it's from.
//static const float GaussianKernel5[5] =
//{ 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp(x * (a * x + b) / (x * (c * x + d) + e), 0.0, 1.0);
}


float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

float2 fibonacci_lattice(int i, int N)
{
    float PHI = 0.5 * (sqrt(5.) + 1.);
    return float2((float(i) + 0.5f) / float(N), fmod(float(i) / PHI, 1.));
}
float3 fibonacci_sphere(int i, int N)
{
    float2 xy = fibonacci_lattice(i, N);
    float2 pt = float2(2.f * PI * xy.y, acos(2.f * xy.x - 1.f) - PI * 0.5f);
    return float3(cos(pt.x) * cos(pt.y), sin(pt.x) * cos(pt.y), sin(pt.y));
}

static const float2 resolution = float2(1264, 681); // this assumes the res is 1280/720, and is the size of the backbuffer.

// Hash function for randomness
float random(float seed)
{
    return frac(sin(seed * 78.233) * 43758.5453);
}

float3 getRandom(float seed)
{
    float r = frac(sin(seed * 12.9898) * 43758.5453);
    float g = frac(sin(seed * 78.233) * 43758.5453);
    float b = frac(sin(seed * 45.164) * 43758.5453);
    return float3(r, g, b);
}

// lerps:

float EaseInSine(float x)
{
    return 1 - cos(x * HALF_PI);
}

float EaseOutSine(float x)
{
    return sin(x * HALF_PI);
}

float EaseInOutSine(float x)
{
    return -(cos(PI * x) - 1) / 2;
}

float EaseInCubic(float x)
{
    return x * x * x;
}

float EaseOutCubic(float x)
{
    return 1 - pow(1 - x, 3);
}

float EaseInOutCubic(float x)
{
    return x < 0.5 ? 4 * x * x * x : 1 - (pow((-2 * x) + 2, 3) / 2);
}

float EaseInQuint(float x)
{
    return x * x * x * x * x;
}

float EaseOutQuint(float x)
{
    return 1 - pow(1 - x, 5);
}

float EaseInOutQuint(float x)
{
    return x < 0.5 ? 16 * x * x * x * x * x : 1 - (pow((-2 * x) + 2, 5) / 2);
}

float EaseInCirc(float x)
{
    return 1 - sqrt(1 - pow(x, 2));
}

float EaseOutCirc(float x)
{
    return sqrt(1 - pow(x - 1, 2));
}

float EaseInOutCirc(float x)
{
    return x < 0.5 ? (1 - sqrt(1 - pow(2 * x, 2))) / 2 : (sqrt(1 - pow((-2 * x) + 2, 2)) + 1) / 2;
}

float EaseInElastic(float x)
{
    const float c4 = TAU / 3.0;

    return x == 0 ? 0 : x == 1 ? 1 : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
}

float EaseOutElastic(float x)
{
    const float c4 = TAU / 3.0;

    return x == 0 ? 0 : x == 1 ? 1 : pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
}

float EaseInOutElastic(float x)
{
    const float c5 = TAU / 4.5;

    return x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2 : (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
}

float EaseInQuad(float x)
{
    return x * x;
}

float EaseOutQuad(float x)
{
    return 1 - ((1 - x) * (1 - x));
}

float EaseInOutQuad(float x)
{
    return x < 0.5 ? 2 * x * x : 1 - (pow((-2 * x) + 2, 2) / 2);
}

float EaseInQuart(float x)
{
    return x * x * x * x;
}

float EaseOutQuart(float x)
{
    return 1 - pow(1 - x, 4);
}

float EaseInOutQuart(float x)
{
    return x < 0.5 ? 8 * x * x * x * x : 1 - (pow((-2 * x) + 2, 4) / 2);
}

float EaseInExpo(float x)
{
    return x == 0 ? 0 : pow(2, (10 * x) - 10);
}

float EaseOutExpo(float x)
{
    return x == 1 ? 1 : 1 - pow(2, -10 * x);
}

float EaseInOutExpo(float x)
{
    return x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? pow(2, 20 * x - 10) / 2 : (2 - pow(2, -20 * x + 10)) / 2;
}

float EaseInBack(float x)
{
    const float c1 = 1.70158;
    const float c3 = c1 + 1;

    return c3 * x * x * x - c1 * x * x;
}

float EaseOutBack(float x)
{
    const float c1 = 1.70158;
    const float c3 = c1 + 1;

    return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

float EaseInOutBack(float x)
{
    const float c1 = 1.70158;
    const float c2 = c1 * 1.525;

    return x < 0.5 ? pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2) / 2 : (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

float EaseOutBounce(float x)
{
    const float n1 = 7.5625;
    const float d1 = 2.75;

    if (x < 1 / d1)
    {
        return n1 * x * x;
    }
    else if (x < 2 / d1)
    {
        x -= 1.5 / d1;
        return n1 * x * x + 0.75;
    }
    else if (x < 2.5 / d1)
    {
        x -= 2.25 / d1;
        return n1 * x * x + 0.9375;
    }
    else
    {
        x -= 2.625 / d1;
        return n1 * x * x + 0.984375;
    }
}

float EaseInBounce(float x)
{
    return 1 - EaseOutBounce(1 - x);
}

float EaseInOutBounce(float x)
{
    return x < 0.5 ? (1 - EaseOutBounce(1 - (2 * x))) / 2 : (1 + EaseOutBounce((2 * x) - 1)) / 2;
}

float MathfInverseLerp(float x)
{
    return clamp((x - 0) / (1 - 0), 0.0, 1.0);
}

float EaseFlatten(float t)
{
    return 1 - exp(-t); // Inverse exponential function
}

float EaseFlatten(float t, float steepness)
{
    return t / (steepness + t); // k controls the steepness of the curve
}

float EaseFlatten2(float t)
{
    return t / (1 + t); // Flattens out as t increases
}
float EaseLogarithmic(float t)
{
    return log(1 + t); // Logarithmic-like function
}

float EaseLogarithmicNormalized(float t)
{
    return log(1 + t) / log(2); // Normalized to approach 1 as t approaches 1
}

bool int3Equals(int3 one, int3 two)
{
    return (one.x == two.x && one.y == two.y && one.z == two.z);
}

bool uint3Equals(uint3 one, uint3 two)
{
    return (one.x == two.x && one.y == two.y && one.z == two.z);
}

float DotClamped(float3 a, float3 b)
{
    return clamp(dot(a, b), 0.0, 1.0);
}