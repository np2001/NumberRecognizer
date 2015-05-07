#include "SNModelMatcher.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\video.hpp>

#include <windows.h>
#include <stdint.h>
//------------------------------------------------------
namespace SNNumberRecognizer
{
	SNModelMatcher::SNModelMatcher()
	{
		AddModelElement(cv::Rect(0, 7, 9, 13), LettersAlphabet);
		AddModelElement(cv::Rect(12, 4, 10, 16), DigitsAlphabet);
		AddModelElement(cv::Rect(23, 4, 10, 16), DigitsAlphabet);
		AddModelElement(cv::Rect(34, 4, 10, 16), DigitsAlphabet);
		AddModelElement(cv::Rect(46, 7, 9, 13), LettersAlphabet);
		AddModelElement(cv::Rect(56, 7, 9, 13), LettersAlphabet);
		AddModelElement(cv::Rect(71, 2, 7, 13), DigitsAlphabet);
		AddModelElement(cv::Rect(79, 2, 7, 13), DigitsAlphabet);
		CalcCenterVectors();
	}
	//------------------------------------------------------

	SNModelMatcher::~SNModelMatcher()
	{
	}
	//------------------------------------------------------

	void SNModelMatcher::AddModelElement(const cv::Rect& rect, const AlphabetTypes type)
	{
		SNPlateModelSymbol s;
		s.SymbolType = type;
		s.SymbolRect = rect;

		Model.push_back(s);
	}
	//------------------------------------------------------

	void SNModelMatcher::CalcCenterVectors()
	{
		CenterVectors.clear();

		//cv::Point2f c0 = CalcCenter(Model[0]);

		for (auto p : Model)
		{
			SNPlateModelCenterVector cvec;

			cv::Point2f c = CalcCenter(p.SymbolRect);
			cvec.Vector = c;

			//CalcCenterVector(c0, c, cvec);

			CenterVectors.push_back(cvec);
		}
	}
	//------------------------------------------------------

	cv::Point2f SNModelMatcher::CalcCenter(const cv::Rect& r1)
	{
		return cv::Point2f(r1.x + r1.width / 2.0f, r1.y + r1.height / 2.0f);
	}
	//------------------------------------------------------

