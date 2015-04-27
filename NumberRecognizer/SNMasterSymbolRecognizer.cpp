#include "SNMasterSymbolRecognizer.h"
#include <opencv2\imgproc\imgproc.hpp>
#include "neural_net_char.cpp"
#include "neural_net_num.cpp"
//----------------------------------------------------------------

SNMasterSymbolRecognizer::SNMasterSymbolRecognizer()
{
	InitNN(MLPChar, std::string(neural_net_char, sizeof(neural_net_char)));
	InitNN(MLPNum, std::string(neural_net_num, sizeof(neural_net_num)));
}
//----------------------------------------------------------------

SNMasterSymbolRecognizer::~SNMasterSymbolRecognizer()
{
}
//----------------------------------------------------------------

std::pair<char, double> SNMasterSymbolRecognizer::ProcessChar(const cv::Mat& input)
{
	return Process(input, &MLPChar, false);
}
//----------------------------------------------------------------

std::pair<char, double> SNMasterSymbolRecognizer::ProcessNum(const cv::Mat& input)
{
	return Process(input, &MLPNum, true);
}
//----------------------------------------------------------------

void SNMasterSymbolRecognizer::InitNN(cv::NeuralNet_MLP& mlp, const std::string& data)
{
	assert(mlp.get_layer_count() == 0);
	try
	{
		cv::FileStorage fs(data, cv::FileStorage::READ | cv::FileStorage::MEMORY);
		cv::FileNode fn = fs["mlp"];
		if (!fn.empty())
		{
			mlp.read(*fs, *fn);
		}
		else
		{
			//throw runtime_error("invalid data in file");
		}
	}
	catch (const std::exception& e)
	{
		//throw runtime_error(string("Failed load train neural network state, reason: ") + e.what());
	}
}
//----------------------------------------------------------------

int SNMasterSymbolRecognizer::SearchMaxVal(const cv::Mat& data, int row)
{
	int max_col = 0;
	float max_val = data.at< float >(row, 0);
	for (int nn = 1; nn < data.cols; ++nn)
	{
		if (data.at< float >(row, nn) > max_val)
		{
			max_col = nn;
			max_val = data.at< float >(row, nn);
		}
	}
	return max_col;
}
//----------------------------------------------------------------

cv::Mat SNMasterSymbolRecognizer::ConvertToRow(const cv::Mat& input)
{
	cv::Mat one_chan_gray;
	if (input.channels() == 3 && input.depth() == CV_MAT_DEPTH(CV_8U))
	{
		cv::Mat gray(input.size(), CV_8U);
		cvtColor(input, one_chan_gray, CV_RGB2GRAY);
	}
	else if (input.channels() == 1 && input.depth() == CV_MAT_DEPTH(CV_8U))
	{
		//		one_chan_gray = input.clone();
		one_chan_gray = input;
	}
	else
	{
		assert(!"Unsupported image format");
	}

	if (!one_chan_gray.empty())
	{
		// сглаживаем
		boxFilter(one_chan_gray, one_chan_gray, -1, cv::Size(3, 3));
		// раст€гиваем по цвету
		equalizeHist(one_chan_gray, one_chan_gray);
		cv::Mat gray_float(one_chan_gray.size(), CV_32F);
		one_chan_gray.convertTo(gray_float, CV_32F);

		cv::Mat float_sized(DataHeight, DataWidth, CV_32F);
		cv::resize(gray_float, float_sized, float_sized.size());

		cv::Mat ret(1, DataHeight * DataWidth, CV_32F);
		for (int mm = 0; mm < DataHeight; ++mm)
		{
			for (int kk = 0; kk < DataWidth; ++kk)
			{
				const int cur_el = mm * DataWidth + kk;
				float val = float_sized.at< float >(mm, kk);
				val = val / 255.F;
				ret.at< float >(0, cur_el) = val;
			}
		}

		return ret;
	}
	return cv::Mat();
}
//----------------------------------------------------------------

std::pair<char, double> SNMasterSymbolRecognizer::Process(const cv::Mat& input, cv::NeuralNet_MLP* mlp, bool is_num)
{
	//assert(mlp.get_layer_count() != 0);
	cv::Mat pred_out;
	mlp->predict(ConvertToRow(input), pred_out);
	const int max_val = SearchMaxVal(pred_out);

	char c = is_num ? IndexToCharNum(max_val) : IndexToCharChar(max_val);

	const std::pair< char, double > ret = std::make_pair(c, PredictMinDiff(pred_out, max_val));

	return ret;
}
//----------------------------------------------------------------

double SNMasterSymbolRecognizer::PredictMinDiff(const cv::Mat& pred_out, int max_val)
{
	float ret = 100.; // the current version of the OpenCV Neural Network, the value will not exceed 1.76
	for (int nn = 0; nn < pred_out.cols; ++nn)
	{
		if (nn != max_val)
		{
			const float curr_diff = abs(pred_out.at< float >(0, max_val) - pred_out.at< float >(0, nn));
			if (curr_diff < ret)
			{
				ret = curr_diff;
			}
		}
	}
	return ret * 1000.;
}
//----------------------------------------------------------------

char SNMasterSymbolRecognizer::IndexToCharNum(int index)
{
	switch (index)
	{
	default:
		//assert(!"govno");
		return '?';
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	}
}
//----------------------------------------------------------------

char SNMasterSymbolRecognizer::IndexToCharChar(int index)
{
	switch (index)
	{
	default:
		//assert(!"govno");
		return '?';
	case 0:
		return 'A';
	case 1:
		return 'B';
	case 2:
		return 'C';
	case 3:
		return 'E';
	case 4:
		return 'H';
	case 5:
		return 'K';
	case 6:
		return 'M';
	case 7:
		return 'O';
	case 8:
		return 'P';
	case 9:
		return 'T';
	case 10:
		return 'X';
	case 11:
		return 'Y';
	}
}




