#pragma once


namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		//Creates a null-vector
		Vector2();

		//Creates a vector (aX, aY)
		Vector2(const T& aX, const T& aY);

		//Copy constructor (compiler generated)
		Vector2(const Vector2& aVector) = default;

		//Assignment operator (compiler generated)
		Vector2& operator=(const Vector2& aVector2) = default;

		//Destructor (compiler generated)
		~Vector2() = default;

		//Returns the squared length of the vector
		[[nodiscard]] T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this
		[[nodiscard]] Vector2 GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector2& aVector) const;
	};

	// ReSharper disable once CppInconsistentNaming
	using Vector2f = Vector2<float>;

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector) { return aVector * aScalar; }

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar) { return aVector * (1 / aScalar); }

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1) { aVector0 = aVector0 + aVector1; }

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1) { aVector0 = aVector0 - aVector1; }

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar) { aVector = aVector * aScalar; }

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar) { aVector = aVector / aScalar; }

	// added later by momo
	template <typename T>
	bool operator==(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x && a.y == b.y; }

	template <typename T>
	bool operator!=(const Vector2<T>& a, const Vector2<T>& b) { return !(a == b); }

	template <class T>
	Vector2<T>::Vector2() : x(0), y(0)
	{
	}

	template <class T>
	Vector2<T>::Vector2(const T& aX, const T& aY) : x(aX), y(aY)
	{
	}

	template <class T>
	T Vector2<T>::LengthSqr() const
	{
		return T(x * x + y * y);
	}

	template <class T>
	T Vector2<T>::Length() const
	{
		return T(sqrt(x * x + y * y));
	}

	template <class T>
	Vector2<T> Vector2<T>::GetNormalized() const
	{
		if (Length() == 0) { return Vector2(); }
		return *this / Length();
	}

	template <class T>
	void Vector2<T>::Normalize()
	{
		if (Length() == 0) { return; }
		*this /= Length(); // divides each element of our vector by the magnitude. also assigns! important!
	}

	template <class T>
	T Vector2<T>::Dot(const Vector2& aVector) const
	{
		return T(x * aVector.x + y * aVector.y);
	}
}

namespace CU = CommonUtilities;
