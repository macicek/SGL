#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <functional>

#include "Color.h"
#include "Geometry.h"
#include "RayTracerDefines.h"
#include "PointLight.h"
#include "Primitive.h"
#include "RayTracer.h"

/// A context class.
/**
	A context class represents a single scene or a context. Multiple contexts can be held in memory at the
	same time. Switching between them is done inside GUI and drawing inside the currently shown one.
*/
class Context
{
	public:
		/// Context constructor.
		/**
			Allocates a color buffer for a single scene (called context) based on width and height. Multiple contexts
			are stored in memory simultaneously. User always works only with the currently shown context.

			@param width [in] Context width.
			@param height [in] Context height.
		*/
		Context ( uint32 width = 0, uint32 height = 0 ) 
			: _w(width), _h(height), _size(width*height), _inCycle(false), _updateMVPMneeded(false)			
		{ 
			_matrixStack		= new std::vector<matrix4x4>;
			_colorBuffer		= new rgb[width * height];
			
			initZBuffer();	

			_rayTracer			= new RayTracer(); // is created when needed
		} 		

		/// Context destructor.
		/**
			Called when deleting a context. Frees all used memory.
		*/
		~Context(){ };

		/// Returns pointer to the color buffer.
		/**
			Returns the address of the start of the allocated memory used for storing color
			information (color buffer).

			@return Color buffer pointer.
		*/
		float*			getColorBufferPointer()
		{ return (float*) _colorBuffer; }

		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param color[in] RGBA color value.
		*/
		void			setCurrentColor(rgb color)
		{ _currentColor = color; }
		
		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param r[in] Red channel value.
			@param g[in] Green channel value.
			@param b[in] Blue channel value.
		*/
		void			setCurrentColor(float r, float g, float b)
		{ _currentColor = rgb(r, g, b); }

		/// Sets the point size used for drawing.
		/**
			Sets the point size currently being used for drawing. 1.0 corresponds to 1px.

			@param size[in] Point size in pixels.
		*/
		void			setPointSize(float size){ _pointSize = size; }

		/// Sets drawing mode.
		/** 
			Sets the element type to use for drawing (mode).

			@param mode[in] Element type to use for drawing.
		*/
		void setDrawingMode(sglEElementType mode){ _drawingMode = mode; }

		/// Returns currently used drawing mode.
		/**
		   Returns the element type (mode) currently being used for drawing.
		 
		   @return Drawing mode.
		 */
		sglEElementType getDrawingMode(){ return _drawingMode; }

		/// Clear the vertex buffer.
		/**
			Clears the vertex buffer storing all the vertices used to draw the current element and calls appropriate destructors.
		*/
		void clearVertexBuffer(){ _vertexBuffer.clear(); }
		void clearVectorBuffer(){ _vectorBuffer.clear(); }

		/// Adds a vertex inside the vertex container.
		/**
			Adds a vertex to the vertex container, later on used for drawing after calling sglEnd().

			@param vertex[in] A vertex.
		*/
		void addVertex( vertex v )
		{ 			
			if ( _updateMVPMneeded )
				doMVPMupdate();

			v *= _matrix[M_MVP];				

			v.wNormalize();			

			v *= _matrix[M_VIEWPORT];
		
			_vertexBuffer.push_back( v );
		}

		void addVector( vector3 const& v )
		{
			_vectorBuffer.push_back( v );
		}

		/// Draws an arc
		/**
			Draws an arc based on parameters given in sglArc (center point, radius and from/to angle). It adds pairs
			of vertices inside vertex buffer and when done it calls line strip rasterization to display an arc.

			@param arc[in] An arc.
		*/
		void			addArc( arc a, bool filled = false )
		{
			const float from		= a.from(),
						to			= a.to(),
						radius		= a.radius(),
						n_segments	= ceil( ARC_SEGMENTS_F * (to - from) / (2.0f * PI_F) ),
						center_x	= a.x(),
						center_y	= a.y(),
						c			= (to - from) / n_segments;

			float x, y, old_x, old_y;
			
			for (float i = from; i < to; i += c)
			{
				x = radius * sin( i - PI_F / 2 );
				y = radius * cos( i - PI_F / 2 );

				if (i > from)
				{
					addVertex( vertex( center_x - old_x, center_y + old_y ) );
					addVertex( vertex( center_x - x, center_y + y) );
				}

				old_x = x;
				old_y = y;
			}
			x = radius * sin( to - PI_F / 2.0f );
			y = radius * cos( to - PI_F / 2.0f );

			addVertex( vertex( center_x - old_x, center_y + old_y ) );
			addVertex( vertex( center_x - x, center_y + y) );

			if ( filled )
			{
				addVertex( vertex( center_x, center_y ) );
				addFilledPolygon();
			}
			else
			{
				rasterizeLineStrip();
			}
			clearVertexBuffer();
		}

