#include "LN.h"

LN::LN(const long long cll)
{
	if (cll == 0)
	{
		this->digits = (char *)malloc(sizeof(char) * 1);
		this->digits[0] = '0';
		this->numberLength = 1;
		this->sign = '+';
	}
	else
	{
		this->digits = (char *)malloc(sizeof(char) * this->digitsLength);
		if (!this->digits)
		{
			throw "MemoryError";
		}
		long long x1 = cll;
		if (x1 < 0)
		{
			this->sign = '-';
		}
		else
		{
			this->sign = '+';
		}
		int ind = 0;
		x1 = std::abs(x1);
		while (x1 != 0)
		{
			int digit = (int)x1 % 10;
			this->digits[ind] = (char)(digit + '0');
			ind++;
			x1 /= 10;
			if (ind == this->digitsLength)
			{
				// do reallocation
				char *digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * 2);
				unsigned int digitsLengthNew = this->digitsLength * 2;
				if (!digitsNew)
				{
					digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * (5 / 4));
					digitsLengthNew = this->digitsLength * (5 / 4);
					if (!digitsNew)
					{
						throw "MemoryError";
					}
				}
				this->digits = digitsNew;
				this->digitsLength = digitsLengthNew;
			}
		}
		this->numberLength = ind;
		clearLeadingZeroes();
	}
}

LN::LN(const char *x)
{
	this->digits = (char *)malloc(sizeof(char) * this->digitsLength);
	if (!this->digits)
	{
		throw "MemoryError";
	}
	int ind = 0;
	int xLength = (int)strlen(x);
	if (x[0] == '-')
	{
		this->sign = '-';
	}
	else
	{
		this->sign = '+';
	}
	for (int i = xLength - 1; i >= (this->sign == '-' ? 1 : 0); i--)
	{
		this->digits[ind] = x[i];
		ind++;
		if (ind == this->digitsLength)
		{
			// do reallocation
			char *digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * 2);
			unsigned int digitsLengthNew = this->digitsLength * 2;
			if (!digitsNew)
			{
				digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * (5 / 4));
				digitsLengthNew = this->digitsLength * (5 / 4);
				if (!digitsNew)
				{
					throw "MemoryError";
				}
			}
			this->digits = digitsNew;
			this->digitsLength = digitsLengthNew;
		}
	}
	this->numberLength = ind;
	clearLeadingZeroes();
}

LN::LN(const std::string_view s)
{
	this->digits = (char *)malloc(sizeof(char) * this->digitsLength);
	if (!this->digits)
	{
		throw "MemoryError";
	}
	int ind = 0;
	if (s[0] == '-')
	{
		this->sign = '-';
	}
	else
	{
		this->sign = '+';
	}
	for (int i = (int)s.length() - 1; i >= (this->sign == '-' ? 1 : 0); i--)
	{
		this->digits[ind] = s[i];
		ind++;
		if (ind == this->digitsLength)
		{
			// do reallocation
			char *digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * 2);
			unsigned int digitsLengthNew = this->digitsLength * 2;
			if (!digitsNew)
			{
				digitsNew = (char *)realloc(this->digits, sizeof(char) * this->digitsLength * (5 / 4));
				digitsLengthNew = this->digitsLength * (5 / 4);
				if (!digitsNew)
				{
					throw "MemoryError";
				}
			}
			this->digits = digitsNew;
			this->digitsLength = digitsLengthNew;
		}
	}
	this->numberLength = ind;
	clearLeadingZeroes();
}

LN::LN(const LN &obj)
{
	this->digits = (char *)malloc(sizeof(char) * obj.digitsLength);
	if (!this->digits)
	{
		throw "MemoryError";
	}
	for (int i = 0; i < obj.numberLength; i++)
	{
		this->digits[i] = obj.digits[i];
	}
	for (int i = (int)obj.numberLength; i < obj.digitsLength; i++)
	{
		this->digits[i] = '0';
	}
	this->sign = obj.sign;
	this->numberLength = obj.numberLength;
	this->digitsLength = obj.digitsLength;
}

LN::LN(LN &&obj)
{
	this->digits = (char *)malloc(sizeof(char) * obj.digitsLength);
	if (this->digits)
	{
		for (int i = 0; i < obj.numberLength; i++)
		{
			this->digits[i] = obj.digits[i];
		}
		for (int i = (int)obj.numberLength; i < obj.digitsLength; i++)
		{
			this->digits[i] = '0';
		}
		this->sign = obj.sign;
		this->numberLength = obj.numberLength;
		this->digitsLength = obj.digitsLength;
	}
	else
	{
		throw "MemoryError";
	}
	free(obj.digits);
	obj.digits = NULL;
	obj.digitsLength = 0;
	obj.numberLength = 0;
	obj.sign = '+';
}

