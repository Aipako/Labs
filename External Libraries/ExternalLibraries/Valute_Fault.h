#pragma once
#include <vector>
class Valute_Fault
{
public:
	std::vector<float> previousValues;
	float currentValue;
	float middleValue;
	float medianValue;

	Valute_Fault();
	void newValueInsert(float value);
	float getMiddleValue();
	float getMedianValue();
private:
	double totalSum;
	long long counter;

	/*double AUD, AUD_Mid, AUD_Med;
	double AZN, AZN_Mid, AZN_Med;
	double GBP, GBP_Mid, GBP_Med;
	double AMD, AMD_Mid, AMD_Med;
	double BYN, BYN_Mid, BYN_Med;
	double BGN, BGN_Mid, BGN_Med;
	double BRL, BRL_Mid, BRL_Med;
	double HUF, HUF_Mid, HUF_Med;
	double HKD, HKD_Mid, HKD_Med;
	double DKK, DKK_Mid, DKK_Med;
	double USD, USD_Mid, USD_Med;
	double EUR, EUR_Mid, EUR_Med;
	double INR, INR_Mid, INR_Med;
	double KZT, KZT_Mid, KZT_Med;
	double CAD, CAD_Mid, CAD_Med;
	double KGS, KGS_Mid, KGS_Med;
	double CNY, CNY_Mid, CNY_Med;
	double MDL, MDL_Mid, MDL_Med;
	double NOK, NOK_Mid, NOK_Med;
	double PLN, PLN_Mid, PLN_Med;
	double RON, RON_Mid, RON_Med;
	double XDR, XDR_Mid, XDR_Med;
	double SGD, SGD_Mid, SGD_Med;
	double TJS, TJS_Mid, TJS_Med;
	double TRY, TRY_Mid, TRY_Med;
	double TMT, TMT_Mid, TMT_Med;
	double UZS, UZS_Mid, UZS_Med;*/
};

