#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

class PointLight
{
	public:
		PointLight( vector3 const& position, rgb const& color )
			: _position(position), _color(color)
		{ }

		vector3 getPosition() const
		{ return _position; }

		float getPosX() const { return _position.x(); }
		float getPosY() const { return _position.y(); }
		float getPosZ() const { return _position.z(); }

		rgb getColor() const
		{ return _color; }

	private:
		vector3	_position;
		rgb		_color;
};

#endif
