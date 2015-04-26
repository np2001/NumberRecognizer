#include "SNMasterRecognizer.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <utility>
#include <set>
//------------------------------------------------------

SNMasterRecognizer::SNMasterRecognizer()
{
	SetSearchLevels(5);

	for (int i = 0; i < COUNT_GLOBAL_DATA; ++i)
	{
		BusyIndexes[i] = false;
		PointsCount[i] = 0;

		PixAround[i][0] = std::make_pair(0, 1);
		PixAround[i][1] = std::make_pair(-1, 0);
		PixAround[i][2] = std::make_pair(1, 0);
		PixAround[i][3] = std::make_pair(0, -1);
	}
	
	RegionCodes.insert("178"); // ХЗ
	//aux::g_region_codes.insert("01"); // Республика Адыгея
	//aux::g_region_codes.insert("02");
	//aux::g_region_codes.insert("102"); // Республика Башкортостан
	//aux::g_region_codes.insert("03"); // Республика Бурятия
	//aux::g_region_codes.insert("04"); // Республика Алтай (Горный Алтай)
	//aux::g_region_codes.insert("05"); // Республика Дагестан
	//aux::g_region_codes.insert("06"); // Республика Ингушетия
	//aux::g_region_codes.insert("07"); // Кабардино-Балкарская Республика
	//aux::g_region_codes.insert("08"); // Республика Калмыкия
	//aux::g_region_codes.insert("09"); // Республика Карачаево-Черкессия
	//aux::g_region_codes.insert("10"); // Республика Карелия
	//aux::g_region_codes.insert("11"); // Республика Коми
	//aux::g_region_codes.insert("12"); // Республика Марий Эл
	//aux::g_region_codes.insert("13");
	//aux::g_region_codes.insert("113"); // Республика Мордовия
	//aux::g_region_codes.insert("14"); // Республика Саха (Якутия)
	//aux::g_region_codes.insert("15"); // Республика Северная Осетия-Алания
	//aux::g_region_codes.insert("16");
	//aux::g_region_codes.insert("116"); // Республика Татарстан
	//aux::g_region_codes.insert("17"); // Республика Тыва
	//aux::g_region_codes.insert("18"); // Удмуртская Республика
	//aux::g_region_codes.insert("19"); // Республика Хакасия
	//aux::g_region_codes.insert("20"); // утилизировано (бывшая Чечня)
	//aux::g_region_codes.insert("21");
	//aux::g_region_codes.insert("121"); // Чувашская Республика
	//aux::g_region_codes.insert("22"); // Алтайский край
	//aux::g_region_codes.insert("23");
	//aux::g_region_codes.insert("93"); // Краснодарский край
	//aux::g_region_codes.insert("24");
	//aux::g_region_codes.insert("84");
	//aux::g_region_codes.insert("88");
	//aux::g_region_codes.insert("124"); // Красноярский край
	//aux::g_region_codes.insert("25");
	//aux::g_region_codes.insert("125"); // Приморский край
	//aux::g_region_codes.insert("26"); // Ставропольский край
	//aux::g_region_codes.insert("27"); // Хабаровский край
	//aux::g_region_codes.insert("28"); // Амурская область
	//aux::g_region_codes.insert("29"); // Архангельская область
	//aux::g_region_codes.insert("30"); // Астраханская область
	//aux::g_region_codes.insert("31"); // Белгородская область
	//aux::g_region_codes.insert("32"); // Брянская область
	//aux::g_region_codes.insert("33"); // Владимирская область
	//aux::g_region_codes.insert("34"); // Волгоградская область
	//aux::g_region_codes.insert("35"); // Вологодская область
	//aux::g_region_codes.insert("36"); // Воронежская область
	//aux::g_region_codes.insert("37"); // Ивановская область
	//aux::g_region_codes.insert("38");
	//aux::g_region_codes.insert("85");
	//aux::g_region_codes.insert("138"); // Иркутская область
	//aux::g_region_codes.insert("39");
	//aux::g_region_codes.insert("91"); // Калининградская область
	//aux::g_region_codes.insert("40"); // Калужская область
	//aux::g_region_codes.insert("41");
	//aux::g_region_codes.insert("82"); // Камчатский край
	//aux::g_region_codes.insert("42"); // Кемеровская область
	//aux::g_region_codes.insert("43"); // Кировская область
	//aux::g_region_codes.insert("44"); // Костромская область
	//aux::g_region_codes.insert("45"); // Курганская область
	//aux::g_region_codes.insert("46"); // Курская область
	//aux::g_region_codes.insert("47"); // Ленинградская область
	//aux::g_region_codes.insert("48"); // Липецкая область
	//aux::g_region_codes.insert("49"); // Магаданская область
	//aux::g_region_codes.insert("50");
	//aux::g_region_codes.insert("90");
	//aux::g_region_codes.insert("150");
	//aux::g_region_codes.insert("190"); // Московская область
	//aux::g_region_codes.insert("51"); // Мурманская область
	//aux::g_region_codes.insert("52");
	//aux::g_region_codes.insert("152"); // Нижегородская область
	//aux::g_region_codes.insert("53"); // Новгородская область
	//aux::g_region_codes.insert("54");
	//aux::g_region_codes.insert("154"); // Новосибирская область
	//aux::g_region_codes.insert("55"); // Омская область
	//aux::g_region_codes.insert("56"); // Оренбургская область
	//aux::g_region_codes.insert("57"); // Орловская область
	//aux::g_region_codes.insert("58"); // Пензенская область
	//aux::g_region_codes.insert("59");
	//aux::g_region_codes.insert("81");
	//aux::g_region_codes.insert("159"); // Пермский край
	//aux::g_region_codes.insert("60"); // Псковская область
	//aux::g_region_codes.insert("61");
	//aux::g_region_codes.insert("161"); // Ростовская область
	//aux::g_region_codes.insert("62"); // Рязанская область
	//aux::g_region_codes.insert("63");
	//aux::g_region_codes.insert("163"); // Самарская область
	//aux::g_region_codes.insert("64");
	//aux::g_region_codes.insert("164"); // Саратовская область
	//aux::g_region_codes.insert("65"); // Сахалинская область
	//aux::g_region_codes.insert("66");
	//aux::g_region_codes.insert("96"); // Свердловская область
	//aux::g_region_codes.insert("67"); // Смоленская область
	//aux::g_region_codes.insert("68"); // Тамбовская область
	//aux::g_region_codes.insert("69"); // Тверская область
	//aux::g_region_codes.insert("70"); // Томская область
	//aux::g_region_codes.insert("71"); // Тульская область
	//aux::g_region_codes.insert("72"); // Тюменская область
	//aux::g_region_codes.insert("73");
	//aux::g_region_codes.insert("173"); // Ульяновская область
	//aux::g_region_codes.insert("74");
	//aux::g_region_codes.insert("174"); // Челябинская область
	//aux::g_region_codes.insert("75");
	//aux::g_region_codes.insert("80"); // Забайкальский край
	//aux::g_region_codes.insert("76"); // Ярославская область
	//aux::g_region_codes.insert("77");
	//aux::g_region_codes.insert("97");
	//aux::g_region_codes.insert("99");
	//aux::g_region_codes.insert("177");
	//aux::g_region_codes.insert("199");
	//aux::g_region_codes.insert("197"); // г. Москва
	//aux::g_region_codes.insert("78");
	//aux::g_region_codes.insert("98");
	//aux::g_region_codes.insert("198"); // г. Санкт-Петербург
	//aux::g_region_codes.insert("79"); // Еврейская автономная область
	//aux::g_region_codes.insert("83"); // Ненецкий автономный округ
	//aux::g_region_codes.insert("86"); // Ханты-Мансийский автономный округ - Югр
	//aux::g_region_codes.insert("87"); // Чукотский автономный округ
	//aux::g_region_codes.insert("89"); // Ямало-Ненецкий автономный округ
	//aux::g_region_codes.insert("92"); // Резерв МВД Российской Федерации
	//aux::g_region_codes.insert("94"); // Территории, которые находятся вне РФ и
	//aux::g_region_codes.insert("95"); // Чеченская республика
}
//------------------------------------------------------

