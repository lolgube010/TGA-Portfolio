#include "Camera.h"
#include "CommonUtilities/UtilityFunctions.h"
#include "Input.h"

CommonUtilities::Vector3f Camera::WorldToPostProjection(const CommonUtilities::Vector3f& aPoint) const
{
	const auto worldToView = WorldToView(aPoint);
	const CommonUtilities::Vector4 viewSpace = { worldToView.x, worldToView.y, worldToView.z, 1.f };  // get to camera space

	return ClipToRaster(ViewToProjection(viewSpace));
}

CommonUtilities::Vector3f Camera::LocalToWorld(const CommonUtilities::Vector3f& aPoint) const
{
	CommonUtilities::Vector4 vector(aPoint.x, aPoint.y, aPoint.z, 1.f);
	vector = vector * myTransform;
	return { vector.x, vector.y, vector.z };
}

CommonUtilities::Vector3f Camera::WorldToView(const CommonUtilities::Vector3f& aPoint) const
{
	CommonUtilities::Vector4 vector(aPoint.x, aPoint.y, aPoint.z, 1.f);
	vector = vector * CommonUtilities::Matrix4x4f::GetFastInverse(myTransform);
	return { vector.x, vector.y, vector.z };
}

CommonUtilities::Vector3f Camera::ViewToProjection(CommonUtilities::Vector4<float> aViewSpacePoint) const
{
	aViewSpacePoint = aViewSpacePoint * myProjectionMatrix;
	if (aViewSpacePoint.w != 1.0f)
	{
		aViewSpacePoint.x /= aViewSpacePoint.w;
		aViewSpacePoint.y /= aViewSpacePoint.w;
		aViewSpacePoint.z /= aViewSpacePoint.w;
	}

	return { aViewSpacePoint.x, aViewSpacePoint.y, aViewSpacePoint.z };
}

CommonUtilities::Vector3f Camera::ClipToRaster(const CommonUtilities::Vector3f& aProjectionSpacePoint) const
{
	CommonUtilities::Vector3f rasterSpace;
	rasterSpace.x = aProjectionSpacePoint.x * (myWindowSize.x / 2);
	rasterSpace.y = aProjectionSpacePoint.y * (myWindowSize.y / 2);
	rasterSpace.x += myWindowSize.x / 2;
	rasterSpace.y += myWindowSize.y / 2;
	rasterSpace.z = aProjectionSpacePoint.z;

	return rasterSpace;
}


CommonUtilities::Matrix4x4f Camera::GetWorldToClipMatrix() const
{
	return CommonUtilities::Matrix4x4f::GetFastInverse(myTransform) * myProjectionMatrix;
}

CommonUtilities::Matrix4x4f Camera::GetProjectionMatrix() const // SEE CreateProjectionMatrix()
{
	return myProjectionMatrix; // SEE CreateProjectionMatrix()
}

CommonUtilities::Matrix4x4f Camera::GetTransform() const
{
	return myTransform;
}

void Camera::SetPosition(const CommonUtilities::Vector3f& aPos)
{
	CommonUtilities::Vector4f newPos = { aPos.x, aPos.y, aPos.z, 0.f };
	newPos = newPos * myTransform; // get into cam space
	myTransform(4, 1) += newPos.x;
	myTransform(4, 2) += newPos.y;
	myTransform(4, 3) += newPos.z;
}

bool Camera::Init(const CommonUtilities::Vector3f& aPos, const int aWidth, const int aHeight, const float aHorizontalFOV, const float aVerticalFOV, const float aNearPlane, const float aFarPlane)
{
	myTransform(4, 1) = aPos.x;
	myTransform(4, 2) = aPos.y;
	myTransform(4, 3) = aPos.z;

	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;

	myWindowSize = { static_cast<float>(aWidth), static_cast<float>(aHeight) };
	myHorizontalFOV = aHorizontalFOV;
	myVerticalFOV = aVerticalFOV;

	CreateProjectionMatrix();
	return true;
}

