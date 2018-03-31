/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#if defined(__clang__) || defined(__clang) || \
       (defined( __GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#  define GCC_DIAG_PRAGMA(x) _Pragma (#x)
/* clang has "clang diagnostic" pragmas, but also understands gcc. */
#  define GCC_DIAG_IGNORE(x) _Pragma("GCC diagnostic push") \
                             GCC_DIAG_PRAGMA(GCC diagnostic ignored #x)
#  define GCC_DIAG_RESTORE   _Pragma("GCC diagnostic pop")
#else
#  define GCC_DIAG_IGNORE(w)
#  define GCC_DIAG_RESTORE
#endif

#define IS_DECODER

#define FIELDG(name,type,dxfgroup) \
  { _obj->name = bit_read_##type(dat); \
    FIELD_G_TRACE(name,type,dxfgroup); }

#define FIELD(name,type) \
  { _obj->name = bit_read_##type(dat); \
    FIELD_TRACE(name,type); }

#define FIELD_CAST(name,type,cast,dxf)\
  { _obj->name = (BITCODE_##cast)bit_read_##type(dat); \
    FIELD_G_TRACE(name,cast,dxf); }

#define FIELD_G_TRACE(name,type,dxfgroup) \
  LOG_TRACE(#name ": " FORMAT_##type " [" #type " %d]\n", _obj->name, dxfgroup)
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type " " #type "\n", _obj->name)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code, dxf) \
  { \
    if (handle_code >= 0) \
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
          _obj->name->absolute_ref);\
  }
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)  \
  {\
    if (handle_code>=0) \
      {\
        _obj->name = dwg_decode_handleref_with_code(dat, obj, dwg, handle_code);\
      }\
    else\
      {\
        _obj->name = dwg_decode_handleref(dat, obj, dwg);\
      }\
    LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lu) absolute:%lu [%d]\n",\
              (int)vcount,                          \
              _obj->name->handleref.code,           \
              _obj->name->handleref.size,           \
              _obj->name->handleref.value,          \
              _obj->name->absolute_ref, dxf);       \
  }

#define FIELD_B(name,dxf) FIELDG(name, B, dxf)
#define FIELD_BB(name,dxf) FIELDG(name, BB, dxf)
#define FIELD_3B(name,dxf) FIELDG(name, 3B, dxf)
#define FIELD_BS(name,dxf) FIELDG(name, BS, dxf)
#define FIELD_BL(name,dxf) FIELDG(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELDG(name, BLL, dxf)
#define FIELD_BD(name,dxf) FIELDG(name, BD, dxf)
#define FIELD_RC(name,dxf) FIELDG(name, RC, dxf)
#define FIELD_RS(name,dxf) FIELDG(name, RS, dxf)
#define FIELD_RD(name,dxf) FIELDG(name, RD, dxf)
#define FIELD_RL(name,dxf) FIELDG(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELDG(name, RLL, dxf)
#define FIELD_MC(name,dxf) FIELDG(name, MC, dxf)
#define FIELD_MS(name,dxf) FIELDG(name, MS, dxf)
#define FIELD_TF(name,len,dxf)       \
  { _obj->name = bit_read_TF(dat,len); \
    FIELD_G_TRACE(name, TF, dxf); }
#define FIELD_TV(name,dxf) FIELDG(name, TV, dxf);
#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name,dxf) FIELDG(name, BT, dxf);
#define FIELD_4BITS(name,dxf) _obj->name = bit_read_4BITS(dat);

#define FIELD_BE(name,dxf) bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z);
#define FIELD_DD(name, _default, dxf) FIELD_VALUE(name) = bit_read_DD(dat, _default);
#define FIELD_2DD(name, d1, d2, dxf) { FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); }
#define FIELD_2BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); }
#define FIELD_2BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); }
#define FIELD_3RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); \
                              FIELDG(name.z, RD,dxf+20); }
#define FIELD_3BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); \
                              FIELDG(name.z, BD, dxf+20); }
#define FIELD_3BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); \
                                FIELDG(name.z, BD, dxf+2); }
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_TIMEBLL(name,dxf) \
  { _obj->name = bit_read_TIMEBLL(dat);                                  \
    LOG_TRACE(#name ": " FORMAT_BL "." FORMAT_BL "\n", _obj->name.days, _obj->name.ms); }
#define FIELD_CMC(name,dxf) \
  { bit_read_CMC(dat, &_obj->name); \
    LOG_TRACE(#name ": index %d\n", _obj->name.index); }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf) \
  if (size>0)\
    {\
      _obj->name = (BITCODE_##type*) malloc(size * sizeof(BITCODE_##type));\
      for (vcount=0; vcount<(long)size; vcount++) \
        {\
          _obj->name[vcount] = bit_read_##type(dat);\
          LOG_INSANE(#name "[%ld]: " FORMAT_##type "\n", (long)vcount, _obj->name[vcount]) \
        }\
    }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)                                   \
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  for (vcount=0; vcount< (long)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf); \
    }

#define FIELD_2DD_VECTOR(name, size, dxf)                                   \
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  FIELD_2RD(name[0], dxf);                                                  \
  for (vcount = 1; vcount < (long)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf); \
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                               \
  _obj->name = (BITCODE_3DPOINT *) malloc(_obj->size * sizeof(BITCODE_3DPOINT));\
  for (vcount=0; vcount < (long)_obj->size; vcount++) \
    {\
      FIELD_3DPOINT(name[vcount], dxf); \
    }

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  FIELD_VALUE(name) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * size);\
  for (vcount=0; vcount < (long)size; vcount++) \
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);  \
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

//skip non-zero bytes and a terminating zero
#define FIELD_INSERT_COUNT(insert_count, type, dxf) \
      FIELD_VALUE(insert_count)=0; \
      while (bit_read_RC(dat)) \
        {\
          FIELD_VALUE(insert_count)++;\
        }\
      FIELD_G_TRACE(insert_count, type, dxf)

#define FIELD_XDATA(name, size)\
  _obj->name = dwg_decode_xdata(dat, obj->tio.object->tio.XRECORD, _obj->size)

#define REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.object->num_reactors);\
  for (vcount=0; vcount < (long)obj->tio.object->num_reactors; vcount++) \
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);  \
    }

