#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

class PointLight
{
	public:
		PointLight( vector3<float> const& position, rgb<float> const& color )
			: _position(position), _color(color)
		{ }

		vector3<float> getPosition() const
		{ return _position; }

		float getPosX() const { return _position.x(); }
		float getPosY() const { return _position.y(); }
		float getPosZ() const { return _position.z(); }

		rgb<float> getColor() const
		{ return _color; }

	private:
		vector3<float>	_position;
		rgb<float>		_color;
};

#endif
