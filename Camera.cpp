#include "Camera.h"

Camera::Camera()
{
	eye = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	pitch = yaw = 0.0f;

	RecalculateViewMatrix();

	viewportWidth = 1280.0f;
	viewportHeight = 720.0f;
	fov = XM_PI / 3.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;
	RecalculateProjectionMatrix();
}

Camera::~Camera()
{
}

void Camera::SetEye(XMVECTOR eye)
{
	this->eye = eye;
	RecalculateViewMatrix();
}

void Camera::MoveEye(XMVECTOR direction)
{
	SetEye(eye + direction);
}

void Camera::SetRotation(float pitch, float yaw)
{
	this->pitch = pitch;
	this->yaw = yaw;
	RecalculateViewMatrix();
}

void Camera::Rotate(float deltaPitch, float deltaYaw)
{
	SetRotation(pitch + deltaPitch, yaw + deltaYaw);
}

void Camera::SetViewParams(XMVECTOR eye, XMVECTOR up, float pitch, float yaw)
{
	this->eye = eye;
	this->up = up;
	this->pitch = pitch;
	this->yaw = yaw;
	RecalculateViewMatrix();
}

void Camera::SetProjectionParams(float viewportWidth, float viewportHeight, float fov, float nearPlane, float farPlane)
{
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	this->fov = fov;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	RecalculateProjectionMatrix();
}

void Camera::RecalculateViewMatrix()
{
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
	viewMatrix = XMMatrixLookToLH(eye, cameraRotationMatrix.r[2], up);
	//viewMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * XMMatrixTranslationFromVector(eye);
	//viewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMStoreFloat4x4(&viewMatrixF4X4, viewMatrix);
}

void Camera::RecalculateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fov, viewportWidth / viewportHeight, nearPlane, farPlane);
	XMStoreFloat4x4(&projectionMatrixF4X4, projectionMatrix);
}