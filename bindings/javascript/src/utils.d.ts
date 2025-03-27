import { MainModule } from '../wasm/libredwg-web'

export const Dwg_File_Type: {
  DWG: number;
  DXF: number;
};

export interface LibreDwg extends MainModule {
  dwg_read_data(fileContent: string | ArrayBuffer, fileType: number);
}

export function extend_lib(lib: MainModule): void;
