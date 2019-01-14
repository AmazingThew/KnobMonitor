#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include "Knobs.h"
#include "json.hpp"
#include <iostream>

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