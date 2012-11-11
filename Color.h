#ifndef __COLOR_H__
#define __COLOR_H__

template <typename T>
struct rgb
{
	public:		
		rgb() 
			: _r(0), _g(0), _b(0)
		{}
		rgb(T const& red, T const& green, T const& blue) 
			: _r(red), _g(green), _b(blue) 
		{}

		void setRed(T value){ _r = value; }
		void setGreen(T value){ _g = value; }
		void setBlue(T value){ _b = value; }

		T red() const { return _r; }
		T green() const { return _g; }
		T blue() const { return _b; }

		rgb<T>& operator* ( rgb<T> const& color )
		{
			_r *= color.red(); _g *= color.green();	_b *= color.blue();
			return *this;
		}

		rgb<T>& operator* ( float const& coef )
		{ 
			_r *= coef; _g *= coef; _b *= coef;
			return *this;
		}

		rgb<T> operator+ ( rgb<T> const& color )
		{			
			return rgb<T>(_r + color.red(), _g + color.green(), _b + color.blue());
		}

		rgb<T>& operator+= ( rgb<T> const& color )
		{
			_r += color.red(); _g += color.green(); _b += color.blue();
			return *this;
		}

	private:
		T _r, _g, _b;
};

#endif
