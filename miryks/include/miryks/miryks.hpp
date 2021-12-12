#ifndef MIRYKS_HPP
#define MIRYKS_HPP

#include <miryks/libs.h>
#include <miryks/constants.h>
#include <miryks/miryks.h>

#include <opengl/group.h>
#include <opengl/drawgroup.h>

#include <cstdarg>

#include <vector>
#include <map>
#include <array>
#include <functional>

namespace miryks
{
	struct record
	{
		typedef const char signature[5];
		typedef csubrecord *subrecord_type;
		crecord *r = nullptr;

		record()
		{
		}
		record(crecord *r)
		{
			setr(r);
		}
		void setr(crecord *p)
		{
			r = p;
			esp_check_rcd(r);
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

	template <int, typename>
	struct grup_iter;

	struct passthrough;

	struct recordgrup;
	typedef recordgrup recordgrup;

	struct record_with_id;
	struct record_with_id_and_grup;

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

	static grup_iter<> upcast_grup(grup &other)
	{
		grup_iter g;
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

	struct recordgrup : record_iter, grup_iter<>
	{
		recordgrup()
		{
		}
		recordgrup(grup &iter) : record_iter(iter), grup_iter<>(iter)
		{
		}
		recordgrup(const recordgrup &rhs)
		{
			r = rhs.r;
			g = rhs.g;
		}
		const char *id = nullptr;
		bool fat_arrow(recordgrup &rhs)
		{
			return this->editor_id(rhs.id);
		}
	};

	/*struct recordgrup : recordgrup_iter
	{
		recordgrup()
		{
		}
	};*/

	template <typename T>
	static inline record find_record_by_id(const char *id, T t)
	{
		record_iter target;
		target.id = id;
		t <= target;
		return target;
	}

	template <typename T>
	static inline recordgrup find_recordgrup_by_id(const char *id, T t)
	{
		recordgrup target;
		target.id = id;
		t <= target;
		return target;
	}

	// digging

	static inline record dig_race(const char *raceId, int plugin)
	{
		return find_record_by_id(
		raceId,
		grup_iter<0>("RACE", plugin)
		);
	}

	static inline recordgrup dig_interior_cell(const char *cellId, int plugin)
	{
		return find_recordgrup_by_id(
		cellId,
		grup_iter<0,
		grup_iter<2,
		grup_iter<3>>>("CELL", plugin)
		);
	}

	static inline recordgrup dig_worldspace(const char *id, int plugin)
	{
		return find_recordgrup_by_id(
		id,
		grup_iter<0>("WRLD", plugin)
		);
	}

	class reference;
	class cell;

	class reference : public record
	{
	public:
		mat4 matrix;
		reference(record rc) : record(rc)
		{
			matrix = mat4(1.0);
		}
		virtual ~reference()
		{
		}
	};

	class worldspace;
	class interior;
	class exterior;

	extern interior *ginterior;
	extern worldspace *gworldspace;

	template<typename reference_derived>
	struct subgroup_iter : record_iter
	{
		cell *cell = nullptr;
		bool operator<=(subgroup_iter &target)
		{
			if (this->is_reference())
			{
				reference_derived *refer = new reference_derived(*this);
				target.cell->add(refer);
			}
			return false;
		}
	};

	class cell : public recordgrup
	{
	public:
		unsigned short flags;
		grup persistent, temporary;
		std::vector<reference *> refers;
		std::map<const char *, reference *> ids;
		cell(recordgrup rg) : recordgrup(rg)
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
		virtual void add(reference *refer)
		{
			refers.push_back(refer);
			ids.emplace(refer->editor_id(), refer);
		}
		template <typename T = reference>
		void iter()
		{
			subgroup_iter<T> maker;
			maker.cell = this;
			upcast_grup( temporary )  <= maker;
			upcast_grup( persistent ) <= maker;
		}
		virtual void unload()
		{
			for (reference *refer : refers)
			delete refer;
			refers.clear();
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
		std::map<std::string, reference *> edIds;
		virtual ~interior()
		{
		}
		interior(recordgrup rg) : cell(rg)
		{
			id = editor_id();
		}
		virtual void unload()
		{
			cell::unload();
		}
		virtual void update()
		{
			//itemfinder::consider();
		}
		/*void putcam()
		{
			for (auto refer : refers)
			{
				if (*refer->base() == 0x00000032) // coc marker heading
				{
					float *locationalData = refer->data<float *>("DATA");
					personCam->pos = cast_vec3(locationalData);
					personCam->pos.z += EYE_HEIGHT;
					personCam->yaw = cast_vec3(locationalData + 3).z;
					dontTeleport = true;
					break;
				}
			}
		}*/
	};

	class worldspace : public recordgrup
	{
	public:
		grup childs;
		std::vector<exterior *> exteriors;
		std::vector<reference *> references;
		worldspace(recordgrup rg) : recordgrup(rg)
		{
			printf("worldspace name %s", this->editor_id());
		}
		worldspace *Init();
		void DiscoverAllCells();
		void LoadExterior(int, int);
	};

	static inline interior *try_create_interior_instance(const char *name, int plugin = 5)
	{
		return new interior(dig_interior_cell(name, plugin));
	}

	static inline worldspace *try_create_worldspace_instace(const char *name, int plugin = 5)
	{
		return new worldspace(dig_worldspace(name, plugin));
	}

	static inline void reload_ginterior(int plugin = 5)
	{
		if (!ginterior)
			return;
		const char *id = ginterior->id;
		delete ginterior;
		ginterior = try_create_interior_instance(id, plugin);
		ginterior->dontTeleport = true;
		ginterior->iter();
	}

	class land;

	struct XCLC
	{
		int32_t x, y, flags;
	};

	class exterior : public cell
	{
	public:
		worldspace *worldSpace;
		land *land;
		XCLC *xclc;
		exterior(recordgrup);
		void Init();
	};

	class land : public record
	{
	public:
		exterior *exterior;
		Group *group;
		DrawGroup *drawGroup;
		land(record land);
	};
}

#endif