	void SNModelMatcher::BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model)
	{
		cv::Point2f c1 = CalcCenter(r1);
		cv::Point2f c2 = CalcCenter(r2);

		SNPlateModelCenterVector& cvec0 = CenterVectors[1];

		SNPlateModelCenterVector cvec;

		CalcCenterVector(c1, c2, cvec);

		float scale_ratio = cvec.VectorLength / cvec0.VectorLength;

		//out_model.push_back(r1);

		for (size_t i = 0; i < Model.size(); ++i)
		{
			cv::Point2f scaled_cv = CenterVectors[i].Vector * scale_ratio;

			cv::Point2f center = c1 + scaled_cv;

			SNPlateModelSymbol s;
			cv::Rect r(
				(int32_t)(center.x - Model[i].SymbolRect.width * scale_ratio / 2), 
				(int32_t)(center.y - Model[i].SymbolRect.height * scale_ratio / 2), 
				(int32_t)(Model[i].SymbolRect.width * scale_ratio), 
				(int32_t)(Model[i].SymbolRect.height * scale_ratio));
			s.SymbolRect = r;
			s.SymbolType = UnknownAlphabet;
			out_model.push_back(s);
		}
	}
	//------------------------------------------------------

	void SNModelMatcher::CalcCenterVector(const cv::Point2f& c1, const cv::Point2f& c2, SNPlateModelCenterVector& vec)
	{
		vec.Vector = c2 - c1;
		vec.VectorLength = sqrt(vec.Vector.x * vec.Vector.x + vec.Vector.y * vec.Vector.y);
	}
	//------------------------------------------------------

	void SNModelMatcher::DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, float mean_compare_ratio, int scale /*= 1*/)
	{
		cv::Mat out_image;
		cvtColor(gray_image, out_image, CV_GRAY2RGB);
		cv::resize(out_image, out_image, out_image.size() * scale);

		for (auto m : out_model)
		{
			cv::Scalar color = CV_RGB(255, 0, 0);
			//// отрисовываем найденные фигуры
			cv::rectangle(out_image, cv::Rect(m.SymbolRect.x * scale, m.SymbolRect.y * scale, m.SymbolRect.width * scale, m.SymbolRect.height * scale), color, 1);
		}

		int r = 0;
	}
	//------------------------------------------------------

	cv::Point2f SNModelMatcher::AffineTransform(const cv::Point2f& p, const cv::Mat& trans_mat)
	{
		cv::Mat x = cv::Mat(3, 1, CV_32FC1);
		x.at<float>(0) = p.x;
		x.at<float>(1) = p.y;
		x.at<float>(2) = 1;

		cv::Mat lt = trans_mat * x;

		return cv::Point2f(lt.at<float>(0), lt.at<float>(1));
	}
	//------------------------------------------------------

	bool SNModelMatcher::MatchModel2(const cv::Mat& gray_image, SNFigureGroups& fgs, SNPlateModel& out_model, int32_t& best_group_to_start)
	{
		std::vector<cv::Point2f> src;

		for (int i = 0; i < (int)CenterVectors.size() - 2; ++i)
		{
			src.push_back(CenterVectors[i].Vector);
		}

		cv::Point2f dst[3];

		float best_compare_ratio = 0.0f;

		for (int i = 0; i <= (int)fgs.size() - (int)Model.size() + 2; ++i)
		{
			std::vector<cv::Point2f> dst;

			for (int j = 0; j < (int)Model.size() - 2; ++j)
			{
				dst.push_back(cv::Point2f((float)fgs[i + j].front().center().first, (float)fgs[i + j].front().center().second));
			}

			cv::Mat trans_mat = cv::estimateRigidTransform(src, dst, false);

			if (trans_mat.cols && trans_mat.rows)
			{
				SNPlateModel pm;

				float compare_ratio_sum = 0.0f;
				int compare_ratio_count = 0;

				for (size_t k = 0; k < Model.size(); ++k)
				{
					cv::Point2f tl = AffineTransform(cv::Point2f(Model[k].SymbolRect.tl()), trans_mat);
					cv::Point2f br = AffineTransform(cv::Point2f(Model[k].SymbolRect.br()), trans_mat);

					cv::Rect model_rect = cv::Rect(tl, br);

					if (i + k < (int)fgs.size())
					{
						cv::Rect detected_rect = cv::Rect(fgs[i + k].front().left(), fgs[i + k].front().top(), fgs[i + k].front().Width(), fgs[i + k].front().Height());

						float compare_ratio = (model_rect & detected_rect).area() * 1.0f / (model_rect | detected_rect).area();
						compare_ratio_sum += compare_ratio;
						compare_ratio_count++;
					}

					SNPlateModelSymbol s;
					s.SymbolRect = model_rect;
					s.SymbolType = UnknownAlphabet;
					pm.push_back(s);
				}

				compare_ratio_sum /= compare_ratio_count;

				if (compare_ratio_sum > best_compare_ratio)
				{
					best_compare_ratio = compare_ratio_sum;
					best_group_to_start = i;
					out_model = pm;
				}


			}
		}

		DebugModel(gray_image, out_model, 0.0f);

		return true;
	}
	//------------------------------------------------------

	bool SNModelMatcher::MatchModel3(SNPlate& plate, SNPlateModels& out_models)
	{
		for (auto& fg : plate.FigureGroups)
		{
			for (auto& f : fg)
			{
				float rel_height = f.Height() * 1.0f / plate.PlateImage.rows;
				int r = 0;
			}
		}

		return true;
	}
}
//------------------------------------------------------