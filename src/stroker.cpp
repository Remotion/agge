#include <agge/stroker.h>

#include <agge/stroke_features.h>
#include <agge/stroke_math.h>

namespace agge
{
	struct stroke::point_ref
	{
		point_r point;
		real_t distance;
	};

	stroke::stroke()
		: _cap(new caps::butt)
	{	}

	stroke::~stroke()
	{	delete _cap;	}

	void stroke::remove_all()
	{
		_input.clear();
		_output.clear();
	}

	void stroke::add_vertex(real_t x, real_t y, int /*command*/)
	{
		if (!_input.empty())
		{
			point_ref &last = *(_input.end() - 1);

			last.distance = distance(last.point.x, last.point.y, x, y);
		}

		point_ref p = { create_point(x, y), 0 };

		_input.push_back(p);
	}
		
	int stroke::vertex(real_t *x, real_t *y)
	{
		if (_output.empty())
			generate();
		if (_output_iterator == _output.end())
			return 0x6F;
		*x = _output_iterator->x;
		*y = _output_iterator->y;
		return _output_iterator++ == _output.begin() ? path_command_move_to : path_command_line_to;
	}
		
	void stroke::width(real_t w)
	{	_width = 0.5f * w;	}

	void stroke::generate()
	{
		input_vertices::const_iterator i = _input.begin();

		_cap->calc(_output, _width, i->point, i->distance, (i + 1)->point);
		++i;
		_cap->calc(_output, _width, i->point, (i - 1)->distance, (i - 1)->point);

		_output_iterator = _output.begin();
	}
}