		/// Draws an ellipse
		/**
			Draws an ellipse based on parameters given in sglEllipse (center and a,b - check any geometry
			math book/wikipedia on what these are. Works the same as addArc( arc ), only adds pairs of
			vertices for a whole ellipse and then calls line loop rasterization.

			@param ellipse[in]	Ellipse parameters
			@param filled[in]	Draw a filled/non-filled ellipse. Default value set to false (non-filled).
		*/
		void			addEllipse( ellipse e, bool filled = false )
		{
			const float	a = e.a(),
						b = e.b();

			float		x = e.x() - a,
						y = e.y() - b,

						old_x,
						old_y;
						
			const float	center_x = x,
						center_y = y;

			for (uint8 i = 0; i < ELLIPSE_SEGMENTS_UI; ++i)
			{
				x = a * sin( (static_cast<float>(i) / ELLIPSE_SEGMENTS_F) * (PI_F * 2.0f) );
				y = b * cos( (static_cast<float>(i) / ELLIPSE_SEGMENTS_F) * (PI_F * 2.0f) );

				if (i)				
				{
					addVertex( vertex( a + center_x + old_x, b + center_y - old_y ) );
					addVertex( vertex( a + center_x + x, b + center_y - y ) );
				}

				old_x = x;
				old_y = y;
			}

			filled ? addFilledPolygon() : rasterizeLineLoop();
			clearVertexBuffer();
		}

		/// Draws a circle
		/**					
			@param circle[in] A circle.
			@param filled[in] To be or not to be FILLED ! - Hamlet (http://en.wikipedia.org/wiki/Hamlet)
		*/
		void			addCircle( circle c , bool filled = false )
		{		
			// center normalization
			vertex center = c.center();
			center *= _matrix[M_MODELVIEW];
			center *= _matrix[M_PROJECTION];

			center.wNormalize();
			
			center *= _matrix[M_VIEWPORT];

			float vp_width = static_cast<float>( _viewport.width() );
			float vp_height = static_cast<float>( _viewport.height() );
			// TODO: add documentation

			doMVPMupdate();
			float det = ( (vp_width / 2) * _matrix[M_MVP][0] * 
						(vp_height / 2) * _matrix[M_MVP][5] ) -
						( (vp_width / 2) * _matrix[M_MVP][1] *
						(vp_height / 2) * _matrix[M_MVP][4] );

			int32 radius = static_cast<int32>(sqrt(det) * c.radius());

			// midpoint alg. (modified bressenham)
			int32	x = radius, 
					y = 0;

			int32	center_x = static_cast<int32>(center.x()),
					center_y = static_cast<int32>(center.y());

			int32	cd2 = 0;
 
			if ( filled )
			{
				fillBetweenPoints2D( center_x - radius, center_x + radius, center_y );
			}
			else
			{
				setPixel(static_cast<uint32>(center_x - radius), static_cast<uint32>(center_y));
				setPixel(static_cast<uint32>(center_x + radius), static_cast<uint32>(center_y));
				setPixel(static_cast<uint32>(center_x),			 static_cast<uint32>(center_y - radius));
				setPixel(static_cast<uint32>(center_x),			 static_cast<uint32>(center_y + radius));
			}

			while ( x > y )
			{
				cd2 -= (--x) - (++y);
				
				if ( cd2 < 0 ) 
					cd2 += x++;

				if ( filled )
				{
					fillBetweenPoints2D( center_x - x, center_x + x, center_y - y );
					fillBetweenPoints2D( center_x - y, center_x + y, center_y + x );
					fillBetweenPoints2D( center_x - x, center_x + x, center_y + y );
					fillBetweenPoints2D( center_x - y, center_x + y, center_y - x );				
				}
				else
				{
					// draws 8ths of the circle at the same time
					setPixel(static_cast<uint32>(center_x - x), static_cast<uint32>(center_y - y) );	// <135; 180>
					setPixel(static_cast<uint32>(center_x - y), static_cast<uint32>(center_y - x) );	// <90; 135>
					setPixel(static_cast<uint32>(center_x + y), static_cast<uint32>(center_y - x) );	// <45; 90>
					setPixel(static_cast<uint32>(center_x + x), static_cast<uint32>(center_y - y) );	// <0; 45>

					setPixel(static_cast<uint32>(center_x - x), static_cast<uint32>(center_y + y) );	// <180; 225>
					setPixel(static_cast<uint32>(center_x - y), static_cast<uint32>(center_y + x) );	// <225; 270>
					setPixel(static_cast<uint32>(center_x + y), static_cast<uint32>(center_y + x) );	// <270; 315>
					setPixel(static_cast<uint32>(center_x + x), static_cast<uint32>(center_y + y) );	// <315; 0>
				}

			 } 		
		}
	
