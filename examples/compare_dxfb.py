#!/usr/bin/env python3
"""Compare two binary DXF (.dxfb) files by parsing and diffing their records.

Usage: compare_dxfb.py example_2000.dxfb ../td/example_2000.dxb [--diff] [--section SECTION]

Binary DXF format:
  - Each record: 2-byte INT16 group code (LE), then typed value
  - Strings: null-terminated
  - Codes 310-319 (binary): 1-byte count + count bytes (repeated for >127 bytes)
  - Doubles: 8-byte IEEE 754 LE
  - INT16: 2 bytes LE
  - INT32: 4 bytes LE
  - INT64: 8 bytes LE
  - INT8/BOOL: 1 byte
"""

import struct


def dxf_value_type(code):
    """Return (type_str, size) for a given group code.
    size=-1 means null-terminated string, size=-2 means binary (count-prefix)."""
    if 0 <= code <= 9:
        return ("str", -1)
    if 10 <= code <= 59:
        return ("float64", 8)
    if 60 <= code <= 79:
        return ("int16", 2)
    if 80 <= code <= 99:
        return ("int32", 4)
    if 100 <= code <= 109:
        return ("str", -1)  # subclass marker
    if 110 <= code <= 149:
        return ("float64", 8)
    if 160 <= code <= 169:
        return ("int64", 8)
    if 170 <= code <= 179:
        return ("int16", 2)
    if 210 <= code <= 239:
        return ("float64", 8)
    if 270 <= code <= 279:
        return ("int16", 2)
    if 280 <= code <= 289:
        return ("int8", 1)
    if 290 <= code <= 299:
        return ("bool", 1)
    if 300 <= code <= 309:
        return ("str", -1)
    if 310 <= code <= 319:
        return ("binary", -2)
    if 320 <= code <= 369:
        return ("str", -1)  # handles
    if 370 <= code <= 389:
        return ("int16", 2)
    if 390 <= code <= 399:
        return ("str", -1)  # handles
    if 400 <= code <= 409:
        return ("int16", 2)
    if 410 <= code <= 419:
        return ("str", -1)
    if 420 <= code <= 429:
        return ("int32", 4)
    if 430 <= code <= 439:
        return ("str", -1)
    if 440 <= code <= 449:
        return ("int32", 4)
    if 450 <= code <= 459:
        return ("int32", 4)
    if 460 <= code <= 469:
        return ("float64", 8)
    if 470 <= code <= 479:
        return ("str", -1)
    if 480 <= code <= 489:
        return ("str", -1)  # handles
    if code == 999:
        return ("str", -1)
    if 1000 <= code <= 1009:
        return ("str", -1)
    if 1010 <= code <= 1049:
        return ("float64", 8)
    if 1060 <= code <= 1070:
        return ("int16", 2)
    if code == 1071:
        return ("int32", 4)
    # fallback
    return ("str", -1)


def read_record(f, offset):
    """Read one record from binary DXF. Returns (code, value, bytes_read) or None at EOF."""
    code_bytes = f.read(2)
    if len(code_bytes) < 2:
        return None
    code = struct.unpack("<h", code_bytes)[0]
    typ, size = dxf_value_type(code)

    if size == -1:
        # null-terminated string
        buf = bytearray()
        while True:
            b = f.read(1)
            if not b or b == b"\x00":
                break
            buf.extend(b)
        value = buf.decode("latin-1", errors="replace")
        return (code, value, 2 + len(buf) + 1)
    elif size == -2:
        # binary: 1-byte count + count bytes (may repeat)
        chunks = []
        total = 0
        while True:
            cnt_b = f.read(1)
            if not cnt_b:
                break
            cnt = cnt_b[0]
            total += 1
            if cnt > 0:
                data = f.read(cnt)
                chunks.append(data)
                total += cnt
            if cnt < 127:
                break
            # read another chunk for >127
            code2_bytes = f.read(2)
            if len(code2_bytes) < 2:
                break
            code2 = struct.unpack("<h", code2_bytes)[0]
            if code2 != code:
                # put back and stop
                f.seek(-2, 1)
                break
            total += 2
        value = b"".join(chunks)
        return (code, value, 2 + total)
    elif typ == "float64":
        data = f.read(8)
        if len(data) < 8:
            return None
        value = struct.unpack("<d", data)[0]
        return (code, value, 10)
    elif typ == "int64":
        data = f.read(8)
        if len(data) < 8:
            return None
        value = struct.unpack("<q", data)[0]
        return (code, value, 10)
    elif typ == "int32":
        data = f.read(4)
        if len(data) < 4:
            return None
        value = struct.unpack("<i", data)[0]
        return (code, value, 6)
    elif typ == "int16":
        data = f.read(2)
        if len(data) < 2:
            return None
        value = struct.unpack("<h", data)[0]
        return (code, value, 4)
    elif typ in ("int8", "bool"):
        data = f.read(1)
        if not data:
            return None
        value = data[0]
        return (code, value, 3)
    else:
        # unknown: null-terminated fallback
        buf = bytearray()
        while True:
            b = f.read(1)
            if not b or b == b"\x00":
                break
            buf.extend(b)
        value = buf.decode("latin-1", errors="replace")
        return (code, value, 2 + len(buf) + 1)


