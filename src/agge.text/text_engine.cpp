#include <agge.text/text_engine.h>

#include <agge.text/functional.h>

using namespace std;

namespace agge
{
	namespace
	{
		enum {
			c_rescalable_height = 1000,
		};
	}

	class text_engine_base::cached_outline_accessor : public font::accessor, noncopyable
	{
	public:
		cached_outline_accessor(const font::accessor_ptr &underlying)
			: _underlying(underlying)
		{	}

	private:
		typedef hash_map< uint16_t, pair<glyph::outline_ptr, glyph::glyph_metrics> > glyphs;

	private:
		virtual font::metrics get_metrics() const
		{	return _underlying->get_metrics();	}

		virtual uint16_t get_glyph_index(wchar_t character) const
		{	return _underlying->get_glyph_index(character);	}

		virtual glyph::outline_ptr load_glyph(uint16_t index, glyph::glyph_metrics &m) const
		{
			glyphs::iterator i = _glyphs.find(index);

			if (_glyphs.end() == i)
			{
				glyph::outline_ptr o = _underlying->load_glyph(index, m);
					
				_glyphs.insert(index, make_pair(o, m), i);
			}
			m = i->second.second;
			return i->second.first;
		}

	private:
		const font::accessor_ptr _underlying;
		mutable glyphs _glyphs;
	};


	size_t text_engine_base::font_key_hasher::operator ()(const font::key &/*key*/) const
	{	return 1;	}


	text_engine_base::text_engine_base(loader &loader_, unsigned collection_cycles)
		: _loader(loader_), _collection_cycles(collection_cycles)			
	{	}

	text_engine_base::~text_engine_base()
	{
		for (garbage_container::iterator i = _garbage.begin(); i != _garbage.end(); ++i)
			destroy(i->second.first);
	}

	void text_engine_base::collect()
	{
		for (garbage_container::iterator i = _garbage.begin(); i != _garbage.end(); )
			i = !--i->second.second ? destroy(i->second.first), _garbage.erase(i) : ++i;
	}

	font::ptr text_engine_base::create_font(const wchar_t *typeface, int height, bool bold, bool italic,
		font::key::grid_fit grid_fit)
	{
		fonts_cache::iterator i;
		font::key key(typeface, height, bold, italic, grid_fit);

		if (!_fonts.insert(key, weak_ptr<font>(), i) && !i->second.expired())
			return i->second.lock();

		garbage_container::const_iterator gi = _garbage.find(key);
		std::unique_ptr<font> pre_f;

		if (_garbage.end() != gi)
		{
			pre_f.reset(gi->second.first);
			_garbage.erase(gi);
		}
		else
		{
			pair<font::accessor_ptr, real_t> acc = create_font_accessor(key);
			pre_f.reset(new font(acc.first, acc.second));
		}

		//? auto bf = bind(&text_engine_base::on_released, this, &*i, _1);
		//? font::ptr f(pre_f.get(), bf );

		auto lf = [=](font *font) { this->on_released(&*i, font); };
		font::ptr f(pre_f.get(), lf );

		pre_f.release();
		i->second = f;
		return f;
	}

	pair<font::accessor_ptr, real_t> text_engine_base::create_font_accessor(font::key fk)
	{
		if (font::key::gf_none != fk.grid_fit_)
			return make_pair(_loader.load(fk.typeface.c_str(), fk.height, fk.bold, fk.italic, fk.grid_fit_), 1.0f);

		scalabale_fonts_cache::iterator i;
		const real_t factor = static_cast<real_t>(fk.height) / c_rescalable_height;

		fk.height = c_rescalable_height;
		if (_scalable_fonts.insert(fk, weak_ptr<font::accessor>(), i) || i->second.expired())
		{
			font::accessor_ptr a(new cached_outline_accessor(_loader.load(fk.typeface.c_str(), fk.height, fk.bold,
				fk.italic, fk.grid_fit_)));

			i->second = a;
			return make_pair(a, factor);
		}
		return make_pair(i->second.lock(), factor);
	}

	void text_engine_base::on_released(const fonts_cache::value_type *entry, font *font_)
	{
		garbage_container::iterator i ;

		if (_collection_cycles)
			_garbage.insert(entry->first, make_pair(font_, _collection_cycles), i);
		else
			destroy(font_);
	}

	void text_engine_base::destroy(font *font_) throw()
	{
		on_before_removed(font_);
		delete font_;
	}


	text_engine_base::offset_conv::offset_conv(const glyph::path_iterator &source, real_t dx, real_t dy)
		: _source(source), _dx(dx), _dy(dy)
	{	}

	void text_engine_base::offset_conv::rewind(unsigned /*id*/)
	{	}

	int text_engine_base::offset_conv::vertex(real_t *x, real_t *y)
	{
		int command = _source.vertex(x, y);

		*x += _dx, *y += _dy;
		return command;
	}
}