SNMasterRecognizer::~SNMasterRecognizer()
{
}
//------------------------------------------------------

void SNMasterRecognizer::SetSearchLevels(int32_t gray_step /*= 10*/)
{
	if (gray_step <= 0 || gray_step >= 256)
		gray_step = 10;

	for (int nn = gray_step; nn < 255; nn += gray_step)
	{
		SearchLevels.push_back(nn);
	}
}
//------------------------------------------------------

int SNMasterRecognizer::GetFreeIndex()
{
	for (int nn = 0; nn < COUNT_GLOBAL_DATA; ++nn)
	{
		if (!BusyIndexes[nn])
		{
			BusyIndexes[nn] = true;
			return nn;
		}
	}

	return -1;
	//throw runtime_error("no free data");
}
//------------------------------------------------------

void SNMasterRecognizer::SetFreeIndex(int index)
{
	assert(index >= 0 && index < COUNT_GLOBAL_DATA);
	BusyIndexes[index] = false;
}
//------------------------------------------------------

cv::Mat CreateGrayImage(const cv::Mat& input)
{
	cv::Mat gray(input.size(), CV_8U);
	if (input.channels() == 1)
	{
		gray = input;
	}
	else if (input.channels() == 3 || input.channels() == 4)
	{
		cvtColor(input, gray, CV_RGB2GRAY);
	}
	else
	{
		//cout << "!!! Invalid chanel count: " << input.channels() << " !!!" << endl;
		//assert(!"не поддерживаемое количество каналов");
	}

	return gray;
}
//------------------------------------------------------

std::vector<SNFoundNumber> SNMasterRecognizer::ReadNumberImpl(int stat_data_index, const cv::Mat& input, int gray_level, SNNumberData& stat_data)
{
	const cv::Mat& gray_image = CreateGrayImage(input);

	cv::Mat img_bw = gray_image > gray_level;
	cv::Mat img_to_rez = img_bw.clone();

	//	remove_single_pixels( img_bw );
	// ищем фигуры
	std::vector<SNFigure> figs = ParseToFigures(stat_data_index, img_bw);

	// бьем по группам в зависимости от угла наклона элементов отностительно друг друга
	std::vector<SNFigureGroup> groups = MakeGroups(figs);
	
	// выкидываем элементы, что выходят за размеры номера, предпологаем что номер не шире 7 * ширина первого элемента
	FigsRemoveTooLongByXFromFirst(groups);
	
	// выкидываем элементы, которые слишком далеко по высоте от предыдущего
	FigsRemoveTooLongByYFromFirst(groups);
	
	// выкидываем элементы, не пропорциональные первому элементу
	FigsRemoveInvalidFromFirstBySize(groups);
	
	//// Выкидываем группы, которые включают в себя другие группы
	GroupsRemoveIncluded(groups);

	//// сливаем пересекающиеся группы
	GroupsMergeIntersects(groups);
	//// ищем номера
	std::vector<SNFoundNumber> nums;
	
	//nums = SearchNumber(img_to_rez, groups, gray_image, stat_data);

	return nums;
}
//------------------------------------------------------

