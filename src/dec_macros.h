/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#define IS_DECODER

#define FIELDG(name,type,dxfgroup) \
  _obj->name = bit_read_##type(dat);\
  FIELD_G_TRACE(name,type,dxfgroup)

#define FIELD(name,type) \
  _obj->name = bit_read_##type(dat);\
  FIELD_TRACE(name,type)

#define FIELD_G_TRACE(name,type,dxfgroup) \
  LOG_TRACE(#name ": " FORMAT_##type " " #type " " #dxfgroup "\n", _obj->name)
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type " " #type "\n", _obj->name)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code)  \
  if (handle_code>=0)\
    {\
      _obj->name = dwg_decode_handleref_with_code(dat, obj, dwg, handle_code);\
    }\
  else\
    {\
      _obj->name = dwg_decode_handleref(dat, obj, dwg);\
    }\
  LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu\n",\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref)
#define FIELD_HANDLE_N(name, vcount, handle_code)  \
  if (handle_code>=0)\
    {\
      _obj->name = dwg_decode_handleref_with_code(dat, obj, dwg, handle_code);\
    }\
  else\
    {\
      _obj->name = dwg_decode_handleref(dat, obj, dwg);\
    }\
  LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lu) absolute:%lu\n",\
        (int)vcount,\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref)

#define FIELD_B(name) FIELD(name, B);
#define FIELD_BB(name) FIELD(name, BB);
#define FIELD_3B(name) FIELD(name, 3B);
#define FIELD_BS(name) FIELD(name, BS);
#define FIELD_BL(name) FIELD(name, BL);
#define FIELD_BLL(name) FIELD(name, BLL);
#define FIELD_BD(name) FIELD(name, BD);
#define FIELD_RC(name) FIELD(name, RC);
#define FIELD_RS(name) FIELD(name, RS);
#define FIELD_RD(name) FIELD(name, RD);
#define FIELD_RL(name) FIELD(name, RL);
#define FIELD_RLL(name) FIELD(name, RLL);
#define FIELD_MC(name) FIELD(name, MC);
#define FIELD_MS(name) FIELD(name, MS);
#define FIELD_TV(name) FIELD(name, TV);
#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name) FIELD(name, BT);
#define FIELD_4BITS(name) _obj->name = bit_read_4BITS(dat);

#define FIELD_BE(name) bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z);
#define FIELD_DD(name, _default) FIELD_VALUE(name) = bit_read_DD(dat, _default);
#define FIELD_2DD(name, d1, d2) FIELD_DD(name.x, d1); FIELD_DD(name.y, d2);
#define FIELD_2RD(name) FIELD(name.x, RD); FIELD(name.y, RD);
#define FIELD_2BD(name) FIELD(name.x, BD); FIELD(name.y, BD);
#define FIELD_3RD(name) FIELD(name.x, RD); FIELD(name.y, RD); FIELD(name.z, RD);
#define FIELD_3BD(name) FIELD(name.x, BD); FIELD(name.y, BD); FIELD(name.z, BD);
#define FIELD_3DPOINT(name) FIELD_3BD(name)
#define FIELD_CMC(name)\
    bit_read_CMC(dat, &_obj->name);\
    LOG_TRACE(#name ": index %d\n", _obj->name.index)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size)\
  if (size>0)\
    {\
      _obj->name = (BITCODE_##type*) malloc(size * sizeof(BITCODE_##type));\
      for (vcount=0; vcount<(long)size; vcount++) \
        {\
          _obj->name[vcount] = bit_read_##type(dat);\
          LOG_INSANE(#name "[%ld]: " FORMAT_##type "\n", (long)vcount, _obj->name[vcount]) \
        }\
    }

#define FIELD_VECTOR(name, type, size) FIELD_VECTOR_N(name, type, _obj->size)

#define FIELD_2RD_VECTOR(name, size)\
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  for (vcount=0; vcount< (long)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount]);\
    }

#define FIELD_2DD_VECTOR(name, size)\
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  FIELD_2RD(name[0]);\
  for (vcount = 1; vcount < (long)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y));\
    }

#define FIELD_3DPOINT_VECTOR(name, size)\
  _obj->name = (BITCODE_3DPOINT *) malloc(_obj->size * sizeof(BITCODE_3DPOINT));\
  for (vcount=0; vcount < (long)_obj->size; vcount++) \
    {\
      FIELD_3DPOINT(name[vcount]);\
    }

#define HANDLE_VECTOR_N(name, size, code)\
  FIELD_VALUE(name) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * size);\
  for (vcount=0; vcount < (long)size; vcount++) \
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code); \
    }

#define HANDLE_VECTOR(name, sizefield, code) HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code)

//skip non-zero bytes and a terminating zero
#define FIELD_INSERT_COUNT(insert_count, type)   \
      FIELD_VALUE(insert_count)=0; \
      while (bit_read_RC(dat)) \
        {\
          FIELD_VALUE(insert_count)++;\
        }\
      FIELD_TRACE(insert_count, type)

#define FIELD_XDATA(name, size)\
  _obj->name = dwg_decode_xdata(dat, _obj->size)

#define REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.object->num_reactors);\
  for (vcount=0; vcount < (long)obj->tio.object->num_reactors; vcount++) \
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code);      \
    }

#define ENT_REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.entity->num_reactors);\
  for (vcount=0; vcount < obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code);\
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);  \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);      \
    }

#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);  \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);      \
    }

//TODO unify REPEAT macros
#define REPEAT_N(times, name, type) \
  _obj->name = (type *) malloc(times * sizeof(type));\
  for (rcount=0; rcount<(long)times; rcount++)

#define REPEAT(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount=0; rcount<(long)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount2=0; rcount2<(long)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount3=0; rcount3<(long)_obj->times; rcount3++)

#define COMMON_ENTITY_HANDLE_DATA \
  dwg_decode_common_entity_handle_data(dat, obj)

#define DWG_ENTITY(token) \
static void \
 dwg_decode_##token (Bit_Chain * dat, Dwg_Object * obj)\
{\
  long vcount, rcount, rcount2, rcount3;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Entity " #token "\n")\
  dwg->num_entities++;\
  obj->supertype = DWG_SUPERTYPE_ENTITY;\
  obj->tio.entity = (Dwg_Object_Entity*)malloc (sizeof (Dwg_Object_Entity));	\
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)calloc (1, sizeof (Dwg_Entity_##token)); \
  ent = obj->tio.entity->tio.token;\
  _obj = ent;\
  obj->tio.entity->object = obj;\
  if (dwg_decode_entity (dat, obj->tio.entity)) return;

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) static void  dwg_decode_ ## token (Bit_Chain * dat, Dwg_Object * obj) {\
  long vcount, rcount, rcount2, rcount3;\
  Dwg_Object_##token *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Object " #token "\n")\
  obj->supertype = DWG_SUPERTYPE_OBJECT;\
  obj->tio.object = (Dwg_Object_Object*)malloc (sizeof (Dwg_Object_Object));	\
  obj->tio.object->tio.token = (Dwg_Object_##token * ) calloc (1, sizeof (Dwg_Object_##token)); \
  obj->tio.object->object = obj;\
  if (dwg_decode_object (dat, obj->tio.object)) return;\
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END }

