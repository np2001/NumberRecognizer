#include "SNMasterSegmentor.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <set>
//---------------------------------------------------

SNMasterSegmentor::SNMasterSegmentor()
{
	PointsCount = 0;

	PixAround[0] = std::make_pair(0, 1);
	PixAround[1] = std::make_pair(-1, 0);
	PixAround[2] = std::make_pair(1, 0);
	PixAround[3] = std::make_pair(0, -1);
}
//---------------------------------------------------

SNMasterSegmentor::~SNMasterSegmentor()
{
}
//---------------------------------------------------

void SNMasterSegmentor::Segment(const cv::Mat& gray_image, SNFigureGroups& groups, int32_t min_gray_level /*= 0*/, int32_t max_gray_level /*= 255*/, int32_t gray_step /*= 10*/)
{
	SNFigureGroup group;

	for (int32_t gray_level = min_gray_level; gray_level <= max_gray_level; gray_level += gray_step)
	{
		cv::Mat img_bw = gray_image > gray_level;

		ParseToFigures(img_bw, group);
	}

	sort(group.begin(), group.end(),
		[](const SNFigure & a, const SNFigure & b) -> bool
	{
		return a.left() < b.left();
	});

	RemoveEqualRects(group);

	//MakeGroups(group, groups);
	//// выкидываем элементы, что выходят за размеры номера, предпологаем что номер не шире 7 * ширина первого элемента
	//FigsRemoveTooLongByXFromFirst(groups);

	//// выкидываем элементы, которые слишком далеко по высоте от предыдущего
	//FigsRemoveTooLongByYFromFirst(groups);

	//// выкидываем элементы, не пропорциональные первому элементу
	//FigsRemoveInvalidFromFirstBySize(groups);

	////// Выкидываем группы, которые включают в себя другие группы
	////GroupsRemoveIncluded(groups);

	////// сливаем пересекающиеся группы
	//GroupsMergeIntersects(groups);

	DebugFigureGroups(gray_image, group, 2);
}
//---------------------------------------------------

std::set<SNFigure> SNMasterSegmentor::ToSet(const SNFigureGroup& in)
{
	std::set<SNFigure> out;
	for (size_t nn = 0; nn < in.size(); ++nn)
	{
		out.insert(in.at(nn));
	}
	return out;
}
//---------------------------------------------------

// сливаем пересекающиеся группы
void SNMasterSegmentor::GroupsMergeIntersects(SNFigureGroups& groups)
{
	bool merge_was = true;
	while (merge_was)
	{
		merge_was = false;
		for (size_t nn = 0; nn < groups.size() && !merge_was; ++nn)
		{
			const std::set<SNFigure> s_cur_f = ToSet(groups[nn]);
			for (size_t mm = 0; mm < groups.size() && !merge_was; ++mm)
			{
				if (nn != mm)
				{
					std::set<SNFigure> s_test_f = ToSet(groups[mm]);

					if (find_first_of(s_test_f.begin(), s_test_f.end(), s_cur_f.begin(), s_cur_f.end()) != s_test_f.end())
					{
						// нашли пересечение
						std::set<SNFigure> ss_nn = ToSet(groups[nn]);
						const std::set<SNFigure> ss_mm = ToSet(groups[mm]);
						ss_nn.insert(ss_mm.begin(), ss_mm.end());
						std::vector<SNFigure> res;
						for (std::set<SNFigure>::const_iterator it = ss_nn.begin(); it != ss_nn.end(); ++it)
						{
							res.push_back(*it);
						}

						groups[nn] = res;
						groups.erase(groups.begin() + mm);
						merge_was = true;
						break;
					}
				}
			}
		}
	}
}
//------------------------------------------------------

// Выкидываем группы, которые включают в себя другие группы
void SNMasterSegmentor::GroupsRemoveIncluded(SNFigureGroups& groups)
{
	// todo: переписать постоянное создание set
	bool merge_was = true;
	while (merge_was)
	{
		merge_was = false;
		for (size_t nn = 0; nn < groups.size() && !merge_was; ++nn)
		{
			const std::set<SNFigure> s_cur_f = ToSet(groups[nn]);

			for (size_t mm = 0; mm < groups.size() && !merge_was; mm++)
			{
				if (nn != mm)
				{
					const std::set<SNFigure> s_test_f = ToSet(groups[mm]);
					if (includes(s_cur_f.begin(), s_cur_f.end(), s_test_f.begin(), s_test_f.end()))
					{
						// нашли совпадение
						groups.erase(groups.begin() + mm);
						merge_was = true;
						break;
					}
				}
			}
		}
	}
}
//------------------------------------------------------