		/// Draws a filled polygon
		/**
			Fills a polygon shaped object based on vertices in vertex buffer. Uses a scan-line fill algorithm.
			(more info google or here: http://www.cmpe.boun.edu.tr/~sahiner/cmpe460web/FALL2009/scanlinefill.pdf)

			First it constructs edges of the polygon and pushes them into a non-active buffer. Then it determines
			the max. and min. y-coordinates and loops incrementally from min to max. Every time it finds a 
			starting y-coordinate inside the non-active buffer, it adds the edge to the active buffer and removes it
			from the non-active buffer. The same happens for ending y-coordinates and corresponding edges inside
			the active buffer.

			While looping it fills segments based on the active buffer.
		*/
		void addFilledPolygon( void )
		{			
			int32	maxY = std::numeric_limits<int32>::min();
			for (std::vector<vertex>::iterator it = _vertexBuffer.begin(); it != _vertexBuffer.end(); ) 
			{
				vertex tmp = *it;

				if (++it == _vertexBuffer.end())
					break;

				edge e(tmp, *it);
				
				_edgeBeginnings.push_back(e);				
				maxY = std::max( maxY, math::round(e.end()) );
			}			
			edge e(_vertexBuffer.back(), _vertexBuffer.front());
			_edgeBeginnings.push_back(e);

			// better to use std::sort, than shuffle sort for speed
			// shuffle sort is used later on, when the edges are already sorted
			std::sort( _edgeBeginnings.begin(), _edgeBeginnings.end(), edge::startFunctor() );		

			uint32 i, minY = math::round( _edgeBeginnings.front().start() );

			std::vector<edge>::iterator it;					
			vertex tmp;
			for (int32 y = minY; y < maxY; ++y)
			{								
				// edge activation
				it = _edgeBeginnings.begin();
				while (!_edgeBeginnings.empty() && math::round(it->start()) == y)
				{
					_activeEdges.push_back(*it);
					_edgeBeginnings.erase(it);
					it = _edgeBeginnings.begin();
				}						
			
				int32 roundedEdgeEnd;
				for (i = 0; i < _activeEdges.size(); ++i)
				{
					roundedEdgeEnd = math::round(_activeEdges[i].end());
					if (roundedEdgeEnd == y)
					{
						_activeEdges.erase(_activeEdges.begin()+i);
						--i;
					}
					else
					{						
						edge& e = _activeEdges[i];
						e.nextStep();
					}
				}

				if (_activeEdges.empty())
					continue;			

				sortActiveEdges();	

				for (it = _activeEdges.begin(); it != _activeEdges.end(); ++it)
				{	
					if ( math::round( it->end() ) == y )
						continue;

					tmp = it->toVertex();					
					do					
						++it;
					while (it != _activeEdges.end() && math::round( it->end() ) == y);
						
					if ( it == _activeEdges.end() )
						break;
				
					_depthTest ? fillBetweenPoints3D(tmp.x(), it->x(), y, tmp.z(), it->z()) : fillBetweenPoints2D(tmp.x(), it->x(), y);			
				}
			}
			_activeEdges.clear();
			_edgeBeginnings.clear();
		}

