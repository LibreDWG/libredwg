import {
  Dwg_File_Type,
  Dwg_Object_Type_Inverted,
  extend_lib
} from "./utils.mjs";

// load libredwg webassembly module
const libredwg = await createModule();
extend_lib(libredwg);
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
    const items = obj.getAll(data);
    printItemsByDynApi(obj.id, items, obj.propName);
  })
}

const printImages = (id, libredwg, data) => {
  const images = libredwg.dwg_getall_IMAGE(data);
  printItems(
    id,
    images.length,
    (index) => {
      const item = images[index];
      const point = libredwg.dwg_dynapi_entity_value(item, 'pt0').data;
      const imagedef_ref = libredwg.dwg_dynapi_entity_value(item, 'imagedef').data;
      const obj = libredwg.dwg_ref_object_silent(data, imagedef_ref);
      const imagedef = libredwg.dwg_object_get_tio(obj);
      const filename = libredwg.dwg_dynapi_entity_value(imagedef, 'file_path');
      return `insert point: {${point.x}, ${point.y}, ${point.z}}, file: ${filename}`;
    }
  );
}

const printEntityInfo = (id, polyline) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const propNames = ["objid", "ownerhandle", "layer", "invisible", "ltype", "linewt", "color"];
  propNames.forEach((name) => {
    const li = document.createElement('li');
    const result = libredwg.dwg_dynapi_common_value(polyline, name);
    if (name == "color") {
      const color = result.data;
      li.textContent = `index: ${color.index}, flag: ${color.flag}, rgb: ${color.rgb}, name: ${color.name}, book_name: ${color.book_name}`;
      listElement.appendChild(li);
    } else {
      li.textContent = `${name}: ${result.data}`;
      listElement.appendChild(li);
    }
  });
}

const printVertexesInTheFirstPolyline = (id, polyline) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const vertexes = polyline.getVertexList();
  for (let index = 0, size = vertexes.size(); index < size; ++index) {
    const vertex = vertexes.get(index);
    const li = document.createElement('li');
    li.textContent = `(x: ${vertex.x}, y: ${vertex.y}, bulge: ${vertex.bulge})`;
    listElement.appendChild(li);
  }
}

const printEntityStats = (id, libredwg, entities) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const group = new Map();
  entities.forEach((entity) => {
    const type = libredwg.dwg_object_get_fixedtype(entity);
    const typeName = Dwg_Object_Type_Inverted[type.toString()]
    // console.log('type: ', type, ', typeName : ', typeName);

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
        console.log('LIMMAX: ', libredwg.dwg_dynapi_header_value(data, 'LIMMAX').data);

        const layerCount = libredwg.dwg_get_layer_count(data);
        for (let index = 0; index < layerCount; index++) {
          const layer = libredwg.dwg_get_layer_index(data, index);
          libredwg.dwg_obj_layer_get_name(layer, 'name');
        }

        printAllItems(libredwg, data);

        const entities = libredwg.dwg_getall_entitie_in_model_space(data);
        printEntityStats('entityList', libredwg, entities);

        const polylines = libredwg.dwg_getall_POLYLINE_3D(data);
        if (polylines.length > 0) {
          printEntityInfo('entityInfoList', polylines[0]);
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