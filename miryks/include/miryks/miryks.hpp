#ifndef MIRYKS_HPP
#define MIRYKS_HPP

extern "C"
{
#include <lib/common.h>
#include <lib/bsa.h>
#include <lib/esp.h>
#include <lib/nif.h>
#include <lib/niftypes.h>
}

#include <miryks/constants.h>
#include <files.h>

#include <opengl/scene_graph.hpp>

#include <cstdarg>

#include <vector>
#include <map>
#include <array>
#include <functional>
#include <algorithm>

/*
the templated grup code is hard to read
it uses template argument deduction
*/

constexpr float ONE_CENTIMETER_IN_SKYRIM_UNITS = 1 / 1.428f;

#define EYE_HEIGHT 160 * ONE_CENTIMETER_IN_SKYRIM_UNITS

extern bool showCursor;
bool pressing_key(const char *);
bool holding_key(const char *);

namespace miryks
{
	class Player;
	extern Player *player1;

	extern std::string installed_path;

	namespace hooks
	{
		extern bool (*hooks_some_behavior)(int);
		extern void (*load_interior)(const char *, int, bool);
		extern void (*load_world_space)(const char *, int, bool);
	}

	BSA load_archive(const char *);
	ESP load_plugin(const char *, bool = false);

	void load_these_definitions(ESP);

	class keyframes;

	keyframes *load_keyframes_from_disk(const char *);

	void init_miryks();

	static inline void init_data_files()
	{
		get_plugins()[0] = load_plugin(MASTER_0);
		get_plugins()[1] = load_plugin(MASTER_1);
		get_plugins()[2] = load_plugin(MASTER_2);
		get_plugins()[3] = load_plugin(MASTER_3);
		get_plugins()[4] = load_plugin(MASTER_4);
		get_plugins()[5] = load_plugin(PLUGIN_0, true);
		assertc(get_plugins()[0]);
		assertc(get_plugins()[1]);
		assertc(get_plugins()[2]);
		assertc(get_plugins()[3]);
		assertc(get_plugins()[4]);
		assertc(get_plugins()[5]);
	}

	static inline void init_archives()
	{
		get_archives()[0] = load_archive(ARCHIVE_0);
		// get_archives()[1] = load_archive(ARCHIVE_1);
		// get_archives()[2] = load_archive(ARCHIVE_2);
		get_archives()[3] = load_archive(ARCHIVE_3);
		get_archives()[4] = load_archive(ARCHIVE_4);
		get_archives()[5] = load_archive(ARCHIVE_5);
		// get_archives()[6] = load_archive(ARCHIVE_6);
		// get_archives()[7] = load_archive(ARCHIVE_7);
		get_archives()[8] = load_archive(ARCHIVE_8);
		get_archives()[9] = load_archive(ARCHIVE_9);
		get_archives()[10] = load_archive(ARCHIVE_10);
		get_archives()[11] = load_archive(ARCHIVE_11);
		get_archives()[12] = load_archive(ARCHIVE_12);
		get_archives()[13] = load_archive(ARCHIVE_13);
		get_archives()[14] = load_archive(ARCHIVE_14);
		get_archives()[15] = load_archive(ARCHIVE_15);
		get_archives()[16] = load_archive(ARCHIVE_16);
		// get_archives()[17] = load_archive(ARCHIVE_17);
	}

	extern std::map<const char *, nif *> nis;

	static inline const std::map<const char *, nif *> &access_nis()
	{
		return nis;
	}

	static inline void save_ni(const char *key, nif *ni)
	{
		nis.emplace(key, ni);
	}

	static inline nif *got_ni(const char *key)
	{
		auto has = nis.find(key);
		if (has != nis.end())
			*has->second;
		return nullptr;
	}

