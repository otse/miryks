#include <skyrim_units>
#include <libs>
#include <Gloom/Interior.h>
#include <Gloom/Util.h>

#include <Gloom/Mesh.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <algorithm>
#include <cctype>
#include <string>

// #include <OpenGL/renderable.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Lights.h>
#include <OpenGL/Types.h>

namespace gloom
{
	Interior::Interior(const char *edid)
	{
		editorId = edid;
		Group *group = new Group();
	}

	void Interior::loadCell()
	{
		loadedCell = getCell(editorId);
		parseGrup(loadedCell, loadedCell.persistent);
		parseGrup(loadedCell, loadedCell.non_persistent);
	}

	Cell Interior::getCell(const char *name)
	{
		Cell cell;
		ObjectArray A, B, C;
		Grup *top = esp_top_grup(get_plugins()[1], "CELL");
		A(top).forEach([&](unsigned int i) {
			B(A.getGrup(i)).forEach([&](unsigned int j) {
				C(B.getGrup(j)).forEach([&](unsigned int &k) {
					Object object(C.getRecord(k));
					Grup *grup = C.getGrup(k + 1);
					const char *editorId = getEditorId(object);
					if (0 == strcmp(name, editorId))
					{
						ObjectArray D(grup);
						cell.record = object.record;
						printf("forEach found your interior `%s`\n", editorId);
						cell.persistent = D.size() >= 1 ? D.getGrup(0) : 0;
						cell.non_persistent = D.size() >= 2 ? D.getGrup(1) : 0;
						A.stop = B.stop = C.stop = true;
					}
					k += 1;
				});
			});
		});
		return cell;
	}

	static void PlaceCameraDud(Interior *);

	void Interior::parseGrup(Cell &cell, Grup *grup)
	{
		if (grup == nullptr)
			return;
		ObjectArray array;
		array(grup).forEach([&](unsigned int i) {
			Record *record = array.getRecord(i);
			Object object(record);
			if (object.isType("REFR"))
			{
				Ref *ref = new Ref(record);
				refs.push_back(ref);
				const char *editorId = getEditorId(object);
				if (editorId)
					editorIds.emplace(editorId, ref);
				if (ref->baseObject.valid() && ref->baseObject.isTypeAny({"WEAP", "MISC"}))
				{
					iterables.push_back(ref);
				}
			}
		});
		placeCamera();
	}

	void Interior::placeCamera()
	{
		if (alreadyTeleported)
			return;
		ObjectArray array;
		array(loadedCell.persistent).forEach([&](unsigned int i) {
			Object object(array.getRecord(i));
			if (*getBaseId(object) == 0x0000003B) //  "Marker"
			{
				// Place at any XMarker
				float *locationalData = object.data<float *>("DATA");
				printf(" xmarker ! \n");
				firstPersonCamera->pos = *cast_vec_3(locationalData);
				firstPersonCamera->pos.z += EYE_HEIGHT;
				firstPersonCamera->fyaw = cast_vec_3(locationalData + 3)->z;
				alreadyTeleported = true;
				array.stop = true;
			}
		});
	}

	Interior::~Interior()
	{
		unload();
	}

	void Interior::unload()
	{
		for (auto it = refs.begin(); it != refs.end(); ++it)
		{
			Ref *ref = *it;
			delete ref;
		}
	}

	bool myfunction(Ref *l, Ref *r)
	{
		return l->getDistance() < r->getDistance();
	}

	void Interior::update()
	{
		std::vector<Ref *> closest = iterables;
		std::sort(iterables.begin(), iterables.end(), myfunction);

		for (auto it = closest.begin(); it != closest.end(); ++it)
		{
			Ref *ref = *it;

			if (ref->displayAsItem())
				return;
		}
	}

} // namespace gloom