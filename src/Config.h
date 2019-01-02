#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Knobs.h"
#include <memory>

namespace KnobConfig
{
	struct Config
	{
		std::shared_ptr<std::vector<glm::vec3>> centers;
		glm::vec2 extents;
		glm::vec2 gaugeDiameter;
		float gaugeScale;
		float aspect;
		glm::vec3 aspectCorrection;
	};

	inline Config* getConfig()
	{
		std::vector<glm::vec2> knobGrid;
		// knobGrid.push_back(glm::vec2(3, 3));
		// knobGrid.push_back(glm::vec2(3, 2));
		// knobGrid.push_back(glm::vec2(3, 1));
		// knobGrid.push_back(glm::vec2(3, 0));
		// knobGrid.push_back(glm::vec2(2, 3));
		// knobGrid.push_back(glm::vec2(2, 2));
		// knobGrid.push_back(glm::vec2(2, 1));
		// knobGrid.push_back(glm::vec2(2, 0));
		// knobGrid.push_back(glm::vec2(1, 3));
		// knobGrid.push_back(glm::vec2(1, 2));
		// knobGrid.push_back(glm::vec2(1, 1));
		// knobGrid.push_back(glm::vec2(1, 0));
		// knobGrid.push_back(glm::vec2(0, 3));
		// knobGrid.push_back(glm::vec2(0, 2));
		// knobGrid.push_back(glm::vec2(0, 1));
		// knobGrid.push_back(glm::vec2(0, 0));
		//
		//
		knobGrid.push_back(glm::vec2(0, 0));
		knobGrid.push_back(glm::vec2(0, 1));
		knobGrid.push_back(glm::vec2(0, 2));
		knobGrid.push_back(glm::vec2(1, 2));

		// knobGrid.push_back(glm::vec2(0, 0));
		// knobGrid.push_back(glm::vec2(1, 0));
		// knobGrid.push_back(glm::vec2(2, 0));

		std::shared_ptr<std::vector<glm::vec3>> centers(new std::vector<glm::vec3>());


		glm::vec2 extents = glm::vec2(0);
		for (size_t i = 0; i < knobGrid.size(); i++)
		{
			extents = glm::max(extents, knobGrid[i]);
		}

		float margin = 0.0f;
		glm::vec2 minCorner = glm::vec2(margin);
		glm::vec2 maxCorner = glm::vec2(1 - margin);
		glm::vec2 gaugeDiameter = (maxCorner - minCorner) / (extents + 1.0f);
		glm::vec2 minCenter = minCorner + gaugeDiameter / 2.0f;
		glm::vec2 maxCenter = maxCorner - gaugeDiameter / 2.0f;

		float gaugeScale, aspect;
		glm::vec3 aspectCorrection;
		glm::vec2 dimensions = (extents + 1.0f) + margin * 2;
		if (extents.x > extents.y)
		{
			gaugeScale = gaugeDiameter.x;
			aspect = dimensions.y / dimensions.x;
			aspectCorrection = glm::vec3(1, 1 / aspect, 1);
		} else
		{
			gaugeScale = gaugeDiameter.y;
			aspect = dimensions.x / dimensions.y;
			aspectCorrection = glm::vec3(1 / aspect, 1, 1);
		}

		for (size_t i = 0; i < knobGrid.size(); i++)
		{
			glm::vec2 normalizedGridPosition = knobGrid[i] / glm::max(extents, glm::vec2(1));
			centers->push_back(glm::vec3(glm::mix(minCenter, maxCenter, normalizedGridPosition) * 2.0f - 1.0f, 0) / aspectCorrection);
		}

		return new Config{
			centers,
			extents,
			gaugeDiameter,
			gaugeScale,
			aspect,
			aspectCorrection,
		};
	}
}