SNFigure FindFigure(const SNFigureGroup& gr, const pair_int& pos)
{
	for (size_t nn = 0; nn < gr.size(); ++nn)
	{
		if (gr[nn].top_left() <= pos
			&& gr[nn].bottom_right() >= pos)
		{
			return gr[nn];
		}
	}
	return SNFigure();
}
//------------------------------------------------------

std::vector<SNFoundSymbol> SNMasterRecognizer::FigsSearchSyms(const std::vector<pair_int>& pis, const pair_int& pos_center, const SNFigureGroup& cur_gr, const cv::Mat& original, SNNumberData& stat_data)
{
	//	draw_figures( cur_gr, etal );
	std::vector<SNFoundSymbol> ret;
	std::set<SNFigure> procs_figs;
	assert(pis.size() == 6);
	pair_int prev_pos = pos_center;
	for (size_t kk = 0; kk < pis.size(); ++kk)
	{
		const pair_int next_pos = prev_pos + pis.at(kk);
		//		draw_point( next_pos, etal );
		SNFoundSymbol next;
		next.fig = FindFigure(cur_gr, next_pos);
		if (!next.fig.IsEmpty()
			&& procs_figs.find(next.fig) == procs_figs.end())
		{
			procs_figs.insert(next.fig);
			const std::pair< char, double > cc = FindSymNN(kk >= 1 && kk <= 3, next.fig, original, stat_data);
			assert(cc.first != 0);
			next.pos_in_pis_index = kk;
			next.symbol = cc.first;
			next.weight = cc.second;
			ret.push_back(next);
			prev_pos = next.fig.center();
		}
		else
		{
			prev_pos = next_pos;
		}
	}
	return ret;
}

std::pair<char, double> SNMasterRecognizer::FindSymNN(bool num, const SNFigure& fig, const cv::Mat& original, SNNumberData& stat_data)
{
	const int left = fig.left();
	assert(left >= 0);
	int right = left + fig.Width() + 1;
	if (right >= original.cols)
	{
		right = original.cols - 1;
	}

	const int top = fig.top();
	assert(top >= 0);
	int bottom = top + fig.Height() + 1;
	if (bottom >= original.rows)
	{
		bottom = original.rows - 1;
	}

	const cv::Rect sym_border(left, top, right - left, bottom - top);

	if (stat_data.CacheOrigin != &original)
	{
		stat_data.CacheRets.clear();
		stat_data.CacheOrigin = &original;
	}

	std::map<std::pair<cv::Rect, bool>, std::pair< char, double >  >::const_iterator it = stat_data.CacheRets.find(std::make_pair(sym_border, num));
	if (it != stat_data.CacheRets.end())
	{
		return it->second;
	}

	cv::Mat mm(original.clone(), sym_border);
	std::pair<char, double>  ret = num ? ProcNum(mm) : ProcChar(mm);
	stat_data.CacheRets[std::make_pair(sym_border, num)] = ret;
	return ret;
}

std::pair<char, double> SNMasterRecognizer::ProcChar(const cv::Mat& input)
{
	return std::pair<char, double>();
	//return proc_impl(input, aux::mlp_char, &index_to_char_char);
}

std::pair<char, double> SNMasterRecognizer::ProcNum(const cv::Mat& input)
{
	return std::pair<char, double>();
	//return proc_impl(input, aux::mlp_num, &index_to_char_num);
}

SNFoundNumber SNMasterRecognizer::CreateNumberByPos(const std::vector<pair_int>& pis, const std::vector<SNFoundSymbol>& figs_by_pos)
{
	assert(pis.size() == 6);
	SNFoundNumber ret;
	for (size_t oo = 0; oo < pis.size(); ++oo)
	{
		bool found = false;
		for (size_t kk = 0; kk < figs_by_pos.size(); ++kk)
		{
			if (oo == static_cast<size_t>(figs_by_pos[kk].pos_in_pis_index))
			{
				const SNFoundSymbol & cur_sym = figs_by_pos[kk];
				ret.Number += cur_sym.symbol;
				ret.Weight += static_cast<int>(cur_sym.weight);
				ret.Figures.push_back(cur_sym.fig);
				found = true;
				break;
			}
		}
		if (found == false)
		{
			ret.Number += "?";
		}
	}
	return ret;
}

std::vector<SNFoundNumber> SNMasterRecognizer::SearchNumber(cv::Mat& etal, std::vector<SNFigureGroup>& groups, const cv::Mat& original, SNNumberData& stat_data)
{
	// ищем позиции фигур и соответсвующие им символы
	std::vector<SNFoundNumber> ret;

	for (size_t nn = 0; nn < groups.size(); ++nn)
	{
		std::vector<SNFoundNumber> fig_nums_sums;
		const SNFigureGroup& cur_gr = groups[nn];

		// перебираем фигуры, подставляя их на разные места (пока перебираем только фигуры 0-1-2)
		for (size_t mm = 0; mm < std::min(cur_gr.size(), size_t(2)); ++mm)
		{
			const SNFigure & cur_fig = cur_gr[mm];
			const pair_int cen = cur_fig.center();
			// подставляем текущую фигуру на все позиции (пока ставим только первую и вторую фигуру)
			for (int ll = 0; ll < 1; ++ll)
			{
				// меняем угол наклона номера относительно нас (0 - смотрим прям на номер, если угол меньше 0, то определяем номер с 2-х значным регионом)
				for (int oo = -60; oo < 50; oo += 10)
				{
					const std::vector< pair_int > pis = CalcSymCenters(ll, oo, cur_fig.Height());
					assert(pis.size() == 6); // всегда 6 символов без региона в номере
					const std::vector<SNFoundSymbol> figs_by_pos = FigsSearchSyms(pis, cen, cur_gr, original, stat_data);
					const SNFoundNumber number_sum = CreateNumberByPos(pis, figs_by_pos);
					fig_nums_sums.push_back(number_sum);
				}
			}
		}

		// выбираем лучшее
		if (!fig_nums_sums.empty())
		{
			ret.push_back(FindBestNumberByWeight(fig_nums_sums, &etal));
		}
	}

	// отрисовываем выбранные группы
	/*	for ( size_t nn = 0; nn < groups.size(); ++nn )
	{
	Mat colored_rect( etal.size(), CV_8UC3 );
	cvtColor( etal, colored_rect, CV_GRAY2RGB );
	for ( int mm = 0; mm < groups.at( nn ).size(); ++mm )
	{
	const figure* cur_fig = &groups.at( nn ).at( mm );
	rectangle( colored_rect, Point( cur_fig->left(), cur_fig->top() ), Point( cur_fig->right(), cur_fig->bottom() ), CV_RGB( 0, 255, 0 ) );
	}
	imwrite( next_name( "d" ), colored_rect );
	recog_debug->out_image( colored_rect );
	}*/
	return ret;
}

