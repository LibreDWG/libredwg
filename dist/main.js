import {
  Dwg_File_Type,
  Dwg_Object_Type_Inverted,
  extend_lib
} from "./utils.mjs";

// load libredwg webassembly module
const libredwg = await createModule();
extend_lib(libredwg);
window.libredwg = libredwg;

const printItems = (id, size, getItem, getPropVal, propName = 'name') => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  // Create list item for item
  for (let index = 0; index < size; ++index) {
    const item = getItem(index);
    const li = document.createElement('li');
    li.textContent = `${getPropVal(item, propName)}`;
    listElement.appendChild(li);
  }
}

const printItemsByDynApi = (id, items, propName = 'name') => {
  printItems(
    id,
    items.length,
    (index) => items[index],
    (item, propName) => {
      const result = libredwg.dwg_dynapi_entity_value(item, propName);
      return result.data;
    },
    propName
  );
}

const printAllItems = (libredwg, data) => {
  const objects = [{
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

const printEntityInfo = (id, entity) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const propNames = ["handle", "layer", "lineType", "colorName", "proxyGraphics", "extPoint"];
  propNames.forEach((name) => {
    const li = document.createElement('li');
    if (name == "extPoint") {
      const coord = entity[name]
      if (coord) {
        li.textContent = `${name}: (${coord.x}, ${coord.y}, ${coord.z})`;
        listElement.appendChild(li);
      }
    } else {
      li.textContent = `${name}: ${entity[name]}`;
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

        printItems(
          'layerNameList', 
          libredwg.dwg_get_layer_count(data),
          (index) => libredwg.dwg_get_layer_index(data, index),
          (item, propName) => libredwg.dwg_obj_layer_get_name(item, propName)
        );

        printAllItems(libredwg, data);

        const entities = libredwg.dwg_getall_entitie_in_model_space(data);
        printEntityStats('entityList', libredwg, entities)

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