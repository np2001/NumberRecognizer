#include "SNNumberRecognizerDefines.h"

bool SNNumberRecognizer::SortANNPredictionResults(const SNANNLinePredictionResults& i, const SNANNLinePredictionResults& j)
{
	return (i.WeightSum > j.WeightSum);
}
