// VS22 C++20
// Библиотеки.

#include <iostream>
#include <sstream>

#include <string_view>
#include <string>

#include <vector>
#include <array>
#include <list>

#include <functional>
#include <algorithm>
#include <utility>

#include <cmath>


// Псевдонимы.


using std::exception;
using std::ignore;

using std::istringstream;
using std::string_view;
using std::vector;
using std::string;
using std::array;
using std::list;

using std::advance;
using std::getline;
using std::endl;
using std::move;
using std::cout;
using std::cin;

using std::sin;
using std::cos;
using std::tan;
using std::log;
using std::cbrt;
using std::asin;
using std::acos;
using std::atan;
using std::log2;
using std::log10;


using numeric_t = long double;
const auto & CONVERT = static_cast<numeric_t(*)(const string & _Str, size_t *)> (std::stold);


// Структуры данных.


struct MathFunc final
{
public:

	constexpr explicit MathFunc(string_view && name, numeric_t(*fp) (numeric_t)) noexcept : name_(move(name)), fp_(fp) {}

public:

	string_view name_;
	numeric_t(*fp_) (numeric_t) = nullptr;
};


// Наборы данных.


constexpr string_view EXIT_FLAG { "q" };


constexpr string_view SPACE { " " };
constexpr string_view OPEN_BRACKET { "(" };
constexpr string_view CLOSE_BRACKET { ")" };
constexpr string_view DEFAULT_OPERATION { "*" };


constexpr std::string_view BRACKETS("()");
constexpr std::string_view OPERATIONS("+-*/^%");
constexpr std::string_view PRIORITY_OPERATIONS("*/^%");
constexpr std::string_view NUMBERS("0123456789");

constexpr array MATH_FUNC
{
	MathFunc("cbrt", cbrt),

		MathFunc("sin", sin),
		MathFunc("asin", asin),

		MathFunc("cos", cos),
		MathFunc("acos", acos),

		MathFunc("tan", tan),
		MathFunc("atan", atan),

		MathFunc("log", log),
		MathFunc("log2", log2),
		MathFunc("log10", log10)
};


// Перегрузки.


template<class Ty_, class ostream_>
ostream_ & operator << (ostream_ & os, const Ty_ & obj)
{
	for (const auto & val : obj)
	{
		os << val << SPACE;
	}

	return os;
}


// Проверки.


constexpr bool IsMathFunc(const string & str) noexcept
{
	for (const auto & obj : MATH_FUNC)
	{
		if (obj.name_ == str)
		{
			return true;
		}
	}

	return false;
}

constexpr bool IsNum(const string & str)
{
	try
	{
		ignore = CONVERT(str, 0);
		return true;
	}
	catch (const exception &)
	{
		return false;
	}
}

constexpr bool IsBracket(const string & str) noexcept
{
	return str.size() == 1 && BRACKETS.find(str[0]) != string_view::npos ? true : false;
}

constexpr bool IsOp(const string & str) noexcept
{
	return str.size() == 1 && OPERATIONS.find(str[0]) != string_view::npos ? true : false;
}

constexpr bool IsPriorityOp(const string & str) noexcept
{
	return str.size() == 1 && PRIORITY_OPERATIONS.find(str[0]) != std::string_view::npos ? true : false;
}


// Основные функции.


list<string> StringToList(const string & str)
{
	list<string> list;

	if (!str.empty())
	{
		string guess;
		istringstream iss(str);

		while (iss >> guess)
		{
			list.push_back(guess);
		}
	}

	return list;
}

string ListToString(const list<string> & list)
{
	string newStr;

	for (const auto & str : list)
	{
		newStr += str;
		newStr += SPACE;
	}

	return newStr;
}

list<string> & CorrectionOfTokens(list<string> & equation)
{
	if (equation.empty())
	{
		for (auto current { equation.begin() }; current != equation.end(); )
		{
			if (!(IsNum(*current) || IsOp(*current) || IsBracket(*current) || IsMathFunc(*current)))
			{
				current = equation.erase(current);
			}
			else
			{
				++current;
			}
		}
	}

	return equation;
}

list<string> & CorrectionOfBrackets(list<string> & equation)
{
	if (equation.empty())
	{
		return equation;
	}

	size_t counterOB {};
	size_t counterCB {};

	auto NumberOfBrackets = [&equation](auto & iter, size_t & counter, const string_view & mainBracket, const string_view & otherBracket) -> void
	{
		while (iter != equation.end() && *iter != otherBracket)
		{
			if (*iter == mainBracket)
			{
				++counter;
			}

			++iter;
		}
	};


	for (auto current { equation.begin() }, offset { equation.begin() }; current != equation.end(); )
	{
		NumberOfBrackets(offset, counterOB, OPEN_BRACKET, CLOSE_BRACKET);
		NumberOfBrackets(offset, counterCB, CLOSE_BRACKET, OPEN_BRACKET);

		if (counterOB > counterCB)
		{
			while (counterOB != counterCB && current != offset)
			{
				if (*current == OPEN_BRACKET)
				{
					current = equation.erase(current);
					--counterOB;
				}
				else
				{
					++current;
				}
			}
		}
		else if (counterOB < counterCB)
		{
			while (counterOB != counterCB && current != offset)
			{
				if (*current == CLOSE_BRACKET)
				{
					current = equation.erase(current);
					--counterCB;
				}
				else
				{
					++current;
				}
			}
		}

		current = offset;
	}

	return equation;
}

