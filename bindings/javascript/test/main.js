import {
  Dwg_File_Type,
  Dwg_Object_Type_Inverted,
  LibreDwg
} from './libredwg-web.js';

// load libredwg webassembly module
const libredwg = await LibreDwg.create();
window.libredwg = libredwg;

const printItems = (id, size, getItemContent) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  // Create list item for item
  for (let index = 0; index < size; ++index) {
    const li = document.createElement('li');
    li.textContent = getItemContent(index);
    listElement.appendChild(li);
  }
}

const printItemsByDynApi = (id, items, propName = 'name') => {
  printItems(
    id,
    items.length,
    (index) => {
      const item = items[index];
      const result = libredwg.dwg_dynapi_entity_value(item, propName);
      return `${result.data}`;
    }
  );
}

const printAllItems = (libredwg, data) => {
  const objects = [{
      id: 'layerNameList',
      getAll: libredwg.dwg_getall_LAYER,
      propName: 'name'
    }, {
      id: 'lineTypeList',
      getAll: libredwg.dwg_getall_LTYPE,
      propName: 'name'
    }, {
      id: 'textStyleList',
      getAll: libredwg.dwg_getall_STYLE,
      propName: 'name'
    }, {
      id: 'dimStyleList',
      getAll: libredwg.dwg_getall_DIMSTYLE,
      propName: 'name'
    }, {
      id: 'viewportList',
      getAll: libredwg.dwg_getall_VPORT,
      propName: 'name'
    }, {
      id: 'layoutList',
      getAll: libredwg.dwg_getall_LAYOUT,
      propName: 'layout_name'
    }, {
      id: 'blockList',
      getAll: libredwg.dwg_getall_BLOCK_HEADER,
      propName: 'name'
    }
  ];

  objects.forEach((obj) => {
    const items = obj.getAll.call(libredwg, data);
    printItemsByDynApi(obj.id, items, obj.propName);
  })
}

const printImages = (id, libredwg, data) => {
  const images = libredwg.dwg_getall_IMAGE(data);
  const imageDefs = libredwg.dwg_getall_IMAGEDEF(data);

  const files = new Map();
  imageDefs.forEach((imageDef) => {
    const dwg_obj = libredwg.dwg_obj_generic_to_object(imageDef);
    const handle = libredwg.dwg_object_get_handle_object(dwg_obj);
    const fileName = libredwg.dwg_dynapi_entity_value(imageDef, 'file_path').data;
    files.set(handle.value, fileName);
  });

  printItems(
    id,
    images.length,
    (index) => {
      const item = images[index];
      const point = libredwg.dwg_dynapi_entity_value(item, 'pt0').data;
      const imagedef_ref = libredwg.dwg_dynapi_entity_value(item, 'imagedef').data;
      const imagedef_absref = libredwg.dwg_ref_get_absref(imagedef_ref);
      const obj = libredwg.dwg_absref_get_object(data, imagedef_absref);
      const imagedef = libredwg.dwg_object_to_object_tio(obj);
      const filename = libredwg.dwg_dynapi_entity_value(imagedef, 'file_path').data;
      return `insert point: {${point.x}, ${point.y}, ${point.z}}, file: ${filename}`;
    }
  );
}

const printPolylineInfo = (id, polyline) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const commonPropNames = ['objid', 'ownerhandle', 'layer', 'invisible', 'ltype', 'linewt', 'color'];
  commonPropNames.forEach((name) => {
    const li = document.createElement('li');
    const result = libredwg.dwg_dynapi_common_value(polyline, name);
    if (name == 'color') {
      const color = result.data;
      li.textContent = `color: -index: ${color.index}, -flag: ${color.flag}, -rgb: ${color.rgb}, -name: ${color.name}, -book_name: ${color.book_name}`;
      listElement.appendChild(li);
    } else {
      li.textContent = `${name}: ${result.data}`;
      listElement.appendChild(li);
    }
  });

  const polylinePropNames = ['flag', 'elevation', 'thickness', 'extrusion', 'num_points', 'num_bulges'];
  polylinePropNames.forEach((name) => {
    const li = document.createElement('li');
    const result = libredwg.dwg_dynapi_entity_value(polyline, name);
    li.textContent = `${name}: ${result.data}`;
    listElement.appendChild(li);
  });

  const num_points = libredwg.dwg_dynapi_entity_value(polyline, 'num_points').data;
  const points_ptr = libredwg.dwg_dynapi_entity_value(polyline, 'points').data;
  const points = libredwg.dwg_ptr_to_point2d_array(points_ptr, num_points);
  const num_bulges = libredwg.dwg_dynapi_entity_value(polyline, 'num_bulges').data;
  const bulges_ptr = libredwg.dwg_dynapi_entity_value(polyline, 'bulges').data;
  const bulges = libredwg.dwg_ptr_to_double_array(bulges_ptr, num_bulges);
  points.forEach((point, index) => {
    const li = document.createElement('li');
    li.textContent = ` - (x: ${point.x}, y: ${point.y}, bulge: ${bulges[index] ? bulges[index] : 0})`;
    listElement.appendChild(li);
  })
}

const printEntityStats = (id, libredwg, entities) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const group = new Map();
  entities.forEach((entity) => {
    const type = libredwg.dwg_object_get_fixedtype(entity);
    const typeName = Dwg_Object_Type_Inverted[type.toString()]

    if (!group.has(typeName)) {
      group.set(typeName, 0);
    }

    // Increment the count for the current group
    group.set(typeName, group.get(typeName) + 1);
  });

  // Create list item for item
  group.forEach((value, key) => {
    const li = document.createElement('li');
    li.textContent = `${key}: ${value}`;
    listElement.appendChild(li);
  });
}

const fileInput = document.getElementById('fileInput');

// Function to handle file input change event
fileInput.addEventListener('change', function(event) {
  const file = event.target.files[0];
  
  if (file) {
    // Get file extension
    const fileExtension = file.name.split('.').pop().toLowerCase();

    // Create a FileReader to read the file
    const reader = new FileReader();

    // Define the callback function for when the file is read
    reader.onload = function(e) {
      const fileContent = e.target.result;
      try {
        let fileType = undefined;
        if (fileExtension == 'dxf') {
          fileType = Dwg_File_Type.DXF;
        } else if (fileExtension == 'dwg') {
          fileType = Dwg_File_Type.DWG;
        }
        const data = libredwg.dwg_read_data(fileContent, fileType);
        // console.log('LIMMAX: ', libredwg.dwg_dynapi_header_value(data, 'LIMMAX').data);

        const layerCount = libredwg.dwg_get_layer_count(data);
        for (let index = 0; index < layerCount; index++) {
          const layer = libredwg.dwg_get_layer_index(data, index);
          libredwg.dwg_obj_layer_get_name(layer, 'name');
        }

        printAllItems(libredwg, data);

        const entities = libredwg.dwg_getall_entitie_in_model_space(data);
        printEntityStats('entityList', libredwg, entities);

        const polylines = libredwg.dwg_getall_LWPOLYLINE(data);
        if (polylines.length > 0) {
          printPolylineInfo('entityInfoList', polylines[0]);
        }

        printImages('imageList', libredwg, data);

        libredwg.dwg_free(data);
      } catch (error) {
        console.error('Error processing DXF/DWG file: ', error);
      }
    };

    // Read the file
    if (fileExtension == 'dxf') {
      reader.readAsText(file);
    } else if (fileExtension == 'dwg') {
      reader.readAsArrayBuffer(file);
    }
  } else {
    convertButton.disabled = true;
    console.log('No file selected');
  }
});