SNFoundNumber SNMasterRecognizer::FindBestNumberByWeight(const std::vector<SNFoundNumber>& vals, const cv::Mat* etal)
{
	(void)etal;
	assert(!vals.empty());
	if (vals.empty())
		return SNFoundNumber();

	int best_index = 0;
	for (size_t nn = 1; nn < vals.size(); ++nn)
	{
		if (vals[best_index] < vals[nn])
		{
			best_index = nn;
		}
		/*		static int tt = 0;
		cout << tt << " " << vals[ best_index ].number << " " << vals[ best_index ].weight << endl;
		if ( etal )
		{
		Mat colored_rect( etal->size(), CV_8UC3 );
		cvtColor( *etal, colored_rect, CV_GRAY2RGB );
		for ( size_t nn = 0; nn < vals[ best_index ].figs.size(); ++nn )
		{
		const figure* cur_fig = &vals[ best_index ].figs.at( nn );
		rectangle( colored_rect, Point( cur_fig->left(), cur_fig->top() ), Point( cur_fig->right(), cur_fig->bottom() ), CV_RGB( 0, 255, 0 ) );
		}
		imwrite( next_name( "fig" ), colored_rect );
		}
		tt++;*/
	}
	return vals[best_index];
}

// Рассчитываем центры символов
std::vector<pair_int> SNMasterRecognizer::CalcSymCenters(int index, int angle, int first_fig_height)
{
	const double angl_grad = static_cast<double>(std::abs(angle < 0 ? angle + 10 : angle)) * 3.14 / 180.;
	const double koef = static_cast<double>(first_fig_height)* cos(angl_grad);
	const double move_by_y = koef * 0.2;
	double move_by_x = 0.;

	std::vector<pair_int> etalons;
	if (angle >= 0)
	{
		move_by_x = koef * 0.96;
	}
	else
	{
		move_by_x = koef * 1.06;
	}
	
	etalons.push_back(std::make_pair(static_cast<int>(1. * move_by_x), -1 * static_cast<int>(move_by_y)));
	etalons.push_back(std::make_pair(static_cast<int>(1. * move_by_x), -1 * static_cast<int>(move_by_y)));
	etalons.push_back(std::make_pair(static_cast<int>(1. * move_by_x), -1 * static_cast<int>(move_by_y)));
	etalons.push_back(std::make_pair(static_cast<int>(1. * move_by_x), 0));
	etalons.push_back(std::make_pair(static_cast<int>(1. * move_by_x), 0));

	std::vector<pair_int> ret;
	const pair_int ret_front(std::make_pair(0, 0));
	ret.push_back(ret_front);

	for (int nn = index - 1; nn >= 0; --nn)
	{
		ret.insert(ret.begin(), ret_front - etalons.at(nn));
	}

	for (int nn = index; nn < 6 - 1; ++nn)
	{
		ret.push_back(ret_front + etalons.at(nn));
	}
	return ret;
}


template< class T >
std::set<T> to_set(const std::vector<T>& in)
{
	std::set< T > out;
	for (size_t nn = 0; nn < in.size(); ++nn)
	{
		out.insert(in.at(nn));
	}
	return out;
}
//------------------------------------------------------

