#include "Config.h"
#include <fstream>
#include <iostream>

KnobConfig::KnobConfig()
{
	json j;
	std::ifstream i("config.json");
	i >> j;

	for (auto page : j["KnobPages"])
	{
		std::shared_ptr<std::vector<Knob>> knobs(new std::vector<Knob>());

		std::vector<glm::vec2> knobGridCoords;
		std::vector<int> knobIndices;
		std::vector<glm::vec3> knobCenters;


		int y = 0;
		int knobsOnPage = 0;
		for (auto knobRow : page)
		{
			int x = 0;
			for (auto knobIndex : knobRow)
			{
				knobGridCoords.push_back(glm::vec2(x, y));
				knobIndices.push_back(knobIndex.get<int>());
				x++;
				knobsOnPage++;
			}
			y++;
		}

		glm::vec2 extents = glm::vec2(0);
		for (glm::vec2 gridCoord : knobGridCoords)
		{
			extents = glm::max(extents, gridCoord);
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
		}
		else
		{
			gaugeScale = gaugeDiameter.y;
			aspect = dimensions.x / dimensions.y;
			aspectCorrection = glm::vec3(1 / aspect, 1, 1);
		}

		for (glm::vec2 gridCoord : knobGridCoords)
		{
			glm::vec2 normalizedGridPosition = gridCoord / glm::max(extents, glm::vec2(1));
			normalizedGridPosition = glm::vec2(normalizedGridPosition.x, 1 - normalizedGridPosition.y); // Hilarious Y flip
			knobCenters.push_back(glm::vec3(glm::mix(minCenter, maxCenter, normalizedGridPosition) * 2.0f - 1.0f, 0) / aspectCorrection);
		}

		for (int i = 0; i < knobsOnPage; ++i)
		{
			knobs->push_back(Knob {
				knobIndices[i],
				knobCenters[i]
			});
		}

		pages.push_back(Page {
			knobs,
			extents,
			gaugeDiameter,
			gaugeScale,
			aspect,
			aspectCorrection,
		});
	}
}

KnobConfig::~KnobConfig()
{
}

KnobConfig::Page* KnobConfig::currentPage()
{
	return &pages.at(currentPageIndex);
}

void KnobConfig::incrementPage()
{
	int modulus = (int) pages.size();
	currentPageIndex = (currentPageIndex + 1) % modulus;
}

void KnobConfig::decrementPage()
{
	float a = (float) (currentPageIndex - 1);
	float n = (float) pages.size();
	currentPageIndex = (int) (a - n * glm::floor(a / n)); //Knuth modulus, correctly handles negative divident
}