		/// Sort for active edges
		/**
			Improved shuffle sort for active edges. First shuffle sort sorts from the beginning. If the edges are not sorted
			then it sorts from the end.
		*/
		void sortActiveEdges()
		{
			bool sorted;
			uint32 i = 0;
			do 
			{
				sorted = false;
				for (; i < _activeEdges.size() - 1; ++i)
				{
					if ( _activeEdges[i].x() > _activeEdges[i + 1].x() )
					{
						std::swap( _activeEdges[i], _activeEdges[i+1] );				
						sorted = true;
					}
				}

				if (!sorted)
					break;				
					
				for (--i; i > 0; --i)
				{
					if ( _activeEdges[i].x() < _activeEdges[i - 1].x() )
					{
						std::swap( _activeEdges[i], _activeEdges[i-1]);						
						sorted = true;
					}
				}

			} while (sorted);
		}

		// Line drawing in 3D
		/**
			Draws a line from [x_a, y] to [x_b, y] in 3D, therefore it takes depth into account
			computes a depth step a and changes depth of the pixels throughout the iteration.

			@param x_a[in]	Starting x-coordinate
			@param x_b[in]	Ending x-coordinate
			@param y[in]	Y-coordinate, which stays the same
			@param d_a[in]	Starting depth
			@param d_b[in]	Ending depth
		*/
		void fillBetweenPoints3D(float x_a, float x_b, int32 y, float d_a, float d_b)
		{		
			int32 from = static_cast<int32>( x_a );
			int32 to = static_cast<int32>( x_b );

			if (from > to)
				std::swap(from, to);			
			
			float step = (d_b - d_a) / static_cast<float>(to - from);
			for (uint32 x = from; x <= to; ++x, d_a += step)
				setPixel(x, y, d_a, true);
		}

		// Line draing in 2D
		/**
			Optimalized version for 2D. It's not needed to compute depth, so we can leave it out.

			@param x_a[in]	Starting x-coordinate
			@param x_b[in]	Ending x-coordinate
			@param y[in]	Y-coordinate, which stays the same
		*/
		void fillBetweenPoints2D(float x_a, float x_b, int32 y)
		{		
			int32 from = static_cast<int32>( x_a );
			int32 to = static_cast<int32>( x_b );

			if (from > to)
				std::swap(from, to);			

			for (uint32 x = from; x < to; ++x)
				setPixel(x, y);
		}

		void			MVPMupdate()
		{
			_updateMVPMneeded = true;
		}

		/// Inserts points into memory
		/**
			Inserts all the vertices inside vertex buffer as points with given point size into memory
		*/
		void			rasterizePoints()
		{
			uint32 size = static_cast<uint32>( _pointSize/2 );

			for (std::vector<vertex>::iterator it = _vertexBuffer.begin(); it != _vertexBuffer.end(); ++it)
			{
				vertex v = *it;
				uint32 x = static_cast<uint32>(v.x());
				uint32 y = static_cast<uint32>(v.y());

				if (!size)
					setPixel(x, y);
				else
				{
					for (uint32 i = 0; i < size; ++i)
					{
						setPixel(x, y);
						setPixel(x+size, y+size);
						setPixel(x+size, y);
						setPixel(x, y+size);
						setPixel(x-size, y-size);
						setPixel(x-size, y);
						setPixel(x, y-size);
					}
				}
			}
		}

		/// Inserts a line segment into memory
		/**
			Based on vertices inside vertex buffer (must be 2), calculates points using Bressenham algorithm
			and inserts them into memory. 
			
			Note that in case the absolute value of Y coord rises faster, than the absolute value of X coord
			(in the I quadrant its angle towards X axis is over 45deg), we swap the X coord with the Y coord and
			the other way around. Also when the end vertex is closer to [0, 0], we swap the end vertex with the
			start vertex.

			Also instead of using division, we use bit shift to divite by 2.

			@param start[in] Starting vertex.
			@param end[in] Ending vertex.
		*/
		void		rasterizeLineSegment(vertex start, vertex end)
		{
			int32 y_2 = static_cast<int32>( end.y() );
			int32 y_1 = static_cast<int32>( start.y() );

			int32 x_2 = static_cast<int32>( end.x() );
			int32 x_1 = static_cast<int32>( start.x() );

			const bool over45 = (fabs(end.y() - start.y()) > fabs(end.x() - start.x()));
			if (over45)
			{
				std::swap(x_1, y_1);
				std::swap(x_2, y_2);
			}
 
			if (x_1 > x_2)
			{
				std::swap(x_1, x_2);
				std::swap(y_1, y_2);
			}
 
			const int32 dx = x_2 - x_1;
			const int32 dy = abs(y_2 - y_1);
 
			int32 err = 0;
			const int32 y_step = (y_1 < y_2) ? 1 : -1;
			for (; x_1 < x_2; ++x_1)
			{
				over45 ? setPixel( y_1, x_1 ) : setPixel( x_1, y_1 );
				err += dy;

				if ( ( err << 1 ) >= dx)
				{
					y_1 += y_step;
					err -= dx;
				}
			}
		}

