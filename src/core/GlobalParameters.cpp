//
// Created by 6rayWa1cher on 11.06.2020.
//
#include "../include/GlobalParameters.h"

GlobalParameters& GlobalParameters::getInstance() {
	static GlobalParameters globalParameters;
	return globalParameters;
}
