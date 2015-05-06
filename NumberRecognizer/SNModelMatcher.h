#ifndef SNModelMatcher_h__
#define SNModelMatcher_h__
//------------------------------------------------------
#include "opencv2\highgui\highgui.hpp"
#include "SNMasterRecognizerStructs.h"
#include <stdint.h>
#include "..\Common\SNNumberRecognizerDefines.h"
//------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNModelMatcher
	{
	public:
		SNModelMatcher();
		~SNModelMatcher();
		void AddModelElement(const cv::Rect& rect, const AlphabetTypes type);
		void BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model);
		void DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, float mean_compare_ratio, int scale = 1);
		bool MatchModel2(const cv::Mat& gray_image, SNFigureGroups& fgs, SNPlateModel& out_model, int32_t& best_group_to_start);
		bool MatchModel3(SNPlate& plate, SNPlateModels& out_models);
	public:
		SNPlateModel Model;

	private:
		SNPlateModelCenterVectors CenterVectors;
	private:
		void CalcCenterVectors();
		cv::Point2f CalcCenter(const cv::Rect& r1);
		void CalcCenterVector(const cv::Point2f& c1, const cv::Point2f& c2, SNPlateModelCenterVector& vec);
		cv::Point2f AffineTransform(const cv::Point2f& p, const cv::Mat& trans_mat);


	};
}
//------------------------------------------------------

#endif // SNModelMatcher_h__