		/// Calls rasterization for one segment
		void			rasterizeLines()
		{
			VertexIterator old;
			for (VertexIterator it = _vertexBuffer.begin(); it < _vertexBuffer.end(); it += 2)
				rasterizeLineSegment(*it, *(it+1));
		}

		/// Calls rasterization for a line strip
		void			rasterizeLineStrip()
		{
			VertexIterator old;
			for (VertexIterator it = _vertexBuffer.begin(); it != _vertexBuffer.end(); ++it)
			{
				if (it == _vertexBuffer.begin())
				{
					old = it;
					continue;
				}

				rasterizeLineSegment(*old, *it);
				old = it;
			}
		}



		/// Calls rasterization for a line loop
		/**
			Line loop behaves the same as a line strip except there is an extra line from end to start.
		*/
		void			rasterizeLineLoop()
		{
			rasterizeLineStrip();
			rasterizeLineSegment(_vertexBuffer.back(), _vertexBuffer.front());
		}

		/// Sets a given pixel
		/*
			Sets a given pixel with whatever color is currently being used.

			@param x[in] X coordinate.
			@param y[in] Y coordinate.
		*/
		void			setPixel(uint32 x, uint32 y, float z = 0.0f, bool depth = false)
		{
			if (depth)
			{
				uint32 pos = _w * y + x;
					
				if (z <= _zbuffer[pos])
				{
					_zbuffer[pos] = z;
					setColorBuffer(x, y, _currentColor);
				}
			}
			else
				setColorBuffer(x, y, _currentColor);
		}

		/// Sets a color for a given pixel inside memory
		/*
			Sets a specific place inside memory with a given color.

			@param x[in] X coordinate.
			@param y[in] Y coordinate.
			@param color[in] A color.
		*/
		void			setColorBuffer( uint32 x, uint32 y, rgb color )
		{
			uint32 i = _w * y + x;		

			_colorBuffer[i] = color;
		}

		/// Sets matrix mode
		/**
			Sets a mode in which we operate when multiplying matrices. Values can an be 
			SGL_PROJECTION or SGL_MODELVIEW.
			
			@param mode[in] Mode to apply matrix transformation.
		*/
		void			setMatrixMode( sglEMatrixMode mode )
		{ _currentMatrixMode = mode; }

		/// Returns current matrix mode
		sglEMatrixMode	getMatrixMode()
		{ return _currentMatrixMode; }

		/// Sets viewport
		/**
			Sets viewport data structure and calculates a viewport transformation matrix.
			The matrix is generated as a method of matrix4x4.
			
			@param vp[in] Viewport data structure
		*/
		void			setViewport( viewport vp )
		{ 
			_viewport = vp;

			float width		= static_cast<float>( vp.width() );
			float height	= static_cast<float>( vp.height() );
			float offsetX	= static_cast<float>( vp.offsetX() );
			float offsetY	= static_cast<float>( vp.offsetY() );
			
			_matrix[M_VIEWPORT] = matrix4x4().viewport( width, height, offsetX, offsetY );
		}

		/// Returns viewport data
		viewport		getViewport() const
		{ return _viewport; }

		/// Enables/disables depth
		/**
			@param value[in] True to enable, false to disable
		*/
		void			enableDepth(bool value)
		{ _depthTest = value; }

		/// Checks if we are between sglBegin and sglEnd calls
		bool isInCycle() const
		{
			return _inCycle;
		}

		/// Sets if we are between sglBegin/sglEnd or not
		void setInCycle (bool value)
		{ _inCycle = value; }