LN LN::operator+(const LN &b) const &
{
	if (this->sign == b.sign)
	{
		int p = 0;
		unsigned int maxDigitsLength = std::max(this->numberLength, b.numberLength);
		unsigned int length = maxDigitsLength + 1 + (int)(this->sign == '-');
		char *resDigits = (char *)malloc(sizeof(char) * (length + 1));
		for (int i = 0; i < maxDigitsLength; i++)
		{
			int x = (i < this->numberLength ? this->digits[i] - '0' : 0);
			int y = (i < b.numberLength ? b.digits[i] - '0' : 0);
			int r = x + y + p;
			resDigits[i] = (char)((r % 10) + '0');
			p = r / 10;
		}
		resDigits[maxDigitsLength] = (char)(p + '0');
		if (this->sign == '-')
		{
			resDigits[maxDigitsLength + 1] = '-';
		}
		// reverse
		for (int i = 0; i < length / 2; i++)
		{
			char c = resDigits[i];
			resDigits[i] = resDigits[length - i - 1];
			resDigits[length - i - 1] = c;
		}
		resDigits[length] = 0;
		LN res(resDigits);
		free(resDigits);
		return res;
	}
	else
	{
		// res = max(this, b) - min(this, b)
		if (this->comparable(b) == 1)
		{
			// this is +, b is -
			LN b1(b);
			b1.sign = '+';
			return this->operator-(b1);
		}
		// this is -, b is +
		LN a1 = *this;
		a1.sign = '+';
		return b.operator-(a1);
	}
}

LN LN::operator-() const &
{
	if (this->comparable(LN{}) == 0)
	{
		return LN{};
	}
	LN res = *this;
	if (res.sign == '+')
	{
		res.sign = '-';
	}
	else
	{
		res.sign = '+';
	}
	return res;
}

LN LN::doSubtract(const LN &x, const LN &y)
{
	// x > y >= 0
	char *resDigits = (char *)malloc(sizeof(char) * (x.digitsLength + 1));
	int p = 0;
	for (int i = 0; i < x.numberLength; i++)
	{
		if (i >= y.numberLength)
		{
			int a = x.digits[i] - '0';
			a -= p;
			if (a < 0)
			{
				p = 1;
				a += 10;
			}
			else
			{
				p = 0;
			}
			resDigits[i] = (char)(a + '0');
		}
		else
		{
			int a = x.digits[i] - '0';
			a -= p;
			int b = y.digits[i] - '0';
			if (a < b)
			{
				a += 10;
				p = 1;
			}
			else
			{
				p = 0;
			}
			int c = a - b;
			resDigits[i] = (char)(c + '0');
		}
	}
	for (int i = (int)x.numberLength; i < x.digitsLength; i++)
	{
		resDigits[i] = '0';
	}
	// reverse
	for (int i = 0; i < x.digitsLength / 2; ++i)
	{
		char ch = resDigits[i];
		resDigits[i] = resDigits[x.digitsLength - i - 1];
		resDigits[x.digitsLength - i - 1] = ch;
	}
	resDigits[x.digitsLength] = 0;
	LN ret(resDigits);
	free(resDigits);
	return ret;
}

LN LN::operator-(const LN &b) const &
{
	// a > 0, b > 0 : a - b || a > b ? a - b : -(b - a) || 123 - 34 = 89 | ..9 ~ .89 || 34 - 123 = -(123 - 34) = -(89) =
	// -89
	if (this->sign == '+' && b.sign == '+')
	{
		if (this->comparable(b) == 1)
		{
			return doSubtract(*this, b);
		}
		if (this->comparable(b) == -1)
		{
			LN res = doSubtract(b, *this);
			return res.operator-();
		}
		return LN{};
	}
	// a > 0, b < 0 : a - b || a + negate(b) || 123 - (-34) = 123 + 34 = 157
	if (this->sign == '+' && b.sign == '-')
	{
		LN b1 = b.operator-();
		return this->operator+(b1);
	}
	// a < 0, b > 0 : a - b || negate(negate(a) + b) || -123 - 34 = - (123 + 34) = -157
	if (this->sign == '-' && b.sign == '+')
	{
		LN a1 = this->operator-();
		LN res = a1.operator+(b);
		return res.operator-();
	}
	// a < 0, b < 0 : a - b || a < b ? negate(negate(a) - negate(b)) : negate(b) - negate(a)  || -123 - (-34) = -(123 -
	// 34) = -89 || -34 - (-123) = 123 - 34 = 89
	if (this->comparable(b) == -1)
	{
		LN a1 = this->operator-();
		LN b1 = b.operator-();
		LN res = doSubtract(a1, b1);
		return res.operator-();
	}
	if (this->comparable(b) == 1)
	{
		LN a1 = this->operator-();
		LN b1 = b.operator-();
		LN res = doSubtract(b1, a1);
		return res;
	}
	return LN{};
}

