#include "SNANNPredictor.h"
#include "opencv2\core\core_c.h"
#include <QDebug>
//#include "SNANNLearningData.h"
//-------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNANNPredictor::SNANNPredictor()
	{
	}
	//-------------------------------------------------------------------------------

	SNANNPredictor::~SNANNPredictor()
	{
	}
	//-------------------------------------------------------------------------------

	void SNANNPredictor::Load(SNANNConfigMap& ann_config)
	{
		for (auto alph : ann_config)
		{
			cv::FileStorage fs(alph.second.ConfigString, cv::FileStorage::MEMORY);

			CvFileStorage* fss = *fs;

			CvFileNode* model_node = 0;

			CvFileNode* root = cvGetRootFileNode(fss);
			if (root->data.seq->total > 0)
				model_node = (CvFileNode*)cvGetSeqElem(root->data.seq, 0);

			Alphabets[alph.first] = ANNAlphabet();
			Alphabets[alph.first].ANN.read(fss, model_node);
		}
	}
	//-------------------------------------------------------------------------------

	int32_t SNANNPredictor::Predict(AlphabetTypes alphabet_type, const ANNClassItem& features, float& weight)
	{
		cv::Mat output;
		Alphabets[alphabet_type].ANN.predict(features.Features, output);

		float max_response = -1000;
		int max_response_idx = -1;

		for (int i = 0; i < output.cols; ++i)
		{
			float res = output.at<float>(i);
			if (res> 0 && res > max_response)
			{
				max_response = res;
				max_response_idx = i;
			}
		}

		weight = max_response;

		return max_response_idx;
	};
	//-------------------------------------------------------------------------------

	int32_t SNANNPredictor::Predict(AlphabetTypes type, const cv::Mat& image, SNANNFeatureEvaluator& eval, float& weight)
	{
		ANNClassItem features;
		features.Features = eval.Features(image);

		int32_t class_id = Predict(type, features, weight);

		/*if (class_id >= 0)
		{
			SNANNPredictionResult res;
			res.TopLeft = cv::Point(j, i);
			res.WinSize = win_size;
			res.Weight = weight;
			res.ClassID = class_id;

			line_results[class_id].push_back(res);
			weightsum += weight;
		}*/

		return class_id;
	}
	//-------------------------------------------------------------------------------

	//void SNANNPredictor::Predict(const cv::Mat& image, SNANNFeatureEvaluator& eval, cv::Size win_size, SNANNPredictionResults& results)
	//{
		/*for (int i = 0; i <= image.rows - win_size.height; ++i)
		{
		float weightsum = 0;

		SNANNLinePredictionResults line_results;
		line_results.LineNum = i;

		for (int j = 0; j <= image.cols - win_size.width; ++j)
		{
		cv::Rect roi = cv::Rect(j, i, win_size.width, win_size.height);
		cv::Mat roi_image = cv::Mat(image, roi);

		SNANNItemFeatures features;
		features.Features = eval.Features(roi_image);

		float weight = 0.0f;
		int32_t class_id = Predict(features, weight);
		if (class_id >= 0)
		{
		SNANNPredictionResult res;
		res.TopLeft = cv::Point(j, i);
		res.WinSize = win_size;
		res.Weight = weight;
		res.ClassID = class_id;

		line_results[class_id].push_back(res);
		weightsum += weight;
		}
		}

		line_results.WeightSum = weightsum;
		results.push_back(line_results);
		}

		std::sort(results.begin(), results.end(), SortANNPredictionResults);*/
	//}
	//-------------------------------------------------------------------------------

	//void SNANNPredictor::Predict(const cv::Mat& image, SNANNFeatureEvaluator& eval, SNPlateTemplate& templ, SNANNPredictionResults& results)
	//{
	//	int32_t min_y = 10000;
	//	int32_t max_y = 0;

	//	int32_t min_x = 10000;
	//	int32_t max_x = 0;

	//	for (auto& t : templ)
	//	{
	//		if (t.Window.br().y > max_y)
	//			max_y = t.Window.br().y;

	//		if (t.Window.tl().y < min_y)
	//			min_y = t.Window.tl().y;

	//		if (t.Window.br().x > max_x)
	//			max_x = t.Window.br().x;

	//		if (t.Window.tl().x < min_x)
	//			min_x = t.Window.tl().x;
	//	}

	//	int32_t templ_width = max_x - min_x;
	//	int32_t templ_height = max_y - min_y;

	//	for (int i = 0; i <= image.rows - templ_height; ++i)
	//	{
	//		float weightsum = 0;

	//		SNANNLinePredictionResults line_results;
	//		line_results.LineNum = i;

	//		for (int j = 0; j <= image.cols - templ_width; ++j)
	//		{
	//			for (int k = -2; k <= 2; ++k)
	//			{
	//				for (int l = -2; l <= 2; ++l)
	//				{
	//					//for (float m = 0.9; m < 1.1; m *= 1.01)
	//					{
	//						//for (float n = 0.9; n < 1.1; n *= 1.01)
	//						{
	//							std::string res;

	//							float ws = 0.0f;

	//							for (auto& t : templ)
	//							{
	//								cv::Rect roi = cv::Rect(j + t.Window.x + k, i + t.Window.y + l, t.Window.width, t.Window.height);

	//								if (roi.tl().x >= 0 && roi.br().x <= image.cols && roi.tl().y >= 0 && roi.br().y <= image.rows)
	//								{
	//									cv::Mat roi_image = cv::Mat(image, roi);

	//									SNANNItemFeatures features;
	//									features.Features = eval.Features(roi_image);

	//									float weight = 0.0f;
	//									int32_t class_id = Predict(t.AlphabetIdx, features, weight);

	//									char c = '*';
	//									if (class_id >= 0 && class_id < Alphabets[t.AlphabetIdx].Symbols.size())
	//									{
	//										ws += weight;
	//										c = Alphabets[t.AlphabetIdx].Symbols[class_id];

	//									}

	//									res += c;
	//								}
	//							}
	//							if (ws > 2)
	//								qDebug() << res.c_str() << " " << ws;

	//						}

	//					}

	//				}

	//			}


	//			int r = 0;
	//		}
	//	}

	//}
	////-------------------------------------------------------------------------------

	void SNANNPredictor::CalcDetectionVariants(SNANNPredictionResults& results)
	{
		SNColumnPredictionResults col_res;

		int count = 0;

		for (auto line_results : results)
		{
			for (auto class_results : line_results)
			{
				for (auto r : class_results.second)
				{
					col_res[r.TopLeft.x] = r;
				}
			}

			if (count++ == 0)
				break;

			col_res.clear();
		}

		SNPlateVariants variants;

		for (auto r : col_res)
		{
			SNPlateVariants new_variants;

			for (SNPlateVariants::iterator v = variants.begin(); v != variants.end(); ++v)
			{
				if (v->size())
				{
					int next_pos = v->back().TopLeft.x + v->back().WinSize.width;
					if (abs(next_pos - r.second.TopLeft.x) < 3)
					{
						SNPlateVariant new_variant = *v;
						new_variant.WeightSum += r.second.Weight;

						char c[10];
						itoa(r.second.ClassID, c, 10);
						new_variant.Number += c;
						new_variant.push_back(r.second);
						new_variants.push_back(new_variant);
					}
				}
			}

			for (SNPlateVariants::iterator v = new_variants.begin(); v != new_variants.end(); ++v)
			{
				variants.push_back(*v);
			}

			SNPlateVariant pv;
			pv.WeightSum = r.second.Weight;

			char c[10];
			itoa(r.second.ClassID, c, 10);
			pv.Number = c;
			pv.push_back(r.second);
			variants.push_back(pv);
		}

		SNPlateVariantsRating rating;

		for (SNPlateVariants::iterator v = variants.begin(); v != variants.end();)
		{
			if (v->size() != 3)
			{
				v = variants.erase(v);
			}
			else
			{
				SNPlateVariantsRating::iterator i = rating.find(v->Number);
				if (i != rating.end())
				{
					i->second += v->WeightSum;
				}
				else
				{
					rating[v->Number] = v->WeightSum;
				}

				++v;
			}
		}

		//1000 - a0170a
		//2000 - b334rk
		//3000 - t269rt
		//4000 - t219yn
		//5000 - t774rv
	}
}
//-------------------------------------------------------------------------------