import sys

def bmp_to_c_array(bmp_file, array_name="image"):
    with open(bmp_file, "rb") as f:
        data = f.read()

    print(f"const unsigned int {array_name}_len = {len(data)};")
    print(f"const unsigned char {array_name}[] = {{")

    line = "  "
    for i, b in enumerate(data):
        line += f"0x{b:02X}, "
        if (i + 1) % 12 == 0:
            print(line)
            line = "  "
    if line.strip():
        print(line)

    print("};")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python bmp_to_c_array.py input.bmp > output.c")
        sys.exit(1)

    bmp_to_c_array(sys.argv[1])