void SNMasterSegmentor::FigsRemoveInvalidFromFirstBySize(SNFigureGroups& groups)
{
	for (size_t nn = 0; nn < groups.size(); ++nn)
	{
		assert(groups[nn].size() > 2);
		const SNFigure& first_fig = groups[nn][0];
		const double width_first = first_fig.right() - first_fig.left();
		const double height_first = first_fig.bottom() - first_fig.top();
		assert(width_first > 0.);
		for (int mm = groups[nn].size() - 1; mm >= 1; --mm)
		{
			const SNFigure& cur_fig = groups[nn][mm];
			const double width_cur = cur_fig.right() - cur_fig.left();
			const double height_cur = first_fig.bottom() - first_fig.top();
			if (width_cur > width_first * 1.5 || width_cur < width_first * 0.6
				|| height_cur > height_first * 1.5 || height_cur < height_first * 0.6)
			{
				groups[nn].erase(groups[nn].begin() + mm);
			}
		}
	}
	// выкидываем группы, где меньше 3-х элементов
	GroupsRemoveTooSmall(groups);
}

void SNMasterSegmentor::FigsRemoveTooLongByYFromFirst(SNFigureGroups& groups)
{
	for (size_t nn = 0; nn < groups.size(); ++nn)
	{
		if (groups[nn].size() > 2)
		{
			const SNFigure& first_fig = groups[nn].at(0);
			const double height_first = first_fig.Height();
			//assert(first_fig.height() > 0.);
			for (int mm = groups[nn].size() - 1; mm >= 1; --mm)
			{
				const SNFigure& cur_fig = groups[nn][mm];
				if (abs(double(cur_fig.top() - first_fig.top()) / 3.) > height_first)
				{
					groups[nn].erase(groups[nn].begin() + mm);
				}
			}
		}
	}
	// выкидываем группы, где меньше 3-х элементов
	GroupsRemoveTooSmall(groups);
}

void SNMasterSegmentor::FigsRemoveTooLongByXFromFirst(SNFigureGroups& groups)
{
	for (size_t nn = 0; nn < groups.size(); ++nn)
	{
		if (groups[nn].size() > 2)
		{
			const SNFigure& first_fig = groups[nn].at(0);
			const double width_first = first_fig.Width();
			//assert(first_fig.width() > 0.);
			for (int mm = groups[nn].size() - 1; mm >= 1; --mm)
			{
				const SNFigure& cur_fig = groups[nn][mm];
				if (double(cur_fig.left() - first_fig.left()) / 7. > width_first)
				{
					// @todo: тут можно оптимизировать и в начале найти самую допустимо-дальнюю, а уже затем чистить список
					groups[nn].erase(groups[nn].begin() + mm);
				}
				else
				{
					// тут брик, т.к. фигуры отсортированы по оси х и все предыдущие будут левее
					break;
				}
			}
		}
	}
	// выкидываем группы, где меньше 3-х элементов
	GroupsRemoveTooSmall(groups);
}
//------------------------------------------------------

void SNMasterSegmentor::GroupsRemoveTooSmall(SNFigureGroups& groups)
{
	for (int nn = groups.size() - 1; nn >= 0; --nn)
	{
		if (groups[nn].size() < 3)
		{
			groups.erase(groups.begin() + nn);
		}
	}
}
//------------------------------------------------------

void SNMasterSegmentor::RemoveEqualRects(SNFigureGroup& figs)
{
	for (SNFigureGroup::iterator i = figs.begin(); i != figs.end() - 1; )
	{
		SNFigureGroup::iterator next = i + 1;
		if (*i == *next)
			i = figs.erase(i);
		else
			++i;
	}
}
//------------------------------------------------------

void SNMasterSegmentor::ParseToFigures(cv::Mat& mat, SNFigureGroup& group)
{
	group.reserve(1000);

	for (int nn = 0; nn < mat.rows; ++nn)
	{
		for (int mm = 0; mm < mat.cols; ++mm)
		{
			if (mat.at<unsigned char>(nn, mm) == 0)
			{
				SNFigure fig_to_create;
				AddPixelAsSpy(nn, mm, mat, fig_to_create, 1, mat.rows - 1);

				if (fig_to_create.is_too_big())
				{
					// проверяем что высота больше ширины
					if (fig_to_create.Width() < fig_to_create.Height())
					{
						if (fig_to_create.Width() > 4)
						{
							if (fig_to_create.Height() / fig_to_create.Width() < 4)
							{
								group.push_back(fig_to_create);
							}
						}
					}
				}
			}
		}
	}
}
//---------------------------------------------------

