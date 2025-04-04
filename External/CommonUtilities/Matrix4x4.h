#pragma once
#include <array>
#include "Vector4.h"
#include "Vector3.h"

namespace CommonUtilities
{
	template <class T>
	class Matrix3x3;

	template <class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4(); // default constructor sets to identity
		~Matrix4x4() = default;

		// Copy Constructor.
		Matrix4x4(const Matrix4x4& aMatrix);

		// Static functions for creating rotation matrices.
		static Matrix4x4 CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4 CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4 CreateRotationAroundZ(T aAngleInRadians);

		static Matrix4x4 CreateScaleMatrix(Vector3<T> aScale);
		static Matrix4x4 CreateScaleMatrix(T aScale);

		static Matrix4x4 CreateTranslationMatrix(Vector3<T> aPosition);

		// Assumes aTransform is made up of nothing but rotations and translations.
		static Matrix4x4 GetFastInverse(const Matrix4x4<T>& aTransform);

		// Static function for creating a transpose of a matrix.
		static Matrix4x4 Transpose(const Matrix4x4& aMatrixToTranspose);

		Vector3<T> GetForward(); // stolen from tge
		Vector3<T> GetUp(); // stolen from tge
		Vector3<T> GetRight(); // stolen from tge

		Vector3<T> GetPosition();
		Vector4<T> GetPositionVec4();

		void ResetRotation();
		void SetToIdentity();
		static Matrix4x4 GetIdentity();

		void SetPosition(Vector3<T> aPos);

		void Set3X3(const Matrix3x3<T>& aMatrix);
		void Set3X3(const Matrix4x4& aMatrix);

		void SetRotationX(Vector3<T> aRotX);
		void SetRotationY(Vector3<T> aRotY);
		void SetRotationZ(Vector3<T> aRotZ);

		// sets the top left 3x3 of another 4x4 matrix to this matrix's top left 3x3.
		void ApplyRotation(const Matrix4x4& aMatrix);
		void ApplyRotation(const Matrix3x3<T>& aMatrix);

		// operators
		// + Adderar två matriser och returnerar resultatet
		Matrix4x4 operator+(const Matrix4x4& aMatrix) const;

		// += Adderar en matris till this
		Matrix4x4& operator+=(const Matrix4x4& aOther);

		// - Subtraherar en matris från den första och returnerar resultatet
		Matrix4x4 operator-(const Matrix4x4& aMatrix) const;

		// -= Subtraherar en matris från this
		Matrix4x4& operator-=(const Matrix4x4& aOther);

		// * Multiplicerar två matriser och returnar resultatet
		Matrix4x4 operator*(const Matrix4x4& aRightMatrix) const;
		Matrix4x4 operator*(const T& aScalar) const;

		// *= Multiplicerar en matris till this 
		Matrix4x4& operator*=(const Matrix4x4& aOther);
		Matrix4x4& operator*=(const T& aScalar);

		// = Tilldelning
		Matrix4x4& operator=(const Matrix4x4& aOther);

		// == Jämförelse
		bool operator==(const Matrix4x4& aMatrix) const;

		// inte med i föreläsningen
		bool operator!=(const Matrix4x4& aMatrix) const;

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(int aRow, int aColumn);
		const T& operator()(int aRow, int aColumn) const;

