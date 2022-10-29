#pragma once

#include <string_view>

#include <cstring>
#include <iostream>

class LN
{
  public:
	unsigned int numberLength = 0;
	char* digits;
	char sign;

  private:
	unsigned int digitsLength = 16;

  public:
	explicit LN(long long cll = 0);
	explicit LN(const char* x);
	explicit LN(std::string_view s);
	LN(const LN& obj);
	LN(LN&& obj);

	~LN() { free(this->digits); }

  private:
	int comparable(const LN& b) const &;
	void clearLeadingZeroes();
	static LN doSubtract(const LN& x, const LN& y);

  public:
	LN operator+(const LN& b) const &;
	LN operator-(const LN& b) const &;
	LN operator-() const &;
	bool operator>(const LN& b) const &;
	bool operator<(const LN& b) const &;
	bool operator>=(const LN& b) const &;
	bool operator<=(const LN& b) const &;
	bool operator==(const LN& b) const &;
	bool operator!=(const LN& b) const &;
	explicit operator bool() const &;
	explicit operator long long() const &;
	LN& operator=(const LN& b);
	LN& operator=(LN&& b);
};

LN operator"" _ln(const char* x);