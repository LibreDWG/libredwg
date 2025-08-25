export interface DwgVersion {
  type: string
  hdr: string
  description: string
  version: number
}

export const dwgVersions: DwgVersion[] = [
  {
    type: 'invalid',
    hdr: 'INVALI',
    description: 'No DWG',
    version: 0
  },
  {
    type: 'r1.1',
    hdr: 'MC0.0',
    description: 'MicroCAD Release 1.1',
    version: 0
  },
  {
    type: 'r1.2',
    hdr: 'AC1.2',
    description: 'AutoCAD Release 1.2',
    version: 0
  },
  {
    type: 'r1.3',
    hdr: 'AC1.3',
    description: 'AutoCAD Release 1.3',
    version: 1
  },
  {
    type: 'r1.4',
    hdr: 'AC1.40',
    description: 'AutoCAD Release 1.4',
    version: 2
  },
  { type: 'r2.0b', hdr: 'AC1.50', description: 'AutoCAD 2.0 beta', version: 3 }, // not seen
  {
    type: 'r2.0',
    hdr: 'AC1.50',
    description: 'AutoCAD Release 2.0',
    version: 4
  },
  {
    type: 'r2.10',
    hdr: 'AC2.10',
    description: 'AutoCAD Release 2.10',
    version: 5
  },
  {
    type: 'r2.21',
    hdr: 'AC2.21',
    description: 'AutoCAD Release 2.21',
    version: 6
  },
  {
    type: 'r2.22',
    hdr: 'AC2.22',
    description: 'AutoCAD Release 2.22',
    version: 7
  },
  {
    type: 'r2.4',
    hdr: 'AC1001',
    description: 'AutoCAD Release 2.4',
    version: 8
  },
  {
    type: 'r2.5',
    hdr: 'AC1002',
    description: 'AutoCAD Release 2.5',
    version: 9
  },
  {
    type: 'r2.6',
    hdr: 'AC1003',
    description: 'AutoCAD Release 2.6',
    version: 10
  },
  { type: 'r9', hdr: 'AC1004', description: 'AutoCAD Release 9', version: 0xb },
  {
    type: 'r9c1',
    hdr: 'AC1005',
    description: 'AutoCAD Release 9c1',
    version: 0xc
  },
  {
    type: 'r10',
    hdr: 'AC1006',
    description: 'AutoCAD Release 10',
    version: 0xd
  },
  {
    type: 'r11b1',
    hdr: 'AC1007',
    description: 'AutoCAD 11 beta 1',
    version: 0xe
  },
  {
    type: 'r11b2',
    hdr: 'AC1008',
    description: 'AutoCAD 11 beta 2',
    version: 0xf
  },
  {
    type: 'r11',
    hdr: 'AC1009',
    description: 'AutoCAD Release 11/12 (LT R1/R2)',
    version: 0x10
  },
  {
    type: 'r13b1',
    hdr: 'AC1010',
    description: 'AutoCAD pre-R13 a',
    version: 0x11
  },
  {
    type: 'r13b2',
    hdr: 'AC1011',
    description: 'AutoCAD pre-R13 b',
    version: 0x12
  },
  {
    type: 'r13',
    hdr: 'AC1012',
    description: 'AutoCAD Release 13',
    version: 0x13
  },
  {
    type: 'r13c3',
    hdr: 'AC1013',
    description: 'AutoCAD Release 13c3',
    version: 0x14
  },
  {
    type: 'r14',
    hdr: 'AC1014',
    description: 'AutoCAD Release 14',
    version: 0x15
  },
  {
    type: 'r2000b',
    hdr: 'AC1500',
    description: 'AutoCAD 2000 beta',
    version: 0x16
  },
  {
    type: 'r2000',
    hdr: 'AC1015',
    description: 'AutoCAD Release 2000',
    version: 0x17
  },
  {
    type: 'r2000i',
    hdr: 'AC1016',
    description: 'AutoCAD Release 2000i',
    version: 0x17
  },
  {
    type: 'r2002',
    hdr: 'AC1017',
    description: 'AutoCAD Release 2002',
    version: 0x17
  },
  {
    type: 'r2004a',
    hdr: 'AC402a',
    description: 'AutoCAD 2004 alpha a',
    version: 0x18
  },
  {
    type: 'r2004b',
    hdr: 'AC402b',
    description: 'AutoCAD 2004 alpha b',
    version: 0x18
  },
  {
    type: 'r2004c',
    hdr: 'AC1018',
    description: 'AutoCAD 2004 beta',
    version: 0x18
  },
  // (includes versions AC1019/0x19 and AC1020/0x1a)
  {
    type: 'r2004',
    hdr: 'AC1018',
    description: 'AutoCAD Release 2004',
    version: 0x19
  },
  //{ type: "r2005", hdr: "AC1019", description: "AutoCAD 2005", version: 0x19 }, // not seen
  //{ type: "r2006", hdr: "AC1020", description: "AutoCAD 2006", version: 0x19 }, // not seen
  {
    type: 'r2007a',
    hdr: 'AC701a',
    description: 'AutoCAD 2007 alpha',
    version: 0x1a
  },
  {
    type: 'r2007b',
    hdr: 'AC1021',
    description: 'AutoCAD 2007 beta',
    version: 0x1a
  },
  {
    type: 'r2007',
    hdr: 'AC1021',
    description: 'AutoCAD Release 2007',
    version: 0x1b
  },
  //{ type: "r2008", hdr: "AC1022", description: "AutoCAD 2008", version: 0x1b }, // not seen
  //{ type: "r2009", hdr: "AC1023", description: "AutoCAD 2009", version: 0x1b }, // not seen
  {
    type: 'r2010b',
    hdr: 'AC1024',
    description: 'AutoCAD 2010 beta',
    version: 0x1c
  },
  {
    type: 'r2010',
    hdr: 'AC1024',
    description: 'AutoCAD Release 2010',
    version: 0x1d
  },
  //{ type: "r2011", hdr: "AC1025", description: "AutoCAD 2011", version: 0x1d }, // not seen
  //{ type: "r2012", hdr: "AC1026", description: "AutoCAD 2012", version: 0x1e }, // not seen
  {
    type: 'r2013b',
    hdr: 'AC1027',
    description: 'AutoCAD 2013 beta',
    version: 0x1e
  },
  {
    type: 'r2013',
    hdr: 'AC1027',
    description: 'AutoCAD Release 2013',
    version: 0x1f
  },
  //{ type: "r2014", hdr: "AC1028", description: "AutoCAD 2014", version: 0x1f }, // not seen
  //{ type: "r2015", hdr: "AC1029", description: "AutoCAD 2015", version: 0x1f }, // not seen
  //{ type: "r2016", hdr: "AC1030", description: "AutoCAD 2016", version: 0x1f }, // not seen
  //{ type: "r2017", hdr: "AC1031", description: "AutoCAD 2017", version: 0x20 }, // not seen
  {
    type: 'r2018b',
    hdr: 'AC1032',
    description: 'AutoCAD 2018 beta',
    version: 0x20
  },
  {
    type: 'r2018',
    hdr: 'AC1032',
    description: 'AutoCAD Release 2018',
    version: 0x21
  },
  //{ type: "r2019", "AC1033", description: "AutoCAD Release 2019", version: 0x22 }, // not seen
  //{ type: "r2020", "AC1034", description: "AutoCAD Release 2020", version: 0x23 }, // not seen
  //{ type: "r2021", "AC1035", description: "AutoCAD Release 2021", version: 0x23 }, // not seen
  {
    type: 'r2022b',
    hdr: 'AC103-4',
    description: 'AutoCAD 2022 beta',
    version: 0x24
  },
  { type: 'r>2022', hdr: '', description: 'AutoCAD Release >2022', version: 0 }
]
