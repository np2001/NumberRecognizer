#ifndef SNMasterRecognizerStructs_h__
#define SNMasterRecognizerStructs_h__
//---------------------------------------------------
#include <vector>
//---------------------------------------------------
typedef std::pair<int, int> pair_int;
//---------------------------------------------------
class SNFigure
{
public:
	SNFigure()
	{
		Left = -1;
		Right = -1;
		Top = -1;
		Bottom = -1;
		TooBig = false;
	}

	SNFigure(pair_int center, pair_int size)
		: Left(center.first - size.first / 2 - 1)
		, Right(center.first + size.first / 2 + 1)
		, Top(center.second - size.second / 2 - 1)
		, Bottom(center.second + size.second / 2 + 1)
		, TooBig(false)
	{
	}

	SNFigure(int left, int right, int top, int bottom)
	{
		Left = left;
		Right = right;
		Top = top;
		Bottom = bottom;
	}

	int Width() const
	{
		return Right - Left;
	}

	int Height() const
	{
		return Bottom - Top;
	}

	bool IsEmpty() const
	{
		return Left == -1 && Right == -1 && Top == -1 && Bottom == -1;
	}

	void AddPoint(const pair_int& val)
	{
		if (IsTooBig())
			return;

		if (Left == -1 || Left > val.second)
		{
			Left = val.second;
		}

		if (Top == -1 || Top > val.first)
		{
			Top = val.first;
		}

		if (Right == -1 || Right < val.second)
		{
			Right = val.second;
		}

		if (Bottom == -1 || Bottom < val.first)
		{
			Bottom = val.first;
		}

		if (right() - left() > 50)
		{
			TooBig = true;
		}
	}

	pair_int center() const
	{
		const int hor = Left + (Right - Left) / 2;
		const int ver = Top + (Bottom - Top) / 2;
		return std::make_pair(hor, ver);
	}

	pair_int top_left() const
	{
		return std::make_pair(left(), top());
	}

	int left() const
	{
		return Left;
	}

	int top() const
	{
		return Top;
	}

	int right() const
	{
		return Right;
	}

	int bottom() const
	{
		return Bottom;
	}

	pair_int bottom_right() const
	{
		return std::make_pair(right(), bottom());
	}

	bool is_too_big() const
	{
		return !TooBig && !IsEmpty();
	}

	bool IsTooBig() const
	{
		return TooBig;
	}

	bool operator < (const SNFigure & other) const
	{
		if (Left != other.Left)
			return Left < other.Left;
		else if (Top != other.Top)
			return Top < other.Top;
		else if (Right != other.Right)
			return Right < other.Right;
		else if (Bottom != other.Bottom)
			return Bottom < other.Bottom;
		else
			return false;
	}

	bool operator==(const SNFigure & other) const
	{
		return Left == other.Left
			&& Right == other.Right
			&& Top == other.Top
			&& Bottom == other.Bottom;
	}

private:
	int Left;
	int Right;
	int Top;
	int Bottom;
	bool TooBig;
};
//---------------------------------------------------
typedef std::vector<SNFigure> SNFigureGroup;
typedef std::vector<SNFigureGroup> SNFigureGroups;
//---------------------------------------------------

struct SNFoundNumber
{
	SNFoundNumber()
	{
		Weight = -1;
	}

	std::string Number;		// номер
	int Weight;			// вес
	SNFigureGroup  Figures;

	std::pair<std::string, int> to_pair() const
	{
		return std::make_pair(Number, Weight);
	}

	bool operator < (const SNFoundNumber& other) const
	{
		const int cnp = count_not_parsed_syms();
		const int cnp_other = other.count_not_parsed_syms();
		if (cnp != cnp_other)
		{
			return cnp > cnp_other;
		}
		else
		{
			return Weight < other.Weight;
		}
	}

	bool is_valid() const
	{
		return Weight != -1 && !Number.empty();
	}

	int count_not_parsed_syms() const
	{
		if (Number.empty())
			return 100; // вообще ничего нет
		return count(Number.begin(), Number.end(), '?');
	}
};
//------------------------------------------------------

struct SNFoundSymbol
{
	SNFigure fig;
	size_t pos_in_pis_index;
	char symbol;
	double weight;
};
//------------------------------------------------------

struct SNNumberData
{
	SNNumberData()
	{
		CacheOrigin = 0;
	}

	~SNNumberData()
	{
	}

	const cv::Mat* CacheOrigin;
	std::map<std::pair<cv::Rect, bool>, std::pair<char, double>> CacheRets;

private:
	SNNumberData(const SNNumberData& other);
	SNNumberData& operator=(const SNNumberData& other);
};
//------------------------------------------------------
#endif // SNMasterRecognizerStructs_h__
