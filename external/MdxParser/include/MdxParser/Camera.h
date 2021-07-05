#pragma once

#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Camera
	{
	public:
		Camera();
		Camera(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		std::string getName() const;
		std::array<float, 3> getPosition() const;
		float getFieldOfView() const;
		float getFarClippingPlane() const;
		float getNearClippingPlane() const;
		std::array<float, 3> getTargetPosition() const;
		TracksChunk<std::array<float, 3>> getTranslation() const;
		TracksChunk<float> getRotation() const;
		TracksChunk<std::array<float, 3>> getTargetTranslation() const;

	protected:
		uint32_t inclusiveSize;
		std::string name;
		std::array<float, 3> position;
		float fieldOfView;
		float farClippingPlane;
		float nearClippingPlane;
		std::array<float, 3> targetPosition;
		TracksChunk<std::array<float, 3>> translation; // KCTR
		TracksChunk<float> rotation; // KTTR
		TracksChunk<std::array<float, 3>> targetTranslation; // KCRL
	};
}