import sys

def file_to_header(input_file, output_h_file, var_name):
    try:
        with open(input_file, "rb") as f:
            data = f.read()
        
        # Format bytes with a line break every 12 elements for readability
        hex_bytes = [f"0x{b:02x}" for b in data]
        formatted_bytes = ""
        for i in range(0, len(hex_bytes), 12):
            formatted_bytes += "    " + ", ".join(hex_bytes[i:i+12]) + ",\n"
        
        content = [
            "#pragma once\n",
            f"constexpr unsigned char {var_name}[] = {{\n{formatted_bytes}}};\n",
            f"constexpr unsigned int {var_name}_len = {len(data)};"
        ]
        
        with open(output_h_file, "w") as f:
            f.write("\n".join(content))
            
        print(f"Done! File saved as: {output_h_file}")
        
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Check if the user provided enough command-line arguments
    if len(sys.argv) < 4:
        print("Usage: python script.py <input_file> <output_h_file> <variable_name>")
        print("Example: python convert.py image.png resource.h my_image")
    else:
        # sys.argv[0] is the script name, so we take arguments from 1 to 3
        file_to_header(sys.argv[1], sys.argv[2], sys.argv[3])