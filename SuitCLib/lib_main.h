#pragma once
const char* version();
int call_suitc(wchar_t** ppcharInput, int inputSize,
	wchar_t** ppcharOutput, int* pCharOutputSize,
	wchar_t** ppcharOptimumDetails, int* pcharOptimumDetailsSize);
void test_suitc();