	static inline resource *get_res(
		const char *path,
		const char *prepend = "meshes\\",
		unsigned long flags = 0x1)
	{
		std::string str = prepend;
		str += path;
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			[](unsigned char c)
			{ return std::tolower(c); });
		const char *s = str.c_str();
		resource *res;
		res = bsa_find_more(s, flags);
		if (!res)
			printf("get_res fail: %s \n", s);
		bsa_read(res);
		return res;
	}

	static inline nif *get_ni(resource *res)
	{
		if (!res)
			return nullptr;
		nif *ni;
		ni = got_ni(res->path);
		if (ni)
			return ni;
		bsa_read(res);
		if (!res->buf)
			printf("res buf is 0\n");
		ni = calloc_ni();
		ni->path = res->path;
		ni->buf = res->buf;
		nif_read(ni);
		save_ni(res->path, ni);
		return ni;
	}

	void view_in_place(resource *);

	// static inline void reload_plugin_0()
	//{
	//	free_plugin(&get_plugins()[5]);
	//	get_plugins()[5] = load_plugin(PLUGIN_0, true);
	// }

	struct record
	{
		typedef csubrecord *subrecord_type;
		typedef crecord *record_type;
		typedef const char signature[5];

		crecord *r = nullptr;

		record()
		{
		}
		record(record_type p)
		{
			setr(p);
		}
		void setr(record_type p)
		{
			r = p;
			esp_check_rcd(p);
		}
		inline bool rvalid()
		{
			return !!r;
		}
		const crecord_header &rhed()
		{
			return *r->hed;
		}
		const revised_array &subrecords()
		{
			return *r->rcdbs;
		}
		subrecord_type get(unsigned int i)
		{
			return (subrecord_type)subrecords().elements[i];
		}
		bool is_type(signature sgn)
		{
			return *(unsigned int *)sgn == rhed().sgn;
		}
		bool is_types(const std::vector<const char *> &sgns)
		{
			for (const char *sgn : sgns)
				if (is_type(sgn))
					return true;
			return false;
		}
		subrecord_type find(signature sgn, int skip = 0)
		{
			for (unsigned int i = 0; i < subrecords().size; i++)
			{
				subrecord_type sub = get(i);
				if (*(unsigned int *)sgn == sub->hed->sgn)
					if (skip-- < 1)
						return sub;
			}
			return nullptr;
		}
		template <typename T = void *>
		const T data(signature sig, int skip = 0)
		{
			subrecord_type sub = find(sig, skip);
			return sub ? (T)sub->data : nullptr;
		}
		const char *editor_id()
		{
			return data<const char *>("EDID");
		}
		bool editor_id(const char *name)
		{
			return editor_id() ? 0 == strcmp(name, editor_id()) : false;
		}
		const unsigned int *base()
		{
			return data<unsigned int *>("NAME");
		}
		bool is_reference()
		{
			return is_type("REFR");
		}
	};

	// grup stuffs

#define fat_arrow operator<=