list<string> & RemovingEmptyBrackets(list<string> & equation)
{
	if (equation.empty())
	{
		return equation;
	}

	size_t counterOB {};
	size_t counterCB {};


	for (auto left { equation.begin() }, right { equation.begin() }; right != equation.end(); )
	{
		if (*right == OPEN_BRACKET)
		{
			if (counterOB == 0)
			{
				left = right;
			}

			++counterOB;
		}
		else if (*right == CLOSE_BRACKET && counterOB)
		{
			++counterCB;
		}
		else
		{
			left = right;
			counterOB = counterCB = 0;
		}

		if (counterOB == counterCB && counterOB)
		{
			left = right = equation.erase(left, ++right); // Возвращает _Last, поэтому вместо До right (right), необходимо По right (++right).
			counterOB = counterCB = 0;
		}
		else
		{
			++right;
		}
	}

	return equation;
}

list<string> & CorrectionOfEquation(list<string> & equation)
{
	if (equation.empty())
	{
		return equation;
	}
	else if (equation.size() == 1 && !IsNum(*equation.begin()))
	{
		equation.clear();
		return equation;
	}


	size_t changeCounter { 1 };
	auto current { equation.begin() }, next { current };


	while (changeCounter)
	{
		current = next = equation.begin();
		advance(next, 1);
		changeCounter = 0;

		RemovingEmptyBrackets(equation);


	#ifdef _DEBUG
		cout << "RemovingEmptyBrackets(): " << equation << endl;
	#endif // _DEBUG


		while (next != equation.end())
		{
			// |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-| //
			if (IsMathFunc(*current))
			{
				// Правильный ввод: IsNum(*next); *next == OPEN_BRACKET.
				if (IsOp(*next))
				{
					next = equation.erase(next);
					++changeCounter;
				}
				else if (IsMathFunc(*next) || *next == CLOSE_BRACKET)
				{
					current = next = equation.erase(current);
					advance(next, 1);
					++changeCounter;
				}
			}
			// |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-| //
			else if (IsNum(*current))
			{
				// Правильный ввод: IsOp(*next); *next == CLOSE_BRACKET.
				if (IsNum(*next) || IsMathFunc(*next) || *next == OPEN_BRACKET)
				{
					next = equation.insert(next, DEFAULT_OPERATION.data());
					++changeCounter;
				}
			}
			// |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-| //
			else if (IsOp(*current))
			{
				// Правильный ввод: IsMathFunc(*next); IsNum(*next); *next == OPEN_BRACKET.
				if (IsOp(*next) || *next == CLOSE_BRACKET)
				{
					current = next = equation.erase(current);
					advance(next, 1);
					++changeCounter;
				}
			}
			// |-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-| //
			else if (*current == OPEN_BRACKET)
			{
				// Правильный ввод: IsMathFunc(*next); IsNum(*next); *next == OPEN_BRACKET.
				if (IsOp(*next))
				{
					current = next = equation.erase(next);
					advance(next, 1);
					++changeCounter;
				}
				else if (*next == CLOSE_BRACKET)
				{
					current = next = equation.erase(current, ++next);
					advance(next, 1);
					++changeCounter;
				}
			}
			else if (*current == CLOSE_BRACKET)
			{
				// Правильный ввод: *current == CLOSE_BRACKET; IsOp(*next).
				if (IsMathFunc(*next) || IsNum(*next) || *next == OPEN_BRACKET)
				{
					next = equation.insert(next, DEFAULT_OPERATION.data());
					++changeCounter;
				}
			}

			if (current == equation.end() || next == equation.end())
			{
				break;
			}

			++current;
			++next;
		}
	}


	changeCounter = 1;
	current = equation.end();


	while (changeCounter && current != equation.begin())
	{
		--current;

		if (IsOp(*current) || IsMathFunc(*current) || IsOp(*current) || *current == OPEN_BRACKET)
		{
			current = equation.erase(current);
			++changeCounter;
		}
		else
		{
			changeCounter = 0;
		}
	}


	return equation;
}


numeric_t ResultOfBinaryEquation(const numeric_t & lval, const numeric_t & rval, const char & operation)
{
	switch (operation)
	{
		case '+':
			return (lval + rval);
		case '-':
			return (lval - rval);
		case '/':
			return (lval / rval);
		case '*':
			return (lval * rval);
		case '^':
			return pow(lval, rval);
		case '%':
			return fmod(lval, rval);
		default:
			throw std::invalid_argument("Invalid operation sign.\n");
	}
}

