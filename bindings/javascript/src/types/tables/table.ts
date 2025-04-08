export interface DwgCommonTableEntry {
  name: string
  handle: number
  ownerHandle: number
}

export interface DwgTable<T extends DwgCommonTableEntry> {
  // name: string;
  // handle: string;
  // ownerDictionaryIds?: string[];
  // ownerObjectId: string;
  // maxNumberOfEntries: number;
  entries: T[]
}
