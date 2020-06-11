//
// Created by 6rayWa1cher on 11.06.2020.
//

#ifndef PROJECT_MICRIC2_GLOBALPARAMETERS_H
#define PROJECT_MICRIC2_GLOBALPARAMETERS_H

class GlobalParameters {
private:
	GlobalParameters() = default;

public:
	bool enableOperatorFormatter = false;

	static GlobalParameters& getInstance();
};

#endif //PROJECT_MICRIC2_GLOBALPARAMETERS_H