numeric_t ResultLoop(istringstream & iss)
{
	static bool afterMathFunc { false };
	vector<numeric_t> nums;
	vector<string> ops;
	numeric_t res {};
	string guess;

#ifdef _DEBUG
	cout << "-----------------ResultLoop.start-----------------" << endl;
#endif // _DEBUG

	while (iss >> guess)
	{
		if (IsNum(guess))
		{
			if (afterMathFunc)
			{
				afterMathFunc = false;

			#ifdef _DEBUG
				cout << "afterMathFunc" << endl
					<< "return: " << guess << endl
					<< "nums: " << nums << endl
					<< "ops:  " << ops << endl
					<< "-----------------ResultLoop.end-------------------" << endl;
			#endif// _DEBUG

				return CONVERT(guess, 0);
			}

			nums.push_back(CONVERT(guess, 0));

		#ifdef _DEBUG
			cout << "push_back = " << guess << endl;
		#endif// _DEBUG
		}
		else if (IsOp(guess))
		{
			ops.push_back(guess);

		#ifdef _DEBUG
			cout << "push_back = " << guess << endl;
		#endif// _DEBUG
		}
		else if (IsMathFunc(guess))
		{
			afterMathFunc = true;

		#ifdef _DEBUG
			cout << "MathFunc = " << guess << endl
				<< "nums: " << nums << endl
				<< "ops:  " << ops << endl;
		#endif// _DEBUG

			nums.push_back(std::find_if(MATH_FUNC.begin(), MATH_FUNC.end(),
										[&guess](const auto & obj) { return (guess == obj.name_) ? true : false; })->fp_(ResultLoop(iss)));

		#ifdef _DEBUG
			cout << "push_back(math) = " << nums.back() << endl;
		#endif// _DEBUG
		}
		else if (guess == OPEN_BRACKET)
		{
			afterMathFunc = false;

		#ifdef _DEBUG
			cout << "OPEN_BRACKET" << endl;
		#endif// _DEBUG

			nums.push_back(ResultLoop(iss));
		}
		else if (guess == CLOSE_BRACKET)
		{
		#ifdef _DEBUG
			cout << "break" << endl;
		#endif// _DEBUG

			break;
		}
	}


#ifdef _DEBUG
	cout
		<< "nums: " << nums << endl
		<< "ops:  " << ops << endl;

#endif// _DEBUG


	auto Calculation = [&ops, &nums, &res](bool(*fp)(const string &))
	{
		auto num { nums.begin() };
		auto op { ops.begin() };

		while (num != nums.end() && op != ops.end())
		{
			if (fp(*op))
			{
				if ((num + 1) == nums.end())
				{
				#ifdef _DEBUG
					cout << "->" << res << "+" << *num;
				#endif // _DEBUG

					res += *num;

				#ifdef _DEBUG
					cout << "=" << res;
				#endif // _DEBUG

					nums.pop_back();
					break;
				}

			#ifdef _DEBUG
				cout << "-->" << *num << op[0][0] << *(num + 1);
			#endif // _DEBUG

				* num = ResultOfBinaryEquation(*num, *(num + 1), op[0][0]);

			#ifdef _DEBUG
				cout << "=" << *num << endl;
			#endif // _DEBUG

				nums.erase(num + 1);
				op = ops.erase(op);
			}
			else
			{
				++op;
				++num;
			}
		}

		if (nums.size() == 1 && ops.empty())
		{
			res += *nums.begin();
			nums.clear();
		}
	};


	Calculation(IsPriorityOp);
	Calculation(IsOp);


#ifdef _DEBUG
	cout << "RES: " << res << endl;
#endif // _DEBUG

#ifdef _DEBUG
	cout << "-----------------ResultLoop.end-------------------" << endl;
#endif // _DEBUG
	return res;
}

numeric_t Result(const list<string> & equation)
{
	istringstream iss(move(ListToString(equation)));
	return ResultLoop(iss);
}


// Главная функция.


int main()
{
	setlocale(LC_ALL, "ru_RU");

	string equation_string;
	list<string> equation_list;

#ifdef NDEBUG
	try
	{
		while (true)
		{
			cout << "Введите уравнение: ";
			getline(cin, equation_string);

			if (equation_string == EXIT_FLAG)
			{
				break;
			}

			equation_list = StringToList(equation_string);
			CorrectionOfTokens(equation_list);
			CorrectionOfBrackets(equation_list);

			cout << "Исправленное уравнение: "
				<< CorrectionOfEquation(equation_list) << endl;

			cout << "Результат: "
				<< Result(equation_list) << endl << endl;
		}
	}
	catch (const exception & e)
	{
		cout << e.what() << endl;
	}
#endif // NDEBUG


#ifdef _DEBUG

	cout << "DEBUG" << endl
		<< "equation_list           " << equation_list << endl
		<< "CorrectionOfTokens():   " << CorrectionOfTokens(equation_list) << endl
		<< "CorrectionOfBrackets(): " << CorrectionOfBrackets(equation_list) << endl

		<< endl << "| -> BEGIN <- |" << endl
		<< "CorrectionOfEquation(): " << endl << CorrectionOfEquation(equation_list) << endl
		<< "| ->  END  <- |" << endl << endl

		<< endl << "| -> BEGIN <- |" << endl
		<< "Result():               " << endl << Result(equation_list) << endl
		<< "| ->  END  <- |" << endl << endl;
#endif // _DEBUG
}