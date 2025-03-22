import {
  dwg_getall_DIMSTYLE,
  dwg_getall_LTYPE,
  dwg_getall_STYLE,
  dwg_getall_VPORT,
  dwg_getall_LAYOUT,
  dwg_read_data
} from "./utils.mjs";

// load libredwg webassembly module
const libredwg = await createModule();
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
  );
}

const printAllItems = (data) => {
  const ids = [
    'lineTypeList',
    'textStyleList',
    'dimStyleList',
    'viewportList',
    'layoutList'
  ];
  const callbacks = [
    dwg_getall_LTYPE,
    dwg_getall_STYLE,
    dwg_getall_DIMSTYLE,
    dwg_getall_VPORT,
    dwg_getall_LAYOUT
  ];

  ids.forEach((id, index) => {
    const items = callbacks[index](libredwg, data);
    printItemsByDynApi(id, items);
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

const printEntityStats = (id, entities) => {
  // Clear previous item list
  const listElement = document.getElementById(id);
  listElement.innerHTML = '';

  const group = new Map();
  let isFoundPolyline = false;
  for (let index = 0, size = entities.size(); index < size; ++index) {
    const entity = entities.get(index);
    // If the group for the given eType does not exist, initialize the count to 0
    const typeName = entity.eType.constructor.name;
    if (!group.has(typeName)) {
      group.set(typeName, 0);
    }
    if (!isFoundPolyline && (entity.eType == libredwg.DRW_ETYPE.LWPOLYLINE || entity.eType == libredwg.DRW_ETYPE.POLYLINE)) {
      printEntityInfo("entityInfoList", entity);
      printVertexesInTheFirstPolyline("vertexList", entity);
      isFoundPolyline = true;
    }
    // Increment the count for the current group
    group.set(typeName, group.get(typeName) + 1);
  }

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
        if (fileExtension == 'dxf') {
            // Do nothing for now
        } else if (fileExtension == 'dwg') {
          const data = dwg_read_data(libredwg, fileContent);
          console.log('LIMMAX: ', libredwg.dwg_dynapi_header_value(data, 'LIMMAX').data);

          printItems(
            'layerNameList', 
            libredwg.dwg_get_layer_count(data),
            (index) => libredwg.dwg_get_layer_index(data, index),
            (item, propName) => libredwg.dwg_obj_layer_get_name(item, propName)
          );

          printAllItems(data);

          // const ltypes = dwg_getall_LTYPE(libredwg, data);
          // printItemsByDynApi('lineTypeList', ltypes);

          // const styles = dwg_getall_STYLE(libredwg, data);
          // printItemsByDynApi('textStyleList', styles);

          // const dimStyles = dwg_getall_DIMSTYLE(libredwg, data);
          // printItemsByDynApi('dimStyleList', dimStyles);

          // Manually signal that a C++ object is no longer needed and can be deleted.
          // data.delete();
        }
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