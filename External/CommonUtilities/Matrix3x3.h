#pragma once
#include "Vector3.h"
#include "array"

namespace CU = CommonUtilities;

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix. 
		Matrix3x3();
		~Matrix3x3() = default;

		// Copy Constructor.
		Matrix3x3(const Matrix3x3<T>& aMatrix);

		// Copies the top left 3x3 part of the Matrix4x4.
		// explicit forces the user to static cast from a 4x4 to a 3x3, without it being implicit.
		explicit Matrix3x3(const Matrix4x4<T>& aMatrix);

		// operators
		// + Adderar två matriser och returnerar resultatet
		Matrix3x3 operator+(const Matrix3x3& aMatrix) const;

		// += Adderar en matris till this
		Matrix3x3& operator+=(const Matrix3x3& aOther);

		// - Subtraherar en matris från den första och returnerar resultatet
		Matrix3x3 operator-(const Matrix3x3& aMatrix) const;

		// -= Subtraherar en matris från this
		Matrix3x3& operator-=(const Matrix3x3& aOther);

		// * Multiplicerar två matriser och returnar resultatet
		Matrix3x3 operator*(const Matrix3x3& aRightMatrix) const;
		Matrix3x3 operator*(const T& aScalar) const;

		// *= Multiplicerar en matris till this 
		Matrix3x3& operator*=(const Matrix3x3& aOther);
		Matrix3x3& operator*=(const T& aScalar);

		// = Tilldelning
		Matrix3x3& operator=(const Matrix3x3& aOther);

		// == Jämförelse
		bool operator==(const Matrix3x3& aMatrix) const;

		// inte med i föreläsningen
		bool operator!=(const Matrix3x3& aMatrix) const;

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(int aRow, int aColumn);
		const T& operator()(int aRow, int aColumn) const;

		// Static functions for creating rotation matrices.
		static Matrix3x3 CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3 CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3 CreateRotationAroundZ(T aAngleInRadians);

		// Static function for creating a transpose of a matrix.
		static Matrix3x3 Transpose(const Matrix3x3& aMatrixToTranspose);

	private:
		std::array<std::array<T, 3>, 3> myData;
		// // row, column if you do myData[row][column], 0 indexed, note that the () operator does -1 to account for this, so (1,1) is element (0,0)
		//constexpr size_t myLength = 9;
	};

	using Matrix3x3f = Matrix3x3<float>;

	template <class T>
	Matrix3x3<T>::Matrix3x3()
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				if (row == column)
				{
					myData[row][column] = static_cast<T>(1); // Diagonal elements set to 1
				}
				else
				{
					myData[row][column] = static_cast<T>(0); // Off-diagonal elements set to 0
				}
			}
		}
	}

	template <class T>
	Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				myData[row][column] = aMatrix.myData[row][column];
			}
		}
	}

	template <class T>
	Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				myData[row][column] = aMatrix(row + 1, column + 1);
			}
		}
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3& aMatrix) const
	{
		Matrix3x3 result{*this};
		return result += aMatrix;
	}

	template <class T>
	Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3& aOther)
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				myData[row][column] += aOther.myData[row][column];
			}
		}
		return *this;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3& aMatrix) const
	{
		Matrix3x3 result{*this};
		return result -= aMatrix;
	}

	template <class T>
	Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3& aOther)
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				myData[row][column] -= aOther.myData[row][column];
			}
		}
		return *this;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3& aRightMatrix) const
	{
		Matrix3x3 result;

		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				T sum = static_cast<T>(0);
				for (int i = 0; i < 3; ++i)
				{
					sum += myData[row][i] * aRightMatrix.myData[i][column];
				}
				result.myData[row][column] = sum;
			}
		}

		return result;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::operator*(const T& aScalar) const
	{
		Matrix3x3 result{*this};
		return result *= aScalar;
	}

	template <class T>
	Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3& aOther)
	{
		Matrix3x3 temp = *this;

		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				T sum = static_cast<T>(0);
				for (int i = 0; i < 3; ++i)
				{
					sum += temp.myData[row][i] * aOther.myData[i][column];
				}
				myData[row][column] = sum;
			}
		}
		return *this;
	}

	template <class T>
	Matrix3x3<T>& Matrix3x3<T>::operator*=(const T& aScalar)
	{
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				myData[row][column] *= aScalar;
			}
		}
		return *this;
	}

	template <class T>
	Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3& aOther)
	{
		if (this != &aOther) // self-assignment check
		{
			myData = aOther.myData;
		}
		return *this;
	}

	template <class T>
	bool Matrix3x3<T>::operator==(const Matrix3x3& aMatrix) const
	{
		return myData == aMatrix.myData;
	}

	template <class T>
	bool Matrix3x3<T>::operator!=(const Matrix3x3& aMatrix) const
	{
		return myData != aMatrix.myData;
	}

	template <class T>
	T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myData[aRow - 1][aColumn - 1];
	}

	template <class T>
	const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myData[aRow - 1][aColumn - 1];
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3 returnMatrix{};
		returnMatrix.myData[0][0] = 1;
		returnMatrix.myData[1][1] = cos(aAngleInRadians);
		returnMatrix.myData[1][2] = sin(aAngleInRadians);
		returnMatrix.myData[2][1] = -sin(aAngleInRadians);
		returnMatrix.myData[2][2] = cos(aAngleInRadians);

		return returnMatrix;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3 returnMatrix{};
		returnMatrix.myData[0][0] = cos(aAngleInRadians);
		returnMatrix.myData[0][2] = -sin(aAngleInRadians);
		returnMatrix.myData[1][1] = 1;
		returnMatrix.myData[2][0] = sin(aAngleInRadians);
		returnMatrix.myData[2][2] = cos(aAngleInRadians);

		return returnMatrix;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3 returnMatrix{};
		returnMatrix.myData[0][0] = cos(aAngleInRadians);
		returnMatrix.myData[0][1] = sin(aAngleInRadians);
		returnMatrix.myData[1][0] = -sin(aAngleInRadians);
		returnMatrix.myData[1][1] = cos(aAngleInRadians);
		returnMatrix.myData[2][2] = 1;

		return returnMatrix;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3& aMatrixToTranspose)
	{
		Matrix3x3 returnMatrix = aMatrixToTranspose;
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				returnMatrix.myData[row][column] = aMatrixToTranspose.myData[column][row];
			}
		}
		return returnMatrix;
	}

	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> result;

		result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1);
		result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2);
		result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3);

		// you could technically do this if the [] operator was defined for the vector class, as in 0 = first element, 1 = 2nd, 2 = 3rd.
		//for (int i = 0; i < 3; ++i) 
		//{
		//	result[i] = aVector.x * aMatrix(1, i + 1)
		//		+ aVector.y * aMatrix(2, i + 1)
		//		+ aVector.z * aMatrix(3, i + 1);
		//}

		return result;
	}
}
