#pragma once
#include <cmath>
namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		//Creates a null-vector
		Vector3();

		//Creates a vector (aX, aY, aZ)
		Vector3(const T& aX, const T& aY, const T& aZ);

		// constructs the vector with a specific value
		Vector3(const T& aValue);

		//Copy constructor (compiler generated)
		Vector3(const Vector3& aVector) = default;

		//Assignment operator (compiler generated)
		Vector3& operator=(const Vector3& aVector3) = default;

		//Destructor (compiler generated)
		~Vector3() = default;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this
		Vector3 GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector3& aVector) const;

		//Returns the cross product of this and aVector
		Vector3 Cross(const Vector3& aVector) const;
	};

	using Vector3f = Vector3<float>;

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector) { return Vector3<T>(aVector * aScalar); }

	// this isn't recursive thanks to the above definition

	//this feels illegal ugh
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>((aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z));
	} // this isn't recursive thanks to the above definition

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) { return aVector * (1 / aScalar); }

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1) { aVector0 = aVector0 + aVector1; }

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1) { aVector0 = aVector0 - aVector1; }

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar) { aVector = aVector * aScalar; }

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar) { aVector = aVector / aScalar; }

	// added later by momo
	template <typename T>
	bool operator==(const Vector3<T>& a, const Vector3<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

	template <typename T>
	bool operator!=(const Vector3<T>& a, const Vector3<T>& b) { return !(a == b); }

	template <class T>
	Vector3<T>::Vector3()
		: x(0), y(0), z(0)
	{
	}

	template <class T>
	Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
		: x(aX), y(aY), z(aZ)
	{
	}

	template <class T>
	Vector3<T>::Vector3(const T& aValue)
		:x(aValue), y(aValue), z(aValue)
	{
		
	}

	template <class T>
	T Vector3<T>::LengthSqr() const
	{
		return T(x * x + y * y + z * z);
	}

	template <class T>
	T Vector3<T>::Length() const
	{
		return T(sqrt(x * x + y * y + z * z));
	}

	template <class T>
	Vector3<T> Vector3<T>::GetNormalized() const
	{
		if (Length() == 0) { return Vector3(); }
		return *this / Length();
	}

	template <class T>
	void Vector3<T>::Normalize()
	{
		if (Length() == 0) { return; }
		*this /= Length(); // divides each element of our vector by the magnitude. also assigns! important!
	}

	template <class T>
	T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return T(x * aVector.x + y * aVector.y + z * aVector.z);
	}

	template <class T>
	Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>(y * aVector.z - z * aVector.y, z * aVector.x - x * aVector.z, x * aVector.y - y * aVector.x);
	}
}

namespace CU = CommonUtilities;