		std::vector<matrix4x4>* getMatrixStack() const
		{ return _matrixStack; }

		void setMatrix( contextMatrices type, matrix4x4 m )
		{ _matrix[type] = m; }

		matrix4x4 getMatrix( contextMatrices m ) const
		{ return _matrix[m]; }

		void setAreaMode( sglEAreaMode mode )
		{ _currentAreaMode = mode; }

		sglEAreaMode getAreaMode() const
		{ return _currentAreaMode; }

		void setClearColor(float r, float g, float b)
		{ setClearColor(rgb(r, g, b)); }

		void setClearColor(rgb color)
		{ _clearColor = color; }

		void clearColor()
		{			
			for ( uint32 i = 0; i < _size; ++i )
				_colorBuffer[i] = _clearColor;
		}

		void clearZBuffer()
		{			
			for ( uint32 i = 0; i < _size; ++i )
				_zbuffer[i] = Z_BUFFER_INFINITY;
		}

		void setSceneDefining( bool value )
		{ _isDefiningScene = value; }		

		void setCurrentMaterial( float const& r, float const& g, float const& b, float const& kd,  float const& ks, float const& shine, float const& T, float const& ior )
		{ setCurrentMaterial( material( rgb(r, g, b), kd, ks, shine, T, ior ) ); }

		void setCurrentMaterial( material const& m )
		{ _currentMaterial = m; }

		void addLight( PointLight* light )
		{
			_rayTracer->addLight( light );
		}

		bool isDefiningScene() const
		{ return _isDefiningScene; }

		uint32 getVertexBufferSize() const
		{ return _vertexBuffer.size(); }

		uint32 getVectorBufferSize() const
		{ return _vectorBuffer.size(); }

		void addTriangle()
		{
			// pretty ugly conversion from a vertex to a vector
			// TODO: unify vector3 and vertex
				
			Triangle* triangle = new Triangle(
						_vectorBuffer[0],
						_vectorBuffer[1],
						_vectorBuffer[2]
					);

			triangle->setMaterial( _currentMaterial );

			_rayTracer->addPrimitive( triangle );
		}	

		void addSphere( vector3 const& center, float const& radius )
		{
			// setting material is done inside RayTracer::addPrimitive(Primitive*), because it's the same for
			// all the primitives
			Sphere* sphere = new Sphere( center, radius );
			sphere->setMaterial( _currentMaterial );

			_rayTracer->addPrimitive( sphere );
		}

		void renderScene()
		{
			doMVPMupdate();

			_rayTracer->setInverseMatrix( _matrix[M_MVP].inverse() );
			_rayTracer->setViewportMatrix( _viewport, _matrix[M_VIEWPORT] );

			for ( uint32 y = 0; y < _h; ++y )
			{
				for ( uint32 x = 0; x < _w; ++x )				
					setColorBuffer( x, y, _rayTracer->castRay(x, y) );				
			}
		}


	protected:
		void doMVPMupdate()
		{
			_matrix[M_MVP] =  _matrix[M_PROJECTION] * _matrix[M_MODELVIEW];

			_updateMVPMneeded = false;
		}

		void initZBuffer()
		{			
			_zbuffer = new float[_w * _h];
			clearZBuffer();
		}

	private:
		uint32 _w, _h;
		uint32 _size;
		
		rgb*				_colorBuffer;
		float*					_zbuffer;

		float					_pointSize;

		rgb				_currentColor;
		rgb				_clearColor;

		std::vector<vertex>		_vertexBuffer;
		std::vector<vector3>	_vectorBuffer;
		std::vector<matrix4x4>*	_matrixStack;
		std::vector<uint32>		_edgeBuffer;
		
		viewport				_viewport;
		
		matrix4x4				_matrix[M_SIZE];

		sglEMatrixMode			_currentMatrixMode;
		sglEAreaMode			_currentAreaMode;
		sglEElementType			_drawingMode;

		bool					_inCycle;
		bool					_depthTest;
		bool					_updateMVPMneeded;

		std::vector<edge>		_nonActiveEdges;
		std::vector<edge>		_edgeBeginnings;
		std::vector<edge>		_edgesEnds;
		std::vector<edge>		_activeEdges;

		// RayTracer
		bool					_isDefiningScene;
		RayTracer*				_rayTracer;
		material				_currentMaterial;
};

#endif