// сливаем пересекающиеся группы
void SNMasterRecognizer::GroupsMergeIntersects(std::vector<SNFigureGroup> & groups)
{
	bool merge_was = true;
	while (merge_was)
	{
		merge_was = false;
		for (size_t nn = 0; nn < groups.size() && !merge_was; ++nn)
		{
			const std::set<SNFigure> s_cur_f = to_set(groups[nn]);
			for (size_t mm = 0; mm < groups.size() && !merge_was; ++mm)
			{
				if (nn != mm)
				{
					std::set<SNFigure> s_test_f = to_set(groups[mm]);

					if (find_first_of(s_test_f.begin(), s_test_f.end(), s_cur_f.begin(), s_cur_f.end()) != s_test_f.end())
					{
						// нашли пересечение
						std::set<SNFigure> ss_nn = to_set(groups[nn]);
						const std::set<SNFigure> ss_mm = to_set(groups[mm]);
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
void SNMasterRecognizer::GroupsRemoveIncluded(std::vector<SNFigureGroup> & groups)
{
	// todo: переписать постоянное создание set
	bool merge_was = true;
	while (merge_was)
	{
		merge_was = false;
		for (size_t nn = 0; nn < groups.size() && !merge_was; ++nn)
		{
			const std::set<SNFigure> s_cur_f = to_set(groups[nn]);

			for (size_t mm = 0; mm < groups.size() && !merge_was; mm++)
			{
				if (nn != mm)
				{
					const std::set<SNFigure> s_test_f = to_set(groups[mm]);
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

void SNMasterRecognizer::FigsRemoveInvalidFromFirstBySize(std::vector<SNFigureGroup> & groups)
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

// выкидываем элементы, что выходят за размеры номера, предпологаем что номер не выше 3 * высота первого элемента
void SNMasterRecognizer::FigsRemoveTooLongByYFromFirst(std::vector<SNFigureGroup>& groups)
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


void SNMasterRecognizer::FigsRemoveTooLongByXFromFirst(std::vector<SNFigureGroup>& groups)
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

void SNMasterRecognizer::GroupsRemoveTooSmall(std::vector<SNFigureGroup>& groups)
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

std::vector<SNFigure> SNMasterRecognizer::ParseToFigures(int stats_data_index, cv::Mat& mat)
{
	//	time_mesure to_fig( "TO_FIGS: " );

	std::vector<SNFigure> ret;
	
	ret.reserve(1000);
	
	for (int nn = 0; nn < mat.rows; ++nn)
	{
		for (int mm = 0; mm < mat.cols; ++mm)
		{
			if (mat.at<unsigned char>(nn, mm) == 0)
			{
				SNFigure fig_to_create;
				AddPixelAsSpy(stats_data_index, nn, mm, mat, fig_to_create);

				if (fig_to_create.is_too_big())
				{
					// проверяем что высота больше ширины
					if (fig_to_create.Width() < fig_to_create.Height())
					{
						if (fig_to_create.Width() > 4)
						{
							if (fig_to_create.Height() / fig_to_create.Width() < 4)
							{
								ret.push_back(fig_to_create);
							}
						}
					}
				}
			}
		}
	}

	// отрисовываем найденные фигуры
	if (!ret.empty())
	{
		cv::Mat colored_mat(mat.size(), CV_8UC3);
		cvtColor(mat, colored_mat, CV_GRAY2RGB);
		for (size_t nn = 0; nn < ret.size(); ++nn)
		{    
			cv::rectangle(colored_mat, cv::Point(ret[nn].left(), ret[nn].top()), cv::Point(ret[nn].right(), ret[nn].bottom()), CV_RGB(0, 255, 0));
		}

		int r = 0;
	}
	sort(ret.begin(), ret.end(), FlgLessLeft);
	return ret;
}
//------------------------------------------------------

// выбираем пиксели что бы получить контур, ограниченный белыми пикселями
void SNMasterRecognizer::AddPixelAsSpy(int stat_data_index, int row, int col, cv::Mat& mat, SNFigure& fig, int top_border, int bottom_border)
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

	PointsCount[stat_data_index] = 0;
	PointsDiplicateFirst[stat_data_index][PointsCount[stat_data_index]] = row;
	PointsDiplicateSecond[stat_data_index][PointsCount[stat_data_index]++] = col;

	// что бы не зациклилось
	mat.ptr< unsigned char >(row)[col] = 255;
	int cur_index = 0;
	while (cur_index < PointsCount[stat_data_index])
	{
		const int& cur_nn = PointsDiplicateFirst[stat_data_index][cur_index];
		const int& cur_mm = PointsDiplicateSecond[stat_data_index][cur_index];

		for (size_t yy = 0; yy < sizeof(PixAround[0]) / sizeof(PixAround[0][0]); ++yy)
		{
			const int &curr_pix_first = PixAround[stat_data_index][yy].first + cur_nn;
			const int &curr_pix_second = PixAround[stat_data_index][yy].second + cur_mm;
			if (curr_pix_first >= top_border && curr_pix_first < bottom_border
				&& curr_pix_second >= 0 && curr_pix_second < mat.cols)
			{
				if (mat.ptr< unsigned char >(curr_pix_first)[curr_pix_second] == 0)
				{
					PointsDiplicateFirst[stat_data_index][PointsCount[stat_data_index]] = curr_pix_first;
					PointsDiplicateSecond[stat_data_index][PointsCount[stat_data_index]++] = curr_pix_second;
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

// надо сделать какую-то пропорцию для зависимости угла от расстояния
bool SNMasterRecognizer::AngleIsEqual(int an1, int an2)
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

bool FlgLessLeft(const SNFigure& lf, const SNFigure& rf)
{
	return lf.left() < rf.left();
}
//------------------------------------------------------

// по отдельным фигурам создаем группы фигур
std::vector<SNFigureGroup> SNMasterRecognizer::MakeGroups(std::vector<SNFigure>& figs)
{
	using namespace std;
	std::vector<SNFigureGroup> groups;

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
				sort(cur_fig_groups[mm].second.begin(), cur_fig_groups[mm].second.end(), FlgLessLeft);
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
	return groups;
}

int SNMasterRecognizer::FindBestIndex(const std::vector<SNFoundNumber>& found_nums)
{
	if (found_nums.empty())
		return -1;
	else if (found_nums.size() == 1U)
		return 0;
	int ret = 0;
	for (size_t nn = 1; nn < found_nums.size(); ++nn)
	{
		if (found_nums.at(ret) < found_nums.at(nn))
		{
			ret = nn;
		}
	}
	return ret;
}

bool NotInCharDistance(int val)
{
	return val <= 0 || val >= 255;
}

void SNMasterRecognizer::FillReg(std::vector<std::vector<pair_doub> >& data, double x1, double y1, double x2, double y2, double x3, double y3)
{
	std::vector< pair_doub > tt;
	tt.push_back(std::make_pair(x1, y1));
	tt.push_back(std::make_pair(x2, y2));
	tt.push_back(std::make_pair(x3, y3));
	data.push_back(tt);
}

void SNMasterRecognizer::FillReg(std::vector<std::vector<pair_doub>>& data, double x1, double y1, double x2, double y2)
{
	std::vector< pair_doub > tt;
	tt.push_back(std::make_pair(x1, y1));
	tt.push_back(std::make_pair(x2, y2));
	data.push_back(tt);
}

std::vector< std::vector< pair_doub > > SNMasterRecognizer::Get2SymRegKoef(const std::vector<SNFigure>& figs, double avarage_height, double sin_avarage_angle_by_y)
{
	std::vector<std::vector<pair_doub>> ret;
	FillReg(ret, 6.7656, -0.4219, 7.5363, -0.3998);
	FillReg(ret, 5.6061, -0.2560, 6.3767, -0.2338);
	FillReg(ret, 4.6016, -0.2991, 5.3721, -0.2769);
	FillReg(ret, 3.5733, -0.3102, 4.3440, -0.2880);
	FillReg(ret, 2.4332, -0.4802, 3.2039, -0.4580);
	FillReg(ret, 1.4064, -0.5123, 2.1770, -0.4901);
	ApplyAngle(figs, ret, avarage_height, sin_avarage_angle_by_y);
	return ret;
}

std::vector<std::vector< pair_doub > > SNMasterRecognizer::Get3SymRegKoef(const std::vector<SNFigure>& figs, double avarage_height, double sin_avarage_angle_by_y)
{
	std::vector< std::vector< pair_doub > > ret;
	FillReg(ret, 5.8903, -0.3631, 6.6165, -0.3631, 7.3426, -0.3631);
	FillReg(ret, 4.9220, -0.2017, 5.6482, -0.2017, 6.3744, -0.2017);
	FillReg(ret, 3.9537, -0.2421, 4.6799, -0.2421, 5.4061, -0.2421);
	FillReg(ret, 2.9855, -0.2421, 3.7117, -0.2421, 4.4379, -0.2421);
	FillReg(ret, 2.0172, -0.4034, 2.7434, -0.4034, 3.4696, -0.4034);
	FillReg(ret, 1.0490, -0.4034, 1.7752, -0.4034, 2.5013, -0.4034);
	ApplyAngle(figs, ret, avarage_height, sin_avarage_angle_by_y);
	return ret;
}


void SNMasterRecognizer::ApplyAngle(const std::vector<SNFigure>& figs, std::vector<std::vector< pair_doub > >& data, double avarage_height, double sin_avarage_angle_by_y)
{
	// перемножаем все коэффициенты что бы получить реальное значение смещения в пикселях
	const double diff_by_x_2_sym = data[0][0].first - data[data.size() - 1][0].first;
	const double koef_by_x_2_sym = ((figs.at(figs.size() - 1).center().first - figs.at(0).center().first) / avarage_height) / diff_by_x_2_sym;
	for (size_t nn = 0; nn < data.size(); ++nn)
	{
		for (size_t mm = 0; mm < data[nn].size(); ++mm)
		{
			data[nn][mm].first = data[nn][mm].first * avarage_height * koef_by_x_2_sym;
			const double angle_offset = sin_avarage_angle_by_y * data[nn][mm].first;
			data[nn][mm].second = data[nn][mm].second * avarage_height + angle_offset;
		}
	}
}

// ищем ближайшую черную точку
pair_int SNMasterRecognizer::SearchNearestBlack(const cv::Mat& etal, const pair_int& center)
{
	pair_int dist_x(center.first, center.first);
	pair_int dist_y(center.second, center.second);
	for (;;)
	{
		for (int nn = dist_x.first; nn <= dist_x.second; ++nn)
		{
			for (int mm = dist_y.first; mm <= dist_y.second; ++mm)
			{
				const unsigned char cc = etal.at< unsigned char >(mm, nn);
				if (cc != 255)
				{
					assert(cc == 0);
					return pair_int(nn, mm);
				}
			}
		}
		--dist_x.first;
		++dist_x.second;
		--dist_y.first;
		++dist_y.second;
		if (dist_x.first < 0 || dist_x.second >= etal.cols
			|| dist_y.first < 0 || dist_y.second >= etal.rows)
		{
			return pair_int(-1, -1);
		}
	}
}

void SNMasterRecognizer::SearchRegionSymbol(int stat_data_index, SNFoundNumber& number, const cv::Mat& etal, const cv::Mat& origin, const pair_int& reg_center, const double avarage_height, bool last_symbol, SNNumberData& stat_data)
{
	//	number.m_figs.push_back( figure( reg_center, make_pair( 1, 1 ) ) );
	const pair_int nearest_black = SearchNearestBlack(etal, reg_center);
	//	number.m_figs.push_back( figure( nearest_black, make_pair( 1, 1 ) ) );
	//	return;

	if (nearest_black.first != -1
		&& nearest_black.second != -1)
	{
		SNFigure top_border_fig;
		SNFigure conture_fig;
		cv::Mat to_search = etal.clone();
		// Ищем контуры по верхней границе
		AddPixelAsSpy(stat_data_index, nearest_black.second, nearest_black.first, to_search, top_border_fig, -1, nearest_black.second + 1);

		if (top_border_fig.top() > reg_center.second - static_cast<int>(avarage_height)) // ушли не далее чем на одну фигуру
		{
			cv::Mat to_contur = etal.clone();
			AddPixelAsSpy(stat_data_index, nearest_black.second, nearest_black.first, to_contur, conture_fig, -1, top_border_fig.top() + static_cast<int>(avarage_height)+1);
			if (conture_fig.Width() >= static_cast<int>(avarage_height)) // если широкая фигура, то скорее всего захватили рамку
			{
				// центрируем фигуру по Х (вырезаем не большой кусок и определяем его центр)
				cv::Mat to_stable = etal.clone();
				SNFigure short_fig;
				// если центр сильно уехал, этот фокус не сработает, т.к. мы опять захватим рамку
				AddPixelAsSpy(stat_data_index, nearest_black.second, nearest_black.first, to_stable, short_fig, nearest_black.second - static_cast<int>(avarage_height * 0.4), nearest_black.second + static_cast<int>(avarage_height * 0.4));
				conture_fig = SNFigure(pair_int(short_fig.center().first, reg_center.second), pair_int(static_cast<int>(avarage_height * 0.60), static_cast<int>(avarage_height)));
			}
			else if (last_symbol && conture_fig.Width() >= static_cast<int>(avarage_height * 0.80)) // если последняя фигура, то могли захватить болтик черный
			{
				// Тут делаю обработку захвата болтика
				conture_fig = SNFigure(conture_fig.left(), conture_fig.left() + static_cast<int>(avarage_height * 0.60), conture_fig.top(), conture_fig.bottom());
			}
		}
		else
		{
			// центрируем фигуру по Х (вырезаем не большой кусок и определяем его центр)
			cv::Mat to_stable = etal.clone();
			SNFigure short_fig;
			// если центр сильно уехал, этот фокус не сработает, т.к. мы опять захватим рамку
			AddPixelAsSpy(stat_data_index, nearest_black.second, nearest_black.first, to_stable, short_fig, nearest_black.second - static_cast<int>(avarage_height * 0.4), nearest_black.second + static_cast<int>(avarage_height * 0.4));
			if (!short_fig.IsEmpty())
			{
				conture_fig = SNFigure(pair_int(short_fig.center().first, reg_center.second), pair_int(static_cast<int>(avarage_height * 0.60), static_cast<int>(avarage_height)));
			}
			else
			{
				conture_fig = SNFigure();
			}
		}

		if (!conture_fig.IsEmpty()
			&& find(number.Figures.begin(), number.Figures.end(), conture_fig) == number.Figures.end()
			&& static_cast<double>(conture_fig.Height()) > 0.5 * avarage_height
			&& static_cast<double>(conture_fig.Width()) > 0.2 * avarage_height
			)
		{
			const std::pair< char, double > sym_sym = FindSymNN(true, conture_fig, origin, stat_data);
			number.Figures.push_back(conture_fig);
			number.Number += sym_sym.first;
			number.Weight += static_cast<int>(sym_sym.second);
		}
		else
		{
			number.Number += '?';
		}
	}
	else
	{
		number.Number += '?';
	}
}

pair_int SNMasterRecognizer::CalcCenter(const std::vector<SNFigure>& figs, const std::vector< std::vector< pair_doub > >& data, int index)
{
	const static size_t figs_size = 6;
	assert(figs.size() >= figs_size);
	pair_int ret(0, 0);
	for (size_t nn = 0; nn < figs_size; ++nn)
	{
		ret = ret + figs.at(nn).center() + pair_int(static_cast<int>(data[nn][index].first), static_cast<int>(data[nn][index].second));
	}
	ret = ret / figs_size;
	return ret;
}

pair_int SNMasterRecognizer::GetPosNextInRegion(const std::vector<SNFigure>& figs, const std::vector< std::vector< pair_doub > >& move_reg, const int index, const SNFoundNumber& number, const double avarage_height)
{
	pair_int ret = CalcCenter(figs, move_reg, index);
	if (number.Number.at(number.Number.size() - 1) != '?')
	{
		return number.Figures.at(number.Figures.size() - 1).center() + std::make_pair(static_cast<int>(0.75 * avarage_height), 0);
	}
	return ret;
}

bool SNMasterRecognizer::CompareRegions(const SNFoundNumber& lh, const SNFoundNumber& rh)
{
	const bool lh_in_reg_list = RegionCodes.find(lh.Number) != RegionCodes.end();
	const bool rh_in_reg_list = RegionCodes.find(rh.Number) != RegionCodes.end();
	if (lh_in_reg_list != rh_in_reg_list)
	{
		return rh_in_reg_list;
	}
	else
	{
		const int cnp = lh.count_not_parsed_syms();
		const int cnp_other = rh.count_not_parsed_syms();
		if (cnp != cnp_other)
		{
			return cnp > cnp_other;
		}
		else
		{
			return lh.Weight < rh.Weight;
		}
	}
}

void SNMasterRecognizer::SearchRegion(int stats_data_index, SNFoundNumber& best_number, const int best_level, const cv::Mat& original, SNNumberData& stat_data)
{
	const std::vector<SNFigure>& figs = best_number.Figures;

	if (figs.size() != 6) // ищем регион только если у нас есть все 6 символов
	{
		return;
	}

	const double koef_height = 0.76; // отношение высоты буквы к высоте цифры
	// средняя высота буквы
	const double avarage_height = (static_cast< double >(figs.at(0).Height() + figs.at(4).Height() + figs.at(5).Height()) / 3.
		+ static_cast< double >(figs.at(1).Height() + figs.at(2).Height() + figs.at(3).Height()) * koef_height / 3.) / 2.;

	// ищем угол наклона по высоте (считаем среднее между не соседними фигурами 0-4, 0-5, 1-3)
	std::vector<pair_int> index_fig_to_angle;
	index_fig_to_angle.push_back(std::make_pair(0, 4));
	index_fig_to_angle.push_back(std::make_pair(0, 5));
	index_fig_to_angle.push_back(std::make_pair(1, 3));

	double avarage_angle_by_y = 0.; // средний угол наклона номера по оси У
	int move_by_y = 0; // номер наклонен вверх или вниз
	for (size_t nn = 0; nn < index_fig_to_angle.size(); ++nn)
	{
		const pair_int& cur_in = index_fig_to_angle.at(nn);
		avarage_angle_by_y += atan(static_cast< double >(figs[cur_in.first].center().second - figs[cur_in.second].center().second) / static_cast< double >(figs[cur_in.first].center().first - figs[cur_in.second].center().first));
		move_by_y += figs[cur_in.first].center().second - figs[cur_in.second].center().second;
	}

	avarage_angle_by_y = avarage_angle_by_y / static_cast< double >(index_fig_to_angle.size());
	const double sin_avarage_angle_by_y = sin(avarage_angle_by_y);

	std::vector<int> levels_to_iterate;
	for (int nn = best_level - 20; nn <= best_level + 20; nn += 10)
	{
		levels_to_iterate.push_back(nn);
	}

	/*auto not_in_char_distance = [&](std::vector<int> v)
	{ 
		return v.i <= 0 || i >= 255;
	};*/
	
	levels_to_iterate.erase(remove_if(levels_to_iterate.begin(), levels_to_iterate.end(), &NotInCharDistance), levels_to_iterate.end());
	//	levels_to_iterate.push_back( best_level + 10 );

	std::vector<SNFoundNumber> nums;

	for (size_t nn = 0; nn < levels_to_iterate.size(); ++nn)
	{
		cv::Mat etal = original > levels_to_iterate.at(nn);
		{
			SNFoundNumber fs2;
			const std::vector< std::vector< pair_doub > > move_reg_by_2_sym_reg(Get2SymRegKoef(figs, avarage_height, sin_avarage_angle_by_y));

			SearchRegionSymbol(stats_data_index, fs2, etal, original, CalcCenter(figs, move_reg_by_2_sym_reg, 0), avarage_height, false, stat_data);
			const pair_int next_2 = GetPosNextInRegion(figs, move_reg_by_2_sym_reg, 1, fs2, avarage_height);
			SearchRegionSymbol(stats_data_index, fs2, etal, original, next_2, avarage_height, true, stat_data);
			nums.push_back(fs2);
		}
		{
			SNFoundNumber fs3;
			const std::vector< std::vector< pair_doub > > move_reg_by_3_sym_reg(Get3SymRegKoef(figs, avarage_height, sin_avarage_angle_by_y));
			SearchRegionSymbol(stats_data_index, fs3, etal, original, CalcCenter(figs, move_reg_by_3_sym_reg, 0), avarage_height, false, stat_data);
			const pair_int next_2 = GetPosNextInRegion(figs, move_reg_by_3_sym_reg, 1, fs3, avarage_height);

			/*			pair_int next_2 = calc_center( figs, move_reg_by_3_sym_reg, 1 );
			if ( fs3.m_number.at( fs3.m_number.size() - 1 ) != '?' )
			{
			next_2 = fs3.m_figs.at( fs3.m_figs.size() - 1 ).center() + make_pair( static_cast< int >( 0.75 * avarage_height ), 0 );
			}*/
			
			SearchRegionSymbol(stats_data_index, fs3, etal, original, next_2, avarage_height, false, stat_data);
			const pair_int next_3 = GetPosNextInRegion(figs, move_reg_by_3_sym_reg, 2, fs3, avarage_height);
			
			/*			pair_int next_3 = calc_center( figs, move_reg_by_3_sym_reg, 2 );
			if ( fs3.m_number.at( fs3.m_number.size() - 1 ) != '?' )
			{
			next_3 = fs3.m_figs.at( fs3.m_figs.size() - 1 ).center() + make_pair( static_cast< int >( 0.75 * avarage_height ), 0 );
			}*/
			
			SearchRegionSymbol(stats_data_index, fs3, etal, original, next_3, avarage_height, true, stat_data);
			nums.push_back(fs3);
		}
	}

	//TODO: Uncomment!!!!

	/*std::sort(nums.begin(), nums.end(), &compare_regions);
	std::vector<SNFoundNumber>::const_iterator it = max_element(nums.begin(), nums.end(), &compare_regions);

	best_number.Number.append(it->Number);
	best_number.Weight += it->Weight;
	for (size_t nn = 0; nn < it->Figures.size(); ++nn)
	{
	best_number.Figures.push_back(it->Figures.at(nn));
	}*/
}

SNFoundNumber SNMasterRecognizer::ReadNumberLoop(const cv::Mat& input)
{
	SNFoundNumber ret;
	const int free_index = GetFreeIndex();
	try
	{
		SNNumberData stat_data;
		std::vector<SNFoundNumber> found_nums;

		for (size_t nn = 0; nn < SearchLevels.size(); ++nn)
		{
			std::vector<SNFoundNumber> cur_nums;

			switch (free_index)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				ReadNumberImpl(free_index, input, SearchLevels.at(nn), stat_data);
				break;
			default:
			{

			};
				//throw runtime_error("invalid data index");
			};

			if (cur_nums.empty())
			{
				found_nums.push_back(SNFoundNumber());
			}
			else
			{
				found_nums.push_back(cur_nums.at(FindBestIndex(cur_nums)));
			}
		}

		const int best_index = FindBestIndex(found_nums);
		if (best_index != -1)
		{
			SNFoundNumber& best_number = found_nums.at(best_index);
			const cv::Mat& gray_image = CreateGrayImage(input);
			const int& best_level = SearchLevels.at(best_index) - 10;
			switch (free_index)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			SearchRegion(best_index, best_number, best_level, gray_image, stat_data);
			break;
			default:
			{
				//throw runtime_error("invalid data index");
			}
			};

			ret = best_number;
		}
	}
	catch (const cv::Exception&)
	{
	}

	SetFreeIndex(free_index);
	return ret;
}
//------------------------------------------------------
