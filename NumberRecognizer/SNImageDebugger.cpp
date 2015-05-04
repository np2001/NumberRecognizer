#include "SNImageDebugger.h"
#include "opencv2\imgproc\imgproc.hpp"
//--------------------------------------------------------------------------------------

SNImageDebugger::SNImageDebugger()
{
}
//--------------------------------------------------------------------------------------

SNImageDebugger::~SNImageDebugger()
{
}
//--------------------------------------------------------------------------------------

void SNImageDebugger::DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroups& groups, cv::Mat& out_image, int scale /*= 1*/)
{
	for (auto group : groups)
	{
		cv::Scalar color = CV_RGB(rand() % 255, rand() % 255, rand() % 255);
		DebugFigureGroup(gray_image, group, out_image, scale, color);
	}
}
//--------------------------------------------------------------------------------------

void SNImageDebugger::DebugFigureGroup(const cv::Mat& gray_image, const SNFigureGroup& group, cv::Mat& out_image, int scale /*= 1*/, cv::Scalar color /*= cvScalar(255, 0, 0)*/)
{
	if (out_image.cols == 0)
	{
	cvtColor(gray_image, out_image, CV_GRAY2RGB);
	cv::resize(out_image, out_image, out_image.size() * scale);
	}

	//// отрисовываем найденные фигуры
	if (!group.empty())
	{
		for (size_t nn = 0; nn < group.size(); ++nn)
		{
			cv::rectangle(out_image, cv::Point(group[nn].left() * scale, group[nn].top() * scale), cv::Point(group[nn].right() * scale, group[nn].bottom() * scale), color, 1);
		}
	}
}
//--------------------------------------------------------------------------------------