#include "Valute_Fault.h"
Valute_Fault::Valute_Fault()
{
	currentValue = 0.0;
	middleValue = 0.0;
	medianValue = 0.0;
	counter = 0;
	totalSum = 0.0;
}

void Valute_Fault::newValueInsert(float value)
{
	if (previousValues.size() == 0)
		previousValues.push_back(value);
	else
	{
		int left = 0;
		int right = previousValues.size() - 1;
		while (true)
		{
			
			int middle = (left + right) / 2;
			if (value < previousValues[middle])
				right = middle - 1;      
			else if (value > previousValues[middle])  
				left = middle + 1;    
			else
			{
				std::vector<float>::iterator iter = previousValues.begin();
				iter += middle;
				previousValues.insert(iter, 1, value);
				break;
			}

			if (left > right)
			{
				if (left >= previousValues.size())
					previousValues.push_back(value);
				else
				{
					std::vector<float>::iterator iter = previousValues.begin();
					iter += left;
					previousValues.insert(iter, 1, value);
				}
					

				break;
			}
		}
		

	}
	currentValue = value;
	counter++;
	totalSum += value;
}
float Valute_Fault::getMiddleValue()
{
	return totalSum / counter;
}
float Valute_Fault::getMedianValue()
{
	return previousValues[previousValues.size() / 2];
}