void SNMasterSegmentor::DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroup& group, int scale /*= 1*/)
{
	//// отрисовываем найденные фигуры
	if (!group.empty())
	{
		cv::Mat colored_mat(gray_image.size(), CV_8UC3);
		cvtColor(gray_image, colored_mat, CV_GRAY2RGB);
		cv:resize(colored_mat, colored_mat, colored_mat.size() * 2);
		for (size_t nn = 0; nn < group.size(); ++nn)
		{
			cv::rectangle(colored_mat, cv::Point(group[nn].left() * scale, group[nn].top() * scale), cv::Point(group[nn].right() * scale, group[nn].bottom() * scale), CV_RGB(255, 0, 0), 1);
		}

		int r = 0;
	}
}
//---------------------------------------------------

void SNMasterSegmentor::DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroups& groups, int scale /*= 1*/)
{
	for (auto group : groups)
	{
		DebugFigureGroups(gray_image, group, scale);
	}
}
//---------------------------------------------------

void SNMasterSegmentor::AddPixelAsSpy(int row, int col, cv::Mat& mat, SNFigure& fig, int top_border, int bottom_border)
{
	//	time_mesure tm( "NEXT:" );
	if (top_border == -1) // верняя граница поиска
	{
		top_border = 0;
	}
	if (bottom_border == -1) // нижняя граница поиска
	{
		bottom_border = mat.rows;
	}

	PointsCount = 0;
	PointsDiplicateFirst[PointsCount] = row;
	PointsDiplicateSecond[PointsCount++] = col;

	// что бы не зациклилось
	mat.ptr<unsigned char>(row)[col] = 255;
	int cur_index = 0;
	while (cur_index < PointsCount)
	{
		const int& cur_nn = PointsDiplicateFirst[cur_index];
		const int& cur_mm = PointsDiplicateSecond[cur_index];

		for (size_t yy = 0; yy < sizeof(PixAround) / sizeof(PixAround[0]); ++yy)
		{
			const int &curr_pix_first = PixAround[yy].first + cur_nn;
			const int &curr_pix_second = PixAround[yy].second + cur_mm;
			if (curr_pix_first >= top_border && curr_pix_first < bottom_border
				&& curr_pix_second >= 0 && curr_pix_second < mat.cols)
			{
				if (mat.ptr< unsigned char >(curr_pix_first)[curr_pix_second] == 0)
				{
					PointsDiplicateFirst[PointsCount] = curr_pix_first;
					PointsDiplicateSecond[PointsCount++] = curr_pix_second;
					fig.AddPoint(pair_int(curr_pix_first, curr_pix_second));
					// что бы не зациклилось
					mat.ptr< unsigned char >(curr_pix_first)[curr_pix_second] = 255;
				}
			}
		}
		++cur_index;
	}

	//assert(fig.is_empty() || fig.left() >= 0);
}
//------------------------------------------------------

// по отдельным фигурам создаем группы фигур
void SNMasterSegmentor::MakeGroups(const SNFigureGroup& figs, SNFigureGroups& groups)
{
	using namespace std;
	
	for (size_t nn = 0; nn < figs.size(); ++nn)
	{
		vector<pair<double, SNFigureGroup>> cur_fig_groups;

		// todo: тут возможно стоит идти только от nn + 1, т.к. фигуры отсортированы и идем только вправо

		for (size_t mm = 0; mm < figs.size(); ++mm)
		{
			if (mm != nn)
			{
				if (figs[mm].left() > figs[nn].left())
				{
					// угол между левыми-нижними углами фигуры
					const double angle = 57.2957795 * atan2(static_cast<double>(figs[mm].left() - figs[nn].left()), static_cast<double>(figs[mm].bottom() - figs[nn].bottom()));
					bool found = false;
					for (size_t kk = 0; kk < cur_fig_groups.size(); ++kk)
					{
						// проверяем что попадает в группу
						if (AngleIsEqual(static_cast<int>(cur_fig_groups[kk].first), static_cast<int>(angle)))
						{
							bool ok = true;
							// проверяем что бы угол был такой же как и у всех елементов группы, что бы не было дуги или круга
							for (size_t yy = 1; yy < cur_fig_groups[kk].second.size(); ++yy)
							{
								const SNFigure& next_fig = cur_fig_groups[kk].second.at(yy);
								const double angle_to_fig = 57.2957795 * atan2(static_cast<double>(figs[mm].left() - next_fig.left()), static_cast<double>(figs[mm].bottom() - next_fig.bottom()));
								if (!AngleIsEqual(static_cast<int>(cur_fig_groups[kk].first), static_cast<int>(angle_to_fig)))
								{
									ok = false;
									break;
								}
							}
							if (ok)
							{
								cur_fig_groups[kk].second.push_back(figs[mm]);
								found = true;
								break;
							}
						}
					}
					// создаем группу
					if (!found)
					{
						SNFigureGroup to_add;
						to_add.push_back(figs[nn]);
						to_add.push_back(figs[mm]);
						cur_fig_groups.push_back(make_pair(angle, to_add));
					}
				}
			}
		}
		// проверяем что бы элементов в группе было больше 3-х
		for (size_t mm = 0; mm < cur_fig_groups.size(); ++mm)
		{
			if (cur_fig_groups[mm].second.size() >= 3)
			{
				// сортируем фигуры в группах, что бы они шли слева направо

				sort(cur_fig_groups[mm].second.begin(), cur_fig_groups[mm].second.end(),
					[](const SNFigure & a, const SNFigure & b) -> bool
				{
					return a.left() < b.left();
				});

				groups.push_back(cur_fig_groups[mm].second);
			}
		}
	}
	// выкидываем все группы, у которых угол больше 30 градусов
	for (int nn = groups.size() - 1; nn >= 0; --nn)
	{
		const double xx = abs(static_cast<double>(groups.at(nn).at(0).left() - groups.at(nn).at(1).left()));
		const double yy = abs(static_cast<double>(groups.at(nn).at(0).bottom() - groups.at(nn).at(1).bottom()));
		const double angle_to_fig = abs(57.2957795 * atan2(yy, xx));
		if (angle_to_fig > 30. && angle_to_fig < 330.)
		{
			groups.erase(groups.begin() + nn);
		}
	}
}
//---------------------------------------------------

