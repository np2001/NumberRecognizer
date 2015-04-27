#ifndef SNMasterSymbolRecognizer_h__
#define SNMasterSymbolRecognizer_h__
//----------------------------------------------------------------
#include <utility>
#include <opencv2\core\core.hpp>
#include <opencv2\ml\ml.hpp>
#include "SNMasterRecognizerStructs.h"
//----------------------------------------------------------------
typedef std::map<char, double> SNSymbolStats;
typedef std::vector<SNSymbolStats> SNNumberStats;
//----------------------------------------------------------------

class SNMasterSymbolRecognizer
{
public:
	SNMasterSymbolRecognizer();
	~SNMasterSymbolRecognizer();
	std::pair<char, double> ProcessChar(const cv::Mat& input);
	std::pair<char, double> ProcessNum(const cv::Mat& input);
	void Process(const cv::Mat& input, const SNFigureGroups& groups, SNNumberStats& stats);
private:
	std::pair<char, double> Process(const cv::Mat& input, cv::NeuralNet_MLP* mlp, bool is_num);
	cv::Mat ConvertToRow(const cv::Mat& input);
	int SearchMaxVal(const cv::Mat& data, int row = 0);
	void InitNN(cv::NeuralNet_MLP& mlp, const std::string& data);
	char IndexToCharNum(int index);
	char IndexToCharChar(int index);
	double PredictMinDiff(const cv::Mat& pred_out, int max_val);
private:
	cv::NeuralNet_MLP MLPChar;
	cv::NeuralNet_MLP MLPNum;
	const int DataWidth = 15;
	const int DataHeight = 22;
};
//----------------------------------------------------------------

#endif // SNMasterSymbolRecognizer_h__
