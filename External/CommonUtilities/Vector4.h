#pragma once

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		//Creates a null-vector
		Vector4();

		//Creates a vector (aX, aY, aZ, aW)
		Vector4(const T& aX, const T& aY, const T& aZ, const T& aW);

		//Copy constructor (compiler generated)
		Vector4(const Vector4& aVector) = default;

		//Assignment operator (compiler generated)
		Vector4& operator=(const Vector4& aVector4) = default;

		//Destructor (compiler generated)
		~Vector4() = default;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this
		Vector4 GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector4& aVector) const;
	};

	using Vector4f = Vector4<float>;

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>((aVector0.x + aVector1.x), (aVector0.y + aVector1.y), (aVector0.z + aVector1.z),
		                  (aVector0.w + aVector1.w));
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>((aVector0.x - aVector1.x), (aVector0.y - aVector1.y), (aVector0.z - aVector1.z),
		                  (aVector0.w - aVector1.w));
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector) { return (aVector * aScalar); }

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar) { return (aVector * (1 / aScalar)); }

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1) { (aVector0 = aVector0 + aVector1); }

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1) { (aVector0 = aVector0 - aVector1); }

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar) { (aVector = aVector * aScalar); }

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar) { (aVector = aVector / aScalar); }

	// added later by momo
	template <typename T>
	bool operator==(const Vector4<T>& a, const Vector4<T>& b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	}

	template <typename T>
	bool operator!=(const Vector4<T>& a, const Vector4<T>& b) { return !(a == b); }


	template <class T>
	Vector4<T>::Vector4()
		: x(0), y(0), z(0), w(0)
	{
	}

	template <class T>
	Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
		: x(aX), y(aY), z(aZ), w(aW)
	{
	}

	template <class T>
	T Vector4<T>::LengthSqr() const
	{
		return T((x * x) + (y * y) + (z * z) + (w * w));
	}

	template <class T>
	T Vector4<T>::Length() const
	{
		return T(sqrt((x * x) + (y * y) + (z * z) + (w * w)));
	}

	template <class T>
	Vector4<T> Vector4<T>::GetNormalized() const
	{
		if (Length() == 0) { return Vector4<T>(); }
		return *this / Length();
	}

	template <class T>
	void Vector4<T>::Normalize()
	{
		if (Length() == 0) { return; }
		*this /= Length(); // divides each element of our vector by the magnitude. also assigns! important!
	}

	template <class T>
	T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return T(x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w);
	}
}

namespace CU = CommonUtilities;
