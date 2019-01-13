#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include "Knobs.h"
#include "json.hpp"
#include <iostream>

using nlohmann::json;

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
		const int maxKnobs = 64;

		glm::vec2 knobGrid[maxKnobs];

		json j;
		std::ifstream i("config.json");
		i >> j;

		int y = 0;
		int numKnobs = 0;
		for (auto knobRow : j["Knobs"])
		{
			int x = 0;
			for (auto knobIndex : knobRow)
			{
				knobGrid[knobIndex.get<int>()] = glm::vec2(x, y);
				x++;
				numKnobs++;
			}
			y++;
		}

		std::shared_ptr<std::vector<glm::vec3>> centers(new std::vector<glm::vec3>());


		glm::vec2 extents = glm::vec2(0);
		for (int i=0; i<numKnobs; i++)
		{
			extents = glm::max(extents, knobGrid[i]);
		}

		float margin = 0.03f;
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

		for (int i = 0; i<numKnobs; i++)
		{
			glm::vec2 normalizedGridPosition = knobGrid[i] / glm::max(extents, glm::vec2(1));
			normalizedGridPosition = glm::vec2(normalizedGridPosition.x, 1 - normalizedGridPosition.y); // Hilarious Y flip
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