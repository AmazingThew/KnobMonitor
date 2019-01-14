#pragma once
#include "Utils/json.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

using nlohmann::json;

class KnobConfig
{
	static const int maxKnobs = 64;
public:
	KnobConfig();
	~KnobConfig();

	struct Knob
	{
		int index;
		glm::vec3 center;
	};

	struct Page
	{
		std::shared_ptr<std::vector<Knob>> knobs;
		glm::vec2 extents;
		glm::vec2 gaugeDiameter;
		float gaugeScale;
		float aspect;
		glm::vec3 aspectCorrection;
	};

	Page* currentPage();

	void incrementPage();
	void decrementPage();

	int currentPageIndex = 0;
	std::vector<Page> pages;
};