def parse_dxfb(filename):
    """Parse a binary DXF file. Returns list of (offset, code, value) tuples."""
    records = []
    with open(filename, "rb") as f:
        # Check for binary DXF sentinel
        header = f.read(22)
        if header != b"AutoCAD Binary DXF\r\n\x1a\x00":
            # Not standard binary header, seek back to start
            f.seek(0)

        offset = f.tell()
        while True:
            rec = read_record(f, offset)
            if rec is None:
                break
            code, value, nbytes = rec
            records.append((offset, code, value))
            offset += nbytes

    return records


def split_sections(records):
    """Split records into sections by SECTION/ENDSEC markers."""
    sections = {}
    current_name = None
    current = []
    i = 0
    while i < len(records):
        off, code, val = records[i]
        if code == 0 and val == "SECTION":
            if i + 1 < len(records) and records[i + 1][1] == 2:
                current_name = records[i + 1][2]
                current = [records[i], records[i + 1]]
                i += 2
                continue
        elif code == 0 and val == "ENDSEC":
            if current_name:
                current.append(records[i])
                sections[current_name] = current
                current_name = None
                current = []
        elif current_name:
            current.append(records[i])
        i += 1
    # EOF record
    if current:
        sections["__tail__"] = current
    return sections


def format_value(code, value):
    """Format a value for display."""
    typ, _ = dxf_value_type(code)
    if isinstance(value, bytes):
        if len(value) <= 16:
            return value.hex()
        else:
            return value[:16].hex() + f"...({len(value)} bytes)"
    if isinstance(value, float):
        return f"{value:.6g}"
    return repr(value)


def compare_sections(secs1, secs2, show_diff=False, target_section=None):
    """Compare sections between two files."""
    all_sections = sorted(set(list(secs1.keys()) + list(secs2.keys())))

    for sname in all_sections:
        if target_section and sname != target_section:
            continue
        r1 = secs1.get(sname, [])
        r2 = secs2.get(sname, [])
        diff = len(r2) - len(r1)
        diff_str = f"{diff:+d}" if diff != 0 else "="
        print(f"  {sname}: {len(r1)} vs {len(r2)} records ({diff_str})")

        if show_diff and diff != 0:
            print("    --- file1 ---")
            # Find first difference
            for i in range(min(len(r1), len(r2))):
                off1, c1, v1 = r1[i]
                off2, c2, v2 = r2[i]
                if c1 != c2 or v1 != v2:
                    print(f"    First diff at record {i}:")
                    print(
                        f"      file1 @ 0x{off1:x}: code={c1} val={format_value(c1, v1)}"
                    )
                    print(
                        f"      file2 @ 0x{off2:x}: code={c2} val={format_value(c2, v2)}"
                    )
                    # Show context
                    for j in range(max(0, i - 2), min(len(r1), i + 5)):
                        off, c, v = r1[j]
                        marker = ">" if j == i else " "
                        print(
                            f"    {marker} [{j}] file1 @ 0x{off:x}: code={c} val={format_value(c, v)}"
                        )
                    print("    ---")
                    for j in range(max(0, i - 2), min(len(r2), i + 5)):
                        off, c, v = r2[j]
                        marker = ">" if j == i else " "
                        print(
                            f"    {marker} [{j}] file2 @ 0x{off:x}: code={c} val={format_value(c, v)}"
                        )
                    break


def dump_section(records, section_name, start=0, count=50):
    """Dump records from a section."""
    secs = split_sections(records)
    sec = secs.get(section_name, [])
    end = min(start + count, len(sec))
    for i in range(start, end):
        off, code, val = sec[i]
        print(f"  [{i}] 0x{off:x}: code={code} val={format_value(code, val)}")


def main():
    import argparse

    parser = argparse.ArgumentParser(description="Compare two binary DXF files")
    parser.add_argument("file1", help="First .dxfb file (reference)")
    parser.add_argument("file2", nargs="?", help="Second .dxfb file (to compare)")
    parser.add_argument(
        "--diff", action="store_true", help="Show first difference in each section"
    )
    parser.add_argument(
        "--section",
        help="Focus on specific section (HEADER, CLASSES, TABLES, BLOCKS, ENTITIES, OBJECTS)",
    )
    parser.add_argument(
        "--dump",
        type=int,
        default=0,
        help="Dump N records from section (use with --section)",
    )
    parser.add_argument("--start", type=int, default=0, help="Start record for --dump")
    args = parser.parse_args()

    print(f"Parsing {args.file1}...")
    records1 = parse_dxfb(args.file1)
    print(f"  {len(records1)} records total")
    secs1 = split_sections(records1)

    if args.file2 is None:
        # Just dump info about file1
        print("\nSections:")
        for sname, recs in sorted(secs1.items()):
            print(f"  {sname}: {len(recs)} records")
        if args.section and args.dump:
            print(
                f"\nDumping {args.dump} records from {args.section} starting at {args.start}:"
            )
            dump_section(records1, args.section, args.start, args.dump)
        return

    print(f"Parsing {args.file2}...")
    records2 = parse_dxfb(args.file2)
    print(f"  {len(records2)} records total")
    secs2 = split_sections(records2)

    print(
        f"\nTotal records: {len(records1)} (file1) vs {len(records2)} (file2) ({len(records2) - len(records1):+d})"
    )
    print("\nSection comparison:")
    compare_sections(secs1, secs2, show_diff=args.diff, target_section=args.section)

    if args.section and args.dump:
        print(f"\nDumping file1 {args.section} from {args.start}:")
        dump_section(records1, args.section, args.start, args.dump)
        if args.file2:
            print(f"\nDumping file2 {args.section} from {args.start}:")
            dump_section(records2, args.section, args.start, args.dump)


if __name__ == "__main__":
    main()