#define go_sideways fat_arrow

	template <int, typename>
	struct grup_iter;

	struct passthrough;

	struct grup
	{
		cgrup *g = nullptr;
		unsigned int index = 0;
		grup()
		{
		}
		grup(cgrup *p)
		{
			setg(p);
		}
		inline bool gvalid()
		{
			return !!g;
		}
		void setg(cgrup *p)
		{
			index = 0;
			g = p;
			esp_check_grup(g);
		}
		bool under()
		{
			return index < mixed().size;
		}
		const cgrup_header &ghed()
		{
			return *g->hed;
		}
		const revised_array &mixed()
		{
			return *g->mixed;
		}
		template <typename T>
		T child(unsigned int i)
		{
			if (i >= mixed().size)
				return nullptr;
			return reinterpret_cast<T>(mixed().elements[i]);
		}
		cgrup *next_grup()
		{
			return child<cgrup *>(index++);
		}
		crecord *next_record()
		{
			return child<crecord *>(index++);
		}
		esp_dud *next_unknown()
		{
			return child<esp_dud *>(index++);
		}
	};

	template <int intended_group_type = -1, typename next = passthrough>
	struct grup_iter : grup
	{
		grup_iter()
		{
		}
		grup_iter(const char *top, int plugin)
		{
			operator=(esp_top(get_plugins()[plugin], top));
		}
		grup_iter(grup &iter)
		{
			operator=(iter.next_grup());
		}
		void operator=(const grup &rhs)
		{
			this->setg(rhs.g);
			this->index = rhs.index;
			int group_type = this->ghed().group_type;
			assertc(
				intended_group_type == -1 ||
				intended_group_type == group_type);
		}
		template <typename T>
		bool fat_arrow(T &rhs)
		{
			while (this->under())
				if (next(*this) <= rhs)
					return true;
			return false;
		}
	};

	/* lets you use an ordinary grup as a template grup
	 */
	template <int Y>
	static inline grup_iter<Y> upcast_grup(grup &other)
	{
		grup_iter<Y> g;
		g = other;
		return g;
	}

	enum gruptypes
	{
		default_top = 0,
		world_children,
		interior_cell_block,
		interior_cell_subblock,
		exterior_cell_block,
		exterior_cell_subblock,
		cell_children,
		topic_children,
		cell_persistent_children,
		cell_temporary_children
	};

	struct passthrough
	{
		grup *iter;
		passthrough(grup &iter) : iter(&iter)
		{
		}
		template <typename T>
		bool fat_arrow(T &rhs)
		{
			T temp(*iter);
			if (temp <= rhs)
			{
				rhs = temp;
				return true;
			}
			return false;
		}
	};

	struct record_iter : record
	{
		record_iter()
		{
		}
		record_iter(grup &iter)
		{
			operator=(iter.next_record());
		}

		void operator=(const record &rhs)
		{
			this->setr(rhs.r);
		}
		const char *id = nullptr;
		bool fat_arrow(record_iter &rhs)
		{
			return this->editor_id(rhs.id);
		}
	};

	// unused, just use esp_get_form_id
	struct record_formid_iter : record_iter
	{
		unsigned int formId;
		bool fat_arrow(record_formid_iter &rhs)
		{
			return rhs.formId == this->rhed().formId;
		}
	};

	/*struct record_with_grup_iter : record, grup {
		record_with_grup_iter()
		{
		}
		record_with_grup_iter(const record_with_grup_iter &rhs) {
			r = rhs.r;
			g = rhs.g;
		}
	};*/

	/*struct record_with_grup_iter 
	{

	};*/
	
	struct record_with_grup_iter : record_iter, grup_iter<>
	{
		record_with_grup_iter()
		{
		}
		record_with_grup_iter(grup &iter) : record_iter(iter), grup_iter<>(iter)
		{
		}
		void operator=(const record_with_grup_iter &rhs)
		{
			r = rhs.r;
			g = rhs.g;
			//grup::g = rhs.grup::g;
		}
		const char *id = nullptr;
		bool fat_arrow(record_with_grup_iter &rhs)
		{
			return editor_id(rhs.id);
		}
	};

	struct record_with_grup : record, grup
	{
		record_with_grup()
		{
		}
		record_with_grup(const record_with_grup_iter &rhs)
		{
			r = rhs.r;
			g = rhs.g;
		}
	};

	struct unknown
	{
		typedef const esp_dud *dud_type;
		dud_type rhs = nullptr;
		unknown()
		{
		}
		unknown(dud_type d)
		{
			set_dud(d);
		}
		void set_dud(dud_type d) {
			this->rhs = d;
		}
		bool is_record() {
			return this->rhs->x == 'r';
		}
		bool is_grup() {
			return this->rhs->x == 'g';
		}
		record as_record() {
			return record((crecord *)rhs);
		}
		grup as_grup() {
			return grup((cgrup *)rhs);
		}
	};

	struct unknown_iter : unknown
	{
		//const esp_dud *rhs;
		unknown_iter()
		{
		}
		unknown_iter(grup &iter) {
			operator=(iter.next_unknown());
		}
		void operator=(const esp_dud *rhs)
		{
			this->set_dud(rhs);
		}
		bool fat_arrow(record_with_grup_iter &rhs)
		{
			return false;
		}
	};

	template <typename T>
	static inline record find_record_by_id(const char *id, T t)
	{
		record_iter target;
		target.id = id;
		t <= target;
		return target;
	}

	template <typename T>
	static inline record_with_grup find_record_with_grup_by_id(const char *id, T t)
	{
		record_with_grup_iter target;
		target.id = id;
		t <= target;
		return target;
	}

	// digging

	static inline record dig_race(const char *raceId, int plugin)
	{
		return find_record_by_id(
			raceId,
			grup_iter<0>("RACE", plugin));
	}

	class cell;
	class reference;
	class worldspace;
	class interior;
	class exterior;

	class reference : public record
	{
	public:
		mat4 matrix, translation, rotation, scale;
		record baseObject;
		cell *cell = nullptr;
		reference(record rc) : record(rc)
		{
			matrix = mat4(1.0);
		}
		virtual ~reference()
		{
		}
	};

	extern interior *ginterior;
	extern worldspace *gworldspace;

	template <typename T>
	struct reference_factory_iter : record_iter
	{
		cell *cell = nullptr;
		bool fat_arrow(reference_factory_iter &target)
		{
			if (this->is_reference())
			{
				T *refer = new T(*this);
				target.cell->add(refer);
			}
			return false;
		}
	};

	class cell : public record_with_grup
	{
	public:
		unsigned short flags;
		grup persistent, temporary;
		std::vector<reference *> references;
		std::map<std::string, reference *> ids;
		cell(record_with_grup rg) : record_with_grup(rg)
		{
			flags = 0;
			assertc(rg.rvalid() && rg.gvalid());
			assertc(ghed().group_type == 6);
			grup first, second;
			first = next_grup();
			second = next_grup();
			if (first.ghed().group_type == 8)
			{
				persistent = first;
				temporary = second;
			}
			else
				temporary = first;
			flags = *data<uint16_t *>("DATA");
		}
		virtual void add(reference *refe)
		{
			references.push_back(refe);
			if (refe->editor_id())
			{
				ids.emplace(refe->editor_id(), refe);
			}
		}
		template <typename T>
		void iter_both_subgroups(T &t)
		{
			if (temporary.gvalid())
				upcast_grup<9>(temporary) <= t;
			if (persistent.gvalid())
				upcast_grup<8>(persistent) <= t;
		}
		virtual void unload()
		{
			for (reference *refer : references)
				delete refer;
			references.clear();
		}
		virtual ~cell()
		{
			unload();
		}
	};

	class interior : public cell
	{
	public:
		const char *id = nullptr;
		bool dontTeleport = false;
		virtual ~interior()
		{
		}
		interior(record_with_grup rg) : cell(rg)
		{
			id = strdup(editor_id());
		}
		virtual void unload()
		{
			cell::unload();
		}
		virtual void update()
		{
		}
	};

	class worldspace : public record_with_grup
	{
	public:
		grup childs;
		std::vector<exterior *> exteriors;
		std::vector<reference *> references;
		worldspace(record_with_grup rg) : record_with_grup(rg)
		{
			printf("world space: %s\n", this->editor_id());
			childs = rg;
			assertc(ghed().group_type == world_children);
			unknown cell_record_or_exterior_block = childs.next_unknown();
			cell_record_or_exterior_block.is_record() ? childs.index++ : childs.index--;
			init();
		}
		void init();
		template <typename T>
		void build_exteriors(T factory)
		{
			for (auto exterior : exteriors)
			{
				exterior->make(factory);
			}
		}
		void load_ext_loc(int, int);
	};

	/*
	static inline interior *dig_interior(const char *name, int plugin = 5)
	{
		record_with_grup_iter rg = find_record_with_grup_by_id(
			name,
			grup_iter<0,
			grup_iter<2,
			grup_iter<3>>>("CELL", plugin));
		return new interior(rg);
	}

	static inline worldspace *dig_world_space(const char *name, int plugin = 5)
	{
		record_with_grup_iter rg = find_record_with_grup_by_id(
			name,
			grup_iter<0>("WRLD", plugin));
		return new worldspace(rg);
	}
	*/

	// unused convenience method
	/*
	static inline void reload_ginterior(int plugin = 5)
	{
		if (!ginterior)
			return;
		const char *id = ginterior->id;
		delete ginterior;
		//ginterior = dig_interior(id, plugin);
		//ginterior->dontTeleport = true;
	}
	*/

	class land;

	#pragma pack(push, 1)
	struct XCLC
	{
		int32_t x, y, flags;
	};
	#pragma pack(pop)

	class exterior : public cell
	{
	public:
		worldspace *worldSpace;
		land *land;
		XCLC *xclc;
		exterior(record_with_grup_iter rg) : cell(rg)
		{
			//printf("exterior id %i\n", this->r->id);
			land = nullptr;
			xclc = data<XCLC *>("XCLC");
		}
		void init();
		void first_make_land();
		template <typename T>
		void make(T factory)
		{
			//if (xclc->x == 0 && xclc->y == 0)
			{
				first_make_land();
				factory.cell = this;
				iter_both_subgroups(factory);
			}
		}
	};

	class land : public record
	{
	public:
		exterior *exte;
		Group *group[4];
		GroupDrawer *groupDrawer;
		land(record land, exterior *exte);
	};
}

#endif