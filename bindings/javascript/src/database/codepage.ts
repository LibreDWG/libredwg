export enum DwgCodePage {
  CP_UTF8 = 0,
  CP_US_ASCII = 1,
  CP_ISO_8859_1,
  CP_ISO_8859_2,
  CP_ISO_8859_3,
  CP_ISO_8859_4,
  CP_ISO_8859_5,
  CP_ISO_8859_6,
  CP_ISO_8859_7,
  CP_ISO_8859_8,
  CP_ISO_8859_9,
  CP_CP437, // DOS English
  CP_CP850, // 12 DOS Latin-1
  CP_CP852, // DOS Central European
  CP_CP855, // DOS Cyrillic
  CP_CP857, // DOS Turkish
  CP_CP860, // DOS Portoguese
  CP_CP861, // DOS Icelandic
  CP_CP863, // DOS Hebrew
  CP_CP864, // DOS Arabic (IBM)
  CP_CP865, // DOS Nordic
  CP_CP869, // DOS Greek
  CP_CP932, // DOS Japanese (shiftjis)
  CP_MACINTOSH, // 23
  CP_BIG5,
  CP_CP949 = 25, // Korean (Wansung + Johab)
  CP_JOHAB = 26, // Johab?
  CP_CP866 = 27, // Russian
  CP_ANSI_1250 = 28, // Central + Eastern European
  CP_ANSI_1251 = 29, // Cyrillic
  CP_ANSI_1252 = 30, // Western European
  CP_GB2312 = 31, // EUC-CN Chinese
  CP_ANSI_1253, // Greek
  CP_ANSI_1254, // Turkish
  CP_ANSI_1255, // Hebrew
  CP_ANSI_1256, // Arabic
  CP_ANSI_1257, // Baltic
  CP_ANSI_874, // Thai
  CP_ANSI_932, // 38 Japanese (extended shiftjis, windows-31j)
  CP_ANSI_936, // 39 Simplified Chinese
  CP_ANSI_949, // 40 Korean Wansung
  CP_ANSI_950, // 41 Trad Chinese
  CP_ANSI_1361, // 42 Korean Wansung
  CP_UTF16 = 43,
  CP_ANSI_1258 = 44, // Vietnamese
  CP_UNDEFINED = 0xff // mostly R11
}

const encodings = [
  'utf-8', // 0
  'utf-8', // US ASCII
  'iso-8859-1',
  'iso-8859-2',
  'iso-8859-3',
  'iso-8859-4',
  'iso-8859-5',
  'iso-8859-6',
  'iso-8859-7',
  'iso-8859-8',
  'iso-8859-9', // 10
  'utf-8', // DOS English
  'utf-8', // 12 DOS Latin-1
  'utf-8', // DOS Central European
  'utf-8', // DOS Cyrillic
  'utf-8', // DOS Turkish
  'utf-8', // DOS Portoguese
  'utf-8', // DOS Icelandic
  'utf-8', // DOS Hebrew
  'utf-8', // DOS Arabic (IBM)
  'utf-8', // DOS Nordic
  'utf-8', // DOS Greek
  'shift-jis', // DOS Japanese (shiftjis)
  'macintosh', // 23
  'big5',
  'utf-8', // Korean (Wansung + Johab)
  'utf-8', // Johab?
  'ibm866', // Russian
  'windows-1250', // Central + Eastern European
  'windows-1251', // Cyrillic
  'windows-1252', // Western European
  'gbk', // EUC-CN Chinese
  'windows-1253', // Greek
  'windows-1254', // Turkish
  'windows-1255', // Hebrew
  'windows-1256', // Arabic
  'windows-1257', // Baltic
  'windows-874', // Thai
  'shift-jis', // 38 Japanese (extended shiftjis, windows-31j)
  'gbk', // 39 Simplified Chinese
  'euc-kr', // 40 Korean Wansung
  'big5', // 41 Trad Chinese
  'utf-8', // 42 Korean Wansung
  'utf-16le',
  'windows-1258' // Vietnamese
]

export const dwgCodePageToEncoding = (codepage: DwgCodePage) => {
  return encodings[codepage]
}