void Camera::Update(const float aTimeDelta, CommonUtilities::InputManager* aInput)
{
	//CreateProjectionMatrix();

	if (aInput->GetIsKeyDown(KeyCode::Ctrl))
	{
		aInput->ToggleMouseLock();
	}
	if (aInput->GetIsKeyDown(KeyCode::Alt) && aInput->GetIsKeyDown(KeyCode::Tab) && aInput->GetIsMouseLockedToCenter())
	{
		aInput->ReleaseMouseFromCenter();
	}
	if ((aInput->GetMouseDelta().x != 0 || aInput->GetMouseDelta().y != 0) && (aInput->GetIsKeyHeld(KeyCode::RightMouseButton) || aInput->GetIsMouseLockedToCenter()))
	{
		// yaw globally
		// pitch locally

		myYawInRadians -= myLookSens * 0.01f * aInput->GetMouseDelta().x;
		myPitchInRadians += myLookSens * 0.01f * -aInput->GetMouseDelta().y;

		// clamping within limits.
		myPitchInRadians = CommonUtilities::Clamp(myPitchInRadians, myMinPitch, myMaxPitch);

		const auto yawRot = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(myYawInRadians);
		const auto pitchRot = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(myPitchInRadians);

		myTransform.ApplyRotation(pitchRot * yawRot);
	}

	if (aInput->GetIsKeyDown(KeyCode::R))
	{
		myTransform.ResetRotation();
		myYawInRadians = 0;
		myPitchInRadians = 0;
	}
	if (aInput->GetIsKeyHeld(KeyCode::W))
	{
		const CommonUtilities::Vector3f forwardVec = { 0.f, 0.f, myMoveSpeed * aTimeDelta };

		SetPosition(forwardVec);
	}
	if (aInput->GetIsKeyHeld(KeyCode::A))
	{
		const CommonUtilities::Vector3f leftVec = { -myMoveSpeed * aTimeDelta, 0.f, 0.f };
		SetPosition(leftVec);
	}
	if (aInput->GetIsKeyHeld(KeyCode::S))
	{
		const CommonUtilities::Vector3f backVec = { 0.f, 0.f, -myMoveSpeed * aTimeDelta };
		SetPosition(backVec);
	}
	if (aInput->GetIsKeyHeld(KeyCode::D))
	{
		const CommonUtilities::Vector3f rightVec = { myMoveSpeed * aTimeDelta, 0.f, 0.f };
		SetPosition(rightVec);
	}
}



CommonUtilities::Vector3f Camera::GetCamForward()
{
	return myTransform.GetForward();
}

CommonUtilities::Vector3f Camera::GetCamUp()
{
	return myTransform.GetUp();
}

CommonUtilities::Vector3f Camera::GetCamRight()
{
	return myTransform.GetRight();
}

void Camera::CreateProjectionMatrix()
{
	//const float radHorizontalFov = CommonUtilities::DegreesToRad(myHorizontalFOV);
	//const float radVerticalFov = CommonUtilities::DegreesToRad(myVerticalFOV);

	const float radHorizontalFov = myHorizontalFOV;
	const float radVerticalFov = myVerticalFOV;

	//const float radHorizontalFov = 60;
	//const float radVerticalFov = 60;

	float Q = myFarPlane / (myFarPlane - myNearPlane); // what the fuck does Q mean

	myProjectionMatrix(1, 1) = 1 / tanf(radHorizontalFov / 2);
	myProjectionMatrix(2, 2) = myWindowSize.x / myWindowSize.y * (1 / tanf(radVerticalFov / 2));
	myProjectionMatrix(3, 3) = Q;
	myProjectionMatrix(3, 4) = 1.0f;
	myProjectionMatrix(4, 3) = -myNearPlane * Q; // this looks fucked but is the same as the pharentasis.
	myProjectionMatrix(4, 4) = 0;
}