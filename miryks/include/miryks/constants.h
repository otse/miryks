#define EDIT_ME    "editme.txt"

#define MASTER_0   "Skyrim.esm"
#define MASTER_1   "Update.esm"
#define MASTER_2   "Dawnguard.esm"
#define MASTER_3   "HearthFires.esm"
#define MASTER_4   "Dragonborn.esm"

#define PLUGIN_0   "Gloom - Copy.esp"

#define ARCHIVE_0  "Skyrim - Misc.bsa"
#define ARCHIVE_1  "Skyrim - Shaders.bsa"
#define ARCHIVE_2  "Skyrim - Interface.bsa"
#define ARCHIVE_3  "Skyrim - Animations.bsa"
#define ARCHIVE_4  "Skyrim - Meshes0.bsa"
#define ARCHIVE_5  "Skyrim - Meshes1.bsa"
#define ARCHIVE_6  "Skyrim - Sounds.bsa"
#define ARCHIVE_7  "Skyrim - Voices_en0.bsa"
#define ARCHIVE_8  "Skyrim - Textures0.bsa"
#define ARCHIVE_9  "Skyrim - Textures1.bsa"
#define ARCHIVE_10 "Skyrim - Textures2.bsa"
#define ARCHIVE_11 "Skyrim - Textures3.bsa"
#define ARCHIVE_12 "Skyrim - Textures4.bsa"
#define ARCHIVE_13 "Skyrim - Textures5.bsa"
#define ARCHIVE_14 "Skyrim - Textures6.bsa"
#define ARCHIVE_15 "Skyrim - Textures7.bsa"
#define ARCHIVE_16 "Skyrim - Textures8.bsa"
#define ARCHIVE_17 "Skyrim - Patch.bsa"

#define STRINGIFY(x) #x

#define REFR STRINGIFY(REFR)
#define STAT STRINGIFY(STAT)
#define DOOR STRINGIFY(DOOR)
#define FURN STRINGIFY(FURN)
#define BOOK STRINGIFY(BOOK)
#define CONT STRINGIFY(CONT)
#define ARMO STRINGIFY(ARMO)
#define WEAP STRINGIFY(WEAP)
#define AMMO STRINGIFY(AMMO)
#define MISC STRINGIFY(MISC)
#define ALCH STRINGIFY(ALCH)
#define INGR STRINGIFY(INGR)
#define LIGH STRINGIFY(LIGH)
#define MSTT STRINGIFY(MSTT)
#define TREE STRINGIFY(TREE)
#define FLOR STRINGIFY(FLOR)
#define WRLD STRINGIFY(WRLD)
#define CELL STRINGIFY(CELL)
#define LAND STRINGIFY(LAND)
#define CLAS STRINGIFY(CLAS)
#define GMST STRINGIFY(GMST)
#define RACE STRINGIFY(RACE)

#define References REFR
#define Statics STAT
#define Doors DOOR
#define Furniture FURN
#define Books BOOK
#define Containers CONT
#define Armor ARMO
#define Weapons WEAP
#define Ammo AMMO
#define Misc MISC
#define Alchemy ALCH
#define Ingredients INGR
#define Lights LIGH
#define Mists MSTT
#define Plants TREE
#define Flora FLOR
#define Cells CELL
#define Classes CLAS
#define GameSettings GMST
#define Races RACE

// used for bitmasking dn't ask
#define Things { "0", STAT, DOOR, FURN, BOOK, CONT, ARMO, WEAP, AMMO, MISC, ALCH, INGR, LIGH, MSTT, TREE, FLOR }