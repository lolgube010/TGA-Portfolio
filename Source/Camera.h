#pragma once
#include "CommonUtilities/Matrix.hpp"
#include "CommonUtilities/UtilityFunctions.h"
#include "CommonUtilities/Vector.h"

namespace CommonUtilities
{
	class InputManager;
}

class Camera
{
public:
	// TODO- deprecate or actually clean up THESE functions below
	[[nodiscard]] CommonUtilities::Vector3f WorldToPostProjection(const CommonUtilities::Vector3f& aPoint) const; // clip space is the same as post projection
	[[nodiscard]] CommonUtilities::Vector3f LocalToWorld(const CommonUtilities::Vector3f& aPoint) const; // also known as model to world (but just for our cam)
	[[nodiscard]] CommonUtilities::Vector3f WorldToView(const CommonUtilities::Vector3f& aPoint) const; // AKA camera-space
	[[nodiscard]] CommonUtilities::Vector3f ViewToProjection(CommonUtilities::Vector4f aViewSpacePoint) const; // clip aka projection
	[[nodiscard]] CommonUtilities::Vector3f ClipToRaster(const CommonUtilities::Vector3f& aProjectionSpacePoint) const; // clip to screen space

#define WorldToClipSpace WorldToPostProjection
#define ModelToWorld LocalToWorld
#define WorldToCameraSpace WorldToView
#define CameraSpaceToProjectionSpace ViewToProjection
#define ClipToScreenSpace ClipToRaster


	// real class starts from here
	bool Init(const CommonUtilities::Vector3f& aPos, int aWidth, int aHeight, float aHorizontalFOV = 90, float aVerticalFOV = 90, float aNearPlane = 0.1f, float aFarPlane = 100.f);
	void Update(float aTimeDelta, CommonUtilities::InputManager* aInput);

	[[nodiscard]] CommonUtilities::Matrix4x4f GetWorldToClipMatrix() const; // interchangable to worldToPostProjection pretty much.
	[[nodiscard]] CommonUtilities::Matrix4x4f GetProjectionMatrix() const; // SEE CreateProjectionMatrix()
	[[nodiscard]] CommonUtilities::Matrix4x4f GetTransform() const;


	CommonUtilities::Vector3f GetCamForward();
	CommonUtilities::Vector3f GetCamUp();
	CommonUtilities::Vector3f GetCamRight();

	void SetPosition(const CommonUtilities::Vector3f& aPos);
	//void SetNewHorizontalFOV(float aFOV); // TODO
	//void SetNewVerticalFOV(float aFOV);
	//void SetNewFOV(float aFOV);

public:
	void CreateProjectionMatrix();

	CommonUtilities::Matrix4x4f myTransform; // AKA view marix
	CommonUtilities::Matrix4x4f myProjectionMatrix;

	float myHorizontalFOV = 0; // do not set these values here. they are set in camera.init.
	float myVerticalFOV = 0;

	float myNearPlane = 0; // default value is set in init
	float myFarPlane = 0;

	float myMoveSpeed = 10;

	float myLookSens = .4f;

	float myYawInRadians = 0;
	float myPitchInRadians = 0;

	float myMaxPitch = CommonUtilities::DegreesToRadConstExpr(90);
	float myMinPitch = CommonUtilities::DegreesToRadConstExpr(-90);

	CommonUtilities::Vector2f myWindowSize = {};
};