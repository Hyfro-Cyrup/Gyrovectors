"""
Records the file structure of GyroGPU and makes .vcxproj.filters match
"""
import os
import xml.etree.ElementTree as ET

PROJECT_NAME = "GyroGPU"  # just for the root Filter name if you want
ROOT_DIR = r"GyroGPU"  # project root folder
FILTERS_FILE = os.path.join(ROOT_DIR, f"{PROJECT_NAME}.vcxproj.filters")
PROJECT_FILE = os.path.join(ROOT_DIR, f"{PROJECT_NAME}.vcxproj")

def get_relative_path(path):
    return os.path.relpath(path, ROOT_DIR).replace("/", "\\")

def collect_files():
    cpp_files = []
    h_files = []
    other_files = []
    image_files = []
    filters_set = set()

    for dirpath, _, filenames in os.walk(ROOT_DIR):
        if "External" in dirpath:
            continue
        if "x64" in dirpath:
            continue
        if "Debug" in dirpath:
            continue
        for fname in filenames:
            if "vcxproj" in fname:
                continue
            if ".dll" in fname:
                continue
            if fname.lower().endswith(".cpp") or \
                fname.lower().endswith(".tpp"):
                full = os.path.join(dirpath, fname)
                cpp_files.append(full)
                filters_set.add(os.path.relpath(dirpath, ROOT_DIR))
            elif fname.lower().endswith(".png"):
                full = os.path.join(dirpath, fname)
                image_files.append(full)
                filters_set.add(os.path.relpath(dirpath, ROOT_DIR))
            elif fname.lower().endswith(".h"):
                full = os.path.join(dirpath, fname)
                h_files.append(full)
                filters_set.add(os.path.relpath(dirpath, ROOT_DIR))
            else:
                full = os.path.join(dirpath, fname)
                other_files.append(full)
                filters_set.add(os.path.relpath(dirpath, ROOT_DIR))

    filters_set.discard(".")
    return cpp_files, h_files, image_files, other_files, sorted(filters_set)

def build_filters_xml(cpp_files, h_files, image_files, other_files, filters):
    ns = "http://schemas.microsoft.com/developer/msbuild/2003"
    ET.register_namespace("", ns)
    root = ET.Element("Project", {"ToolsVersion": "4.0", "xmlns": ns})

    # Filter list
    item_group_filters = ET.SubElement(root, "ItemGroup")
    for f in filters:
        filt = ET.SubElement(item_group_filters, "Filter", {"Include": f.replace("/", "\\")})
        unique_id = "{" + os.urandom(16).hex() + "}"
        ET.SubElement(filt, "UniqueIdentifier").text = unique_id

    # CPP entries
    item_group_cpp = ET.SubElement(root, "ItemGroup")
    for cpp in cpp_files:
        rel = get_relative_path(cpp).replace("/", "\\")
        elem = ET.SubElement(item_group_cpp, "ClCompile", {"Include": rel})
        folder = os.path.dirname(rel).replace("/", "\\")
        if folder:
            ET.SubElement(elem, "Filter").text = folder

    # H entries
    item_group_h = ET.SubElement(root, "ItemGroup")
    for h in h_files:
        rel = get_relative_path(h).replace("/", "\\")
        elem = ET.SubElement(item_group_h, "ClInclude", {"Include": rel})
        folder = os.path.dirname(rel).replace("/", "\\")
        if folder:
            ET.SubElement(elem, "Filter").text = folder

    # Image entries
    item_group_image = ET.SubElement(root, "ItemGroup")
    for image in image_files:
        rel = get_relative_path(image).replace("/", "\\")
        elem = ET.SubElement(item_group_image, "Image", {"Include": rel})
        folder = os.path.dirname(rel).replace("/", "\\")
        if folder:
            ET.SubElement(elem, "Filter").text = folder

    # Other entries
    item_group_none = ET.SubElement(root, "ItemGroup")
    for file in other_files:
        rel = get_relative_path(file).replace("/", "\\")
        elem = ET.SubElement(item_group_none, "None", {"Include": rel})
        folder = os.path.dirname(rel).replace("/", "\\")
        if folder:
            ET.SubElement(elem, "Filter").text = folder

    return root

def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        for child in elem:
            indent(child, level + 1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def add_dependent_upon_to_vcxproj(vcxproj_path, project_root):
    ns = "http://schemas.microsoft.com/developer/msbuild/2003"
    ET.register_namespace("", ns)
    tree = ET.parse(vcxproj_path)
    root = tree.getroot()

    # Build a set of all header files (relative paths)
    headers = set()
    for dirpath, _, filenames in os.walk(project_root):
        for fname in filenames:
            if fname.lower().endswith(".h"):
                headers.add(os.path.relpath(os.path.join(dirpath, fname), project_root).replace("\\", "/"))

    # Search all ClCompile entries
    modified = False
    for clcompile in root.findall(f".//{{{ns}}}ClCompile"):
        cpp_path = clcompile.attrib.get("Include", "").replace("\\", "/")
        basename = os.path.splitext(os.path.basename(cpp_path))[0]
        folder = os.path.dirname(cpp_path)
        header_rel_path = cpp_path.replace(".cpp", ".h")
        
        if header_rel_path in headers:
            # Remove all existing DependentUpon elements
            for dep_elem in clcompile.findall(f"{{{ns}}}DependentUpon"):
                clcompile.remove(dep_elem)

            # Add the correct DependentUpon
            dep_elem = ET.SubElement(clcompile, "DependentUpon")
            dep_elem.text = header_rel_path
            modified = True
            print(f"Set DependentUpon for: {cpp_path} -> {header_rel_path}")
        else:
            print(f"No matching header for: {cpp_path}")

    if modified:
        tree.write(vcxproj_path, encoding="utf-8", xml_declaration=True)
        print(f"Updated: {vcxproj_path}")
    else:
        print("No changes made.")

if __name__ == "__main__":
    cpp_files, h_files, image_files, other_files, filters = collect_files()
    xml_root = build_filters_xml(cpp_files, h_files, image_files, other_files, filters)
    indent(xml_root)

    tree = ET.ElementTree(xml_root)
    tree.write(FILTERS_FILE, encoding="utf-8", xml_declaration=True)
    print(f"Wrote filters file: {FILTERS_FILE}")

    add_dependent_upon_to_vcxproj(PROJECT_FILE, ROOT_DIR)