#define ENT_REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.entity->num_reactors);\
  for (vcount=0; vcount < obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);  \
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0);\
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    }

#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0); \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    }

//TODO unify REPEAT macros
#define REPEAT_N(times, name, type) \
  _obj->name = (type *) calloc(times, sizeof(type)); \
  for (rcount=0; rcount<(long)times; rcount++)

#define REPEAT(times, name, type) \
  _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount=0; rcount<(long)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount2=0; rcount2<(long)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount3=0; rcount3<(long)_obj->times; rcount3++)

#define REPEAT4(times, name, type) \
  _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount4=0; rcount4<(long)_obj->times; rcount4++)

#define COMMON_ENTITY_HANDLE_DATA               \
  SINCE(R_13) {\
    dwg_decode_common_entity_handle_data(dat, obj);\
  }

#define DWG_ENTITY(token) static void \
dwg_decode_##token (Bit_Chain * dat, Dwg_Object * obj)\
{\
  long vcount, rcount, rcount2, rcount3, rcount4;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Entity " #token "\n")\
  dwg->num_entities++;\
  obj->supertype = DWG_SUPERTYPE_ENTITY;\
  obj->tio.entity = (Dwg_Object_Entity*)calloc(1, sizeof(Dwg_Object_Entity)); \
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)calloc(1, sizeof (Dwg_Entity_##token)); \
  ent = obj->tio.entity->tio.token;\
  _obj = ent;\
  obj->tio.entity->object = obj;\
  if (dwg_decode_entity (dat, obj->tio.entity)) return;

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) static void \
dwg_decode_ ## token (Bit_Chain * dat, Dwg_Object * obj) \
{ \
  long vcount, rcount, rcount2, rcount3, rcount4; \
  Dwg_Object_##token *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Object " #token "\n")\
  obj->supertype = DWG_SUPERTYPE_OBJECT;\
  obj->tio.object = (Dwg_Object_Object*)calloc (1, sizeof(Dwg_Object_Object)); \
  obj->tio.object->tio.token = (Dwg_Object_##token *)calloc (1, sizeof(Dwg_Object_##token)); \
  obj->tio.object->object = obj;\
  if (dwg_decode_object (dat, obj->tio.object)) return;\
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END }