void LN::clearLeadingZeroes()
{
	int indNotZeroDigit = -1;
	for (int i = 0; i < this->numberLength; i++)
	{
		if (this->digits[i] != '0')
		{
			indNotZeroDigit = i;
			break;
		}
	}
	if (indNotZeroDigit == -1)
	{
		// LN is zero!
		this->digits = (char *)malloc(sizeof(char) * 1);
		this->digits[0] = '0';
		this->numberLength = 1;
		this->sign = '+';
	}
	else
	{
		for (int i = indNotZeroDigit + 1; i < this->numberLength; i++)
		{
			if (this->digits[i] != '0')
			{
				indNotZeroDigit = i;
			}
		}
		if (indNotZeroDigit != this->numberLength - 1)
		{
			char *digitsNew = (char *)realloc(this->digits, sizeof(char) * (indNotZeroDigit + 1));
			if (!digitsNew)
			{
				throw "MemoryError";
			}
			this->digits = digitsNew;
			this->digitsLength = indNotZeroDigit + 1;
			this->numberLength = indNotZeroDigit + 1;
		}
	}
}

int LN::comparable(const LN &b) const &
{
	// 1 - this > b, 0 - this == b, -1 - this < b
	if (this->sign != b.sign)
	{
		if (this->sign == '+')
		{
			return 1;
		}
		return -1;
	}
	if (this->sign == '+')
	{
		if (this->numberLength > b.numberLength)
		{
			return 1;
		}
		if (this->numberLength < b.numberLength)
		{
			return -1;
		}
		for (int i = (int)this->numberLength - 1; i >= 0; i--)
		{
			if (this->digits[i] > b.digits[i])
			{
				return 1;
			}
			if (this->digits[i] < b.digits[i])
			{
				return -1;
			}
		}
		return 0;
	}
	if (this->numberLength > b.numberLength)
	{
		return -1;
	}
	if (this->numberLength < b.numberLength)
	{
		return 1;
	}
	for (int i = (int)this->numberLength - 1; i >= 0; i--)
	{
		if (this->digits[i] > b.digits[i])
		{
			return -1;
		}
		if (this->digits[i] < b.digits[i])
		{
			return 1;
		}
	}
	return 0;
}

bool LN::operator>(const LN &b) const &
{
	return this->comparable(b) == 1;
}

bool LN::operator<(const LN &b) const &
{
	return this->comparable(b) == -1;
}

bool LN::operator>=(const LN &b) const &
{
	return this->comparable(b) >= 0;
}

bool LN::operator<=(const LN &b) const &
{
	return this->comparable(b) <= 0;
}

bool LN::operator==(const LN &b) const &
{
	return this->comparable(b) == 0;
}

bool LN::operator!=(const LN &b) const &
{
	return this->comparable(b) != 0;
}

LN::operator bool() const &
{
	if (*this == LN{})
	{
		return false;
	}
	return true;
}

LN::operator long long() const &
{
	long long maxLong = 9223372036854775807;
	long long minLong = -maxLong - 1;
	LN maxLongLong(maxLong);
	LN minLongLong(minLong);
	if (*this > maxLongLong || *this < minLongLong)
	{
		printf("Here!\n");
		throw "CastError";
	}
	if (this->sign == '+')
	{
		long long r = this->digits[this->numberLength - 1] - '0';
		for (int i = (int)this->numberLength - 2; i >= 0; i--)
		{
			r *= 10;
			long long c = this->digits[i] - '0';
			r += c;
		}
		return r;
	}
	long long r = this->digits[this->numberLength - 1] - '0';
	r = -r;
	for (int i = (int)this->numberLength - 2; i >= 0; i--)
	{
		r *= 10;
		long long c = this->digits[i] - '0';
		r -= c;
	}
	return r;
}

LN &LN::operator=(const LN &b)
{
	char *digits1 = (char *)realloc(this->digits, b.digitsLength);
	if (!digits1)
	{
		throw "MemoryError";
	}
	this->digits = digits1;
	for (int i = 0; i < b.numberLength; i++)
	{
		this->digits[i] = b.digits[i];
	}
	for (int i = (int)b.numberLength; i < b.digitsLength; i++)
	{
		this->digits[i] = '0';
	}
	this->digitsLength = b.digitsLength;
	this->numberLength = b.numberLength;
	this->sign = b.sign;
	return *this;
}

LN &LN::operator=(LN &&b)
{
	if (this != &b)
	{
		char *digits1 = (char *)realloc(this->digits, b.digitsLength);
		if (!digits1)
		{
			throw "MemoryError";
		}
		this->digits = digits1;
		for (int i = 0; i < b.numberLength; i++)
		{
			this->digits[i] = b.digits[i];
		}
		for (int i = (int)b.numberLength; i < b.digitsLength; i++)
		{
			this->digits[i] = '0';
		}
		this->digitsLength = b.digitsLength;
		this->numberLength = b.numberLength;
		this->sign = b.sign;
		free(b.digits);
		b.digits = NULL;
		b.digitsLength = 0;
		b.numberLength = 0;
		b.sign = '+';
	}
	return *this;
}

LN operator"" _ln(const char *x)
{
	return LN(x);
}