// надо сделать какую-то пропорцию для зависимости угла от расстояния
bool SNMasterSegmentor::AngleIsEqual(int an1, int an2)
{
	const int angle_diff = 16;
	int an_max = an1 + angle_diff;
	int an_min = an1 - angle_diff;
	if (an_min >= 0 && an_max <= 360)
	{
		const bool ret = an2 < an_max && an2 > an_min;
		return ret;
	}
	else if (an_min <= 0)
	{
		an_min = an_min + 360;
		return an2 < an_max || an2 > an_min;
	}
	else if (an_max > 360)
	{
		an_max = an_max - 360;
		return an2 < an_max || an2 > an_min;
	}
	assert(false);
	return false;
}
//------------------------------------------------------

//std::vector<SNFoundNumber> SNMasterSegmentor::SearchNumber(cv::Mat& etal, std::vector<SNFigureGroup>& groups, const cv::Mat& original, SNNumberData& stat_data)
//{
//	// ищем позиции фигур и соответсвующие им символы
//	std::vector<SNFoundNumber> ret;
//
//	for (size_t nn = 0; nn < groups.size(); ++nn)
//	{
//		std::vector<SNFoundNumber> fig_nums_sums;
//		const SNFigureGroup& cur_gr = groups[nn];
//
//		// перебираем фигуры, подставляя их на разные места (пока перебираем только фигуры 0-1-2)
//		for (size_t mm = 0; mm < std::min(cur_gr.size(), size_t(2)); ++mm)
//		{
//			const SNFigure & cur_fig = cur_gr[mm];
//			const pair_int cen = cur_fig.center();
//			// подставляем текущую фигуру на все позиции (пока ставим только первую и вторую фигуру)
//			for (int ll = 0; ll < 1; ++ll)
//			{
//				// меняем угол наклона номера относительно нас (0 - смотрим прям на номер, если угол меньше 0, то определяем номер с 2-х значным регионом)
//				for (int oo = -60; oo < 50; oo += 10)
//				{
//					const std::vector< pair_int > pis = CalcSymCenters(ll, oo, cur_fig.Height());
//					assert(pis.size() == 6); // всегда 6 символов без региона в номере
//					const std::vector<SNFoundSymbol> figs_by_pos = FigsSearchSyms(pis, cen, cur_gr, original, stat_data);
//					const SNFoundNumber number_sum = CreateNumberByPos(pis, figs_by_pos);
//					fig_nums_sums.push_back(number_sum);
//				}
//			}
//		}
//
//		// выбираем лучшее
//		if (!fig_nums_sums.empty())
//		{
//			ret.push_back(FindBestNumberByWeight(fig_nums_sums, &etal));
//		}
//	}
//
//	// отрисовываем выбранные группы
//	/*	for ( size_t nn = 0; nn < groups.size(); ++nn )
//	{
//	Mat colored_rect( etal.size(), CV_8UC3 );
//	cvtColor( etal, colored_rect, CV_GRAY2RGB );
//	for ( int mm = 0; mm < groups.at( nn ).size(); ++mm )
//	{
//	const figure* cur_fig = &groups.at( nn ).at( mm );
//	rectangle( colored_rect, Point( cur_fig->left(), cur_fig->top() ), Point( cur_fig->right(), cur_fig->bottom() ), CV_RGB( 0, 255, 0 ) );
//	}
//	imwrite( next_name( "d" ), colored_rect );
//	recog_debug->out_image( colored_rect );
//	}*/
//	return ret;
//}
//