	private:
		std::array<std::array<T, 4>, 4> myData; // row, column if you do myData[row][column], 0 indexed
		//static const size_t myLength = 16;
	};

	using Matrix4x4f = Matrix4x4<float>;

	template <class T>
	Matrix4x4<T>::Matrix4x4()
	{
		SetToIdentity();
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4& aMatrix)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				myData[row][column] = aMatrix.myData[row][column];
			}
		}
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4 returnMatrix{};
		returnMatrix.myData[0][0] = static_cast<T>(1);
		returnMatrix.myData[1][1] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[1][2] = static_cast<T>(-sin(aAngleInRadians));
		returnMatrix.myData[2][1] = static_cast<T>(sin(aAngleInRadians));
		returnMatrix.myData[2][2] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[3][3] = static_cast<T>(1);

		return returnMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4 returnMatrix{};
		returnMatrix.myData[0][0] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[0][2] = static_cast<T>(sin(aAngleInRadians));
		returnMatrix.myData[1][1] = static_cast<T>(1);
		returnMatrix.myData[2][0] = static_cast<T>(-sin(aAngleInRadians));
		returnMatrix.myData[2][2] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[3][3] = static_cast<T>(1);

		return returnMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4 returnMatrix{};
		returnMatrix.myData[0][0] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[0][1] = static_cast<T>(-sin(aAngleInRadians));
		returnMatrix.myData[1][0] = static_cast<T>(sin(aAngleInRadians));
		returnMatrix.myData[1][1] = static_cast<T>(cos(aAngleInRadians));
		returnMatrix.myData[2][2] = static_cast<T>(1);
		returnMatrix.myData[3][3] = static_cast<T>(1);

		return returnMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(const Vector3<T> aScale)
	{
		Matrix4x4 scaleMatrix;
		scaleMatrix(1, 1) = aScale.x;
		scaleMatrix(2, 2) = aScale.y;
		scaleMatrix(3, 3) = aScale.z;
		scaleMatrix(4, 4) = 1;
		return scaleMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(T aScale)
	{
		return CreateScaleMatrix({aScale, aScale, aScale});
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(Vector3<T> aPosition)
	{
		Matrix4x4 toReturn;
		toReturn(4, 1) = aPosition.x;
		toReturn(4, 2) = aPosition.y;
		toReturn(4, 3) = aPosition.z;
		return toReturn;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4 toReturn(aTransform);

		// transponera 3x3 delen av vår 4x4 matris.
		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				toReturn.myData[row][column] = aTransform.myData[column][row];
			}
		}
		// multiplicera negativa t med 3x3 matrisen och assigna det till vår return
		T tX = -toReturn(4, 1);
		T tY = -toReturn(4, 2);
		T tZ = -toReturn(4, 3);
		toReturn(4, 1) = tX * toReturn(1, 1) + tY * toReturn(2, 1) + tZ * toReturn(3, 1);
		toReturn(4, 2) = tX * toReturn(1, 2) + tY * toReturn(2, 2) + tZ * toReturn(3, 2);
		toReturn(4, 3) = tX * toReturn(1, 3) + tY * toReturn(2, 3) + tZ * toReturn(3, 3);

		return toReturn;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4& aMatrixToTranspose)
	{
		Matrix4x4 returnMatrix = aMatrixToTranspose;
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				returnMatrix.myData[row][column] = aMatrixToTranspose.myData[column][row];
			}
		}
		return returnMatrix;
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::GetForward()
	{
		return { myData[2][0], myData[2][1], myData[2][2] };
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::GetUp()
	{
		return { myData[1][0], myData[1][1], myData[1][2] };
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::GetRight()
	{
		return { myData[0][0], myData[0][1], myData[0][2] };
	}

	template <class T>
	Vector4<T> Matrix4x4<T>::GetPositionVec4()
	{
		return { myData[3][0], myData[3][1], myData[3][2], myData[3][3] };
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::GetPosition()
	{
		return { myData[3][0], myData[3][1], myData[3][2] };
	}

	template <class T>
	void Matrix4x4<T>::ResetRotation()
	{
		// sets the top left of our matrix to the default (except pos)
		/// [1, 0, 0, 0]
		/// [0, 1, 0, 0]
		/// [0, 0, 1, 0]
		/// [x, y, z, 1]

		myData.at(0).at(0) = 1;
		myData.at(0).at(1) = 0;
		myData.at(0).at(2) = 0;

		myData.at(1).at(0) = 0;
		myData.at(1).at(1) = 1;
		myData.at(1).at(2) = 0;

		myData.at(2).at(0) = 0;
		myData.at(2).at(1) = 0;
		myData.at(2).at(2) = 1;

		myData.at(3).at(3) = 1;
	}

	template <class T>
	void Matrix4x4<T>::SetToIdentity()
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
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
	Matrix4x4<T> Matrix4x4<T>::GetIdentity()
	{
		Matrix4x4 identity;
		return identity;
	}

	template <class T>
	void Matrix4x4<T>::SetPosition(Vector3<T> aPos)
	{
		myData[3][0] = aPos.x;
		myData[3][1] = aPos.y;
		myData[3][2] = aPos.z;
	}

	template <class T>
	void Matrix4x4<T>::Set3X3(const Matrix3x3<T>& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] = aMatrix(i + 1, j + 1);
			}
		}
	}

	template <class T>
	void Matrix4x4<T>::Set3X3(const Matrix4x4& aMatrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				myData[i][j] = aMatrix(i + 1, j + 1);
			}
		}
	}

	template <class T>
	void Matrix4x4<T>::SetRotationX(Vector3<T> aRotX)
	{
		myData[0][0] = aRotX.x;
		myData[0][1] = aRotX.y;
		myData[0][2] = aRotX.z;
	}

	template <class T>
	void Matrix4x4<T>::SetRotationY(Vector3<T> aRotY)
	{
		myData[1][0] = aRotY.x;
		myData[1][1] = aRotY.y;
		myData[1][2] = aRotY.z;
	}

	template <class T>
	void Matrix4x4<T>::SetRotationZ(Vector3<T> aRotZ)
	{
		myData[2][0] = aRotZ.x;
		myData[2][1] = aRotZ.y;
		myData[2][2] = aRotZ.z;
	}

	template <class T>
	void Matrix4x4<T>::ApplyRotation(const Matrix4x4& aMatrix)
		// applies only to the top 3x3 grid (rotation) of this matrix.
	{
		Set3X3(aMatrix);
	}

	template <class T>
	void Matrix4x4<T>::ApplyRotation(const Matrix3x3<T>& aMatrix)
	{
		Set3X3(aMatrix);
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4& aMatrix) const
	{
		Matrix4x4 result{ *this };
		return result += aMatrix;
	}

	template <class T>
	Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> resultMatrix(aMatrix0);
		return resultMatrix += aMatrix1;
	}

	template <class T>
	Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4& aOther)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				myData[row][column] += aOther.myData[row][column];
			}
		}
		return *this;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4& aMatrix) const
	{
		Matrix4x4 result{ *this };
		return result -= aMatrix;
	}

	template <class T>
	Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> matrix2(aMatrix0);
		return matrix2 -= aMatrix1;
	}

	template <class T>
	Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4& aOther)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				myData[row][column] -= aOther.myData[row][column];
			}
		}
		return *this;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4& aRightMatrix) const
	{
		// pick one of these two
		Matrix4x4 matrix2(*this);
		return matrix2 *= aRightMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const T& aScalar) const
	{
		Matrix4x4 result{ *this };
		return result *= aScalar;
	}

	template <class T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> result;

		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				T sum = static_cast<T>(0);
				for (int i = 0; i < 4; ++i)
				{
					sum += aMatrix0.myData[row][i] * aMatrix1.myData[i][column];
				}
				result.myData[row][column] = sum;
			}
		}

		return result;
	}

	template <class T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const T& aScalar)
	{
		Matrix4x4<T> result = aMatrix0;
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				result.myData[row][column] *= aScalar;
			}
		}
		return result;
	}

	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> result;

		result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w *
			aMatrix(4, 1);
		result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w *
			aMatrix(4, 2);
		result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w *
			aMatrix(4, 3);
		result.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w *
			aMatrix(4, 4);

		return result;
	}

	template <class T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4& aOther)
	{
		Matrix4x4 temp = *this; // Create a copy of the current matrix

		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				T sum = static_cast<T>(0);
				for (int i = 0; i < 4; ++i)
				{
					sum += temp.myData[row][i] * aOther.myData[i][column];
				}
				myData[row][column] = sum;
			}
		}
		return *this;
	}

	template <class T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(const T& aScalar)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				myData[row][column] *= aScalar;
			}
		}
		return *this;
	}

	template <class T>
	Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4& aOther)
	{
		if (this != &aOther)
			// self-assignment check. to avoid if you ex do myMatrix = myMatrix (why?). maybe throw an error?
		{
			myData = aOther.myData;
		}
		return *this;
	}

	template <class T>
	bool Matrix4x4<T>::operator==(const Matrix4x4& aMatrix) const
	{
		return myData == aMatrix.myData;
	}

	template <class T>
	bool Matrix4x4<T>::operator!=(const Matrix4x4& aMatrix) const
	{
		return myData != aMatrix.myData;
	}

	template <class T>
	T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
		// offset by one so that matrix(1,1) returns the element matrix[0][0] (first element)
	{
		return myData[aRow - 1][aColumn - 1];
	}

	template <class T>
	const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		return myData[aRow - 1][aColumn - 1];
	}
}

namespace CU = CommonUtilities;
