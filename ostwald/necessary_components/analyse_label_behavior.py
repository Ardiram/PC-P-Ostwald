import os
import csv
import xml.etree.ElementTree as ET
import math
import matplotlib.pyplot as plt
from collections import Counter
import time
from rich import print
import shutil
from xml.dom.minidom import parseString
import functools
from pathlib import Path
from typing import Optional
from tqdm import tqdm

# Define the folder containing the SVG files
input_folder = os.getcwd()
output_csv = "coordinates_with_neighbors.csv"
label_of_interest = 1
layer_of_interest = "adsorbate layer"


def with_parsed_svg(func):
    """
    A decorator that parses an SVG file and passes the root element to the decorated function.
    It supports both xml.etree.ElementTree for well-formed XML files and xml.dom.minidom as a fallback.
    """

    def wrapper(*args, **kwargs):
        file_path = args[0]  # Expecting the first argument to be the SVG file path

        # Parse namespaces from the SVG file
        def parse_namespaces(source):
            return dict(node for _, node in ET.iterparse(source, events=["start-ns"]))

        # Custom Element class to handle namespaces
        class ElementNS(ET.Element):
            namespaces = None

            def find(self, path):
                return super().find(path, self.namespaces)

            def findall(self, path):
                return super().findall(path, self.namespaces)

        # Parse namespaces
        namespaces = parse_namespaces(file_path)

        # Create a factory to produce namespace-aware elements
        def create_element_factory(namespaces):
            def element_factory(tag, attrib):
                el = ElementNS(tag, attrib)
                el.namespaces = namespaces
                return el

            return element_factory

        # Set up the XMLParser with the custom factory
        element_factory = create_element_factory(namespaces)
        parser = ET.XMLParser(target=ET.TreeBuilder(element_factory=element_factory))

        # Parse the file into a tree
        tree = ET.parse(file_path, parser=parser)
        root = tree.getroot()
        root.namespaces = namespaces  # Attach the namespaces for convenience
        # Call the wrapped function with the parsed root
        modified_args = (root,) + args[1:]
        return func(*modified_args, **kwargs)

    return wrapper


def with_parsed_svg_with_minidom(func):
    """
    A decorator that parses an SVG file with minidom and passes the DOM and root element
    to the decorated function.
    """

    def wrapper(svg_file_path, *args, **kwargs):
        try:
            # Open and parse the SVG file using minidom
            with open(svg_file_path, "r", encoding="utf-8") as f:
                svg_content = f.read()
            dom = parseString(svg_content)
            svg_root = dom.documentElement

            # Pass the parsed DOM and root element to the decorated function
            return func(dom, svg_root, svg_file_path, *args, **kwargs)

        except Exception as e:
            raise ValueError(f"Failed to parse {svg_file_path} with minidom: {e}")

    return wrapper


def log_execution(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        print(f"Executed {func.__name__} in {end_time - start_time:.2f}s")
        return result

    return wrapper


def extract_circle_radius(root):
    """
    Extract the radius of the first <circle> element found in the SVG,
    excluding circles inside groups with an id ending in ' circle for legend'.

    Args:
        root (ElementNS): The root element of the parsed SVG file.

    Returns:
        float: The radius of the first valid <circle> element.

    Raises:
        ValueError: If no valid <circle> element or radius can be found in the SVG file.
    """
    # Iterate through all <circle> elements in the SVG
    for circle in root.findall(".//circle"):
        # Check if the circle has a parent <g> element
        parent = circle.getparent() if hasattr(circle, "getparent") else None

        if parent is None:
            # Find parent manually in ElementTree (for ElementTree, parent relationships aren't tracked directly)
            for group in root.findall(".//g"):
                if circle in list(group):
                    parent = group
                    break

        # Check the parent's id, if it exists
        if parent is not None:
            parent_id = parent.attrib.get("id", "")
            if parent_id.endswith(" circle for legend"):
                continue  # Skip this circle

        # If the circle has an 'r' attribute, return its radius
        if "r" in circle.attrib:
            return float(circle.attrib["r"])

    # If no valid circle is found, raise an error
    raise ValueError("Could not find any valid circle radius in the SVG file.")


def extract_layer_circles(root, layer_id):
    """
    Extracts circle coordinates and labels from a specified layer.

    Args:
        root (ElementNS): The root element of the parsed SVG file.
        layer_id (str): The ID of the layer to extract circles from.

    Returns:
        list: A list of dictionaries with circle labels and coordinates.
              Example: [{"label": "1", "x": 10.0, "y": 15.0}, ...]
    """
    layer = root.find(f".//g[@id='{layer_id}']")
    if layer is None:
        raise ValueError(f"No layer with ID '{layer_id}' found in the SVG file.")

    # Find all 'use' and 'text' elements within the layer
    uses = layer.findall(".//use")
    texts = layer.findall(".//text")

    # Combine labels and coordinates into a single list
    circles = []
    for text, use in zip(texts, uses):
        label = text.text.strip()
        x = float(use.attrib.get("x", "0"))
        y = float(use.attrib.get("y", "0"))
        circles.append({"label": label, "x": x, "y": y})

    return circles


def get_layer_transform(root, layer_id):
    """
    Extracts the 'transform' attribute of a specific layer in the SVG file.

    Args:
        root: The root element of the parsed SVG.
        layer_id: The ID of the layer to find.

    Returns:
        A tuple of two floats (x_translate, y_translate) from the transform attribute,
        or None if not found or unable to parse.
    """
    # Find all <use> elements in the SVG
    use_elements = root.findall(".//use")

    # Search for the element with the specified layer_id in xlink:href
    for use_element in use_elements:
        href = use_element.get("{http://www.w3.org/1999/xlink}href")
        if href == f"#{layer_id}":
            transform = use_element.get("transform")
            if transform:
                # Remove "translate(" and ")" and split the values
                try:
                    x, y = map(
                        float,
                        transform.replace("translate(", "").replace(")", "").split(","),
                    )
                    return (x, y)
                except (ValueError, AttributeError):
                    return None
    return None


def create_previous_positions_group(dom, group_id):
    """
    Creates an SVG <g> element for previous positions with predefined attributes.

    Args:
        dom: The DOM object of the SVG file.
        group_id (str): The ID to assign to the <g> element.

    Returns:
        The created <g> element.
    """
    # Create a group element
    group_element = dom.createElement("g")
    group_element.setAttribute("id", group_id)
    group_element.setAttribute(
        "style",
        "stroke:rgb(0,0,255); stroke-width:0.1; fill:rgb(0,0,255); "
        "stroke-opacity:0.5; fill-opacity:0.2;",
    )

    return group_element


def add_circle_elements_to_group(
    dom, group_element, unique_positions, translation_x, translation_y, radius
):
    """
    Adds <circle> elements for unique positions to a given group in the SVG DOM.

    Args:
        dom: The DOM object of the SVG file.
        group_element: The <g> element to which the circles will be added.
        unique_positions (set): A set of (x, y) tuples representing positions.
        translation_x (float): Translation offset for the x-coordinate.
        translation_y (float): Translation offset for the y-coordinate.
        radius (float): The radius of the circles to be added.

    Returns:
        None
    """
    for x, y in unique_positions:
        # Create a <circle> element
        circle = dom.createElement("circle")

        # Apply translation to x and y coordinates
        circle.setAttribute("cx", str(x + translation_x))
        circle.setAttribute("cy", str(y + translation_y))
        circle.setAttribute("r", str(radius))

        # Append the circle to the group element
        group_element.appendChild(circle)


def find_label_and_neighbors(circles, radius, target_label):
    """
    Finds the circle with the specified label and its neighbors within a given distance.

    Args:
        circles (list): A list of dictionaries with circle labels and coordinates.
        radius (float): The radius to consider for neighbors.
        target_label (str): The label of the circle to process.

    Returns:
        tuple: A dictionary representing the target circle, and a list of neighboring labels.
    """
    # Find the circle with the specified label
    target_circle = next(
        (circle for circle in circles if circle["label"] == target_label), None
    )
    if not target_circle:
        raise ValueError(f"Circle with label '{target_label}' not found.")

    x1, y1 = target_circle["x"], target_circle["y"]

    # Find neighboring circles within the specified distance
    neighbors = []
    for circle in circles:
        if circle["label"] != target_label:  # Skip the target circle itself
            x2, y2 = circle["x"], circle["y"]
            distance = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
            if distance <= 2 * radius:
                neighbors.append(circle["label"])

    return target_circle, neighbors


@with_parsed_svg
def process_single_svg(root, filename, layer_id, target_label):
    """
    Processes a single SVG file to extract circle coordinates, labels, and neighbors for a specified label.

    Args:
        root (ElementNS): The root element of the parsed SVG file.
        filename (str): The name of the SVG file being processed.
        layer_id (str): The ID of the layer to extract circles from.
        target_label (str): The label of the circle to process.

    Returns:
        list: A single row of data in the format [filename, x1, y1, neighbors_str].
    """
    # Extract the radius of one diffusion point
    circle_radius = extract_circle_radius(root)

    # Extract circles from the specified layer
    circles = extract_layer_circles(root, layer_id)

    # Find the target circle and its neighbors
    target_circle, neighbors = find_label_and_neighbors(
        circles, circle_radius, target_label
    )

    # Format neighbors into a string
    neighbors_str = " | ".join(neighbors) if neighbors else ""

    # Return processed data
    return [filename, target_circle["x"], target_circle["y"], neighbors_str]


@log_execution
def process_svg_files(input_folder, output_csv, layer_id, target_label):
    """
    Processes all SVG files in the input folder and writes the results to a CSV file.

    Args:
        input_folder (str): Path to the folder containing SVG files.
        output_csv (str): Path to the output CSV file.
        layer_id (str): The ID of the layer to extract circles from.
        target_label (str): The label of the circle to process.

    Returns:
        list: Processed data written to the CSV.
    """
    data = []

    # Get all relevant SVG files
    svg_files = sorted(
        filename for filename in os.listdir(input_folder) 
        if filename.endswith(".svg") and filename.startswith("m")
    )

    # Initialize tqdm progress bar
    with tqdm(total=len(svg_files), desc="Processing SVG files", unit="file") as pbar:
        for filename in svg_files:
            file_path = os.path.join(input_folder, filename)
            try:
                # Process each SVG file
                row = process_single_svg(
                    file_path, filename, layer_id=layer_id, target_label=target_label
                )
                data.append(row)
            except Exception as e:
                print(f"Error processing {filename}: {e}")
            finally:
                # Update the progress bar after processing each file
                pbar.update(1)

    # Write data to CSV
    with open(output_csv, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["Filename", "X", "Y", "Neighbors"])
        writer.writerows(data)

    return data  # Return the data list for further use.


@log_execution
def plot_label_frequencies(data, output_svg="label_frequencies.svg"):
    """
    Plots label frequencies with optimized spacing.
    """
    all_labels = []
    for entry in data:
        neighbors = entry[3]
        if neighbors:
            all_labels.extend(neighbors.split(" | "))

    label_counts = Counter(all_labels)
    sorted_labels = sorted(label_counts.keys(), key=lambda x: int(x))
    sorted_frequencies = [label_counts[label] for label in sorted_labels]
    
    num_labels = len(sorted_labels)
    width = max(16, min(num_labels * 0.4, 40))
    height = 10
    plt.figure(figsize=(width, height))
    
    font_size = max(6, min(12, 300 / num_labels))
    
    plt.bar(range(num_labels), sorted_frequencies, color="blue")
    plt.xticks(range(num_labels), sorted_labels, rotation=45, fontsize=font_size)
    plt.grid(axis='y', linestyle='--', alpha=0.3)
    
    plt.xlabel("Nachbar-Label", fontsize=12, fontweight="bold")
    plt.ylabel("Häufigkeit über alle Dateien", fontsize=12, fontweight="bold")
    plt.title("Häufigkeit der Nachbar Label", fontsize=14, fontweight="bold", fontstyle="italic")
    
    # Remove excess spacing
    plt.margins(x=0.001)  # Minimal margin
    plt.tight_layout(pad=.001)
    
    plt.savefig(output_svg, format="svg", bbox_inches='tight')
    plt.close()


@with_parsed_svg
def check_overlap(root, data, layer_id, current_file):
    """
    Checks if the x and y coordinates of previous label 1 circles in 'data'
    overlap with the x and y coordinates of adsorbate circles in the current file.

    Args:
        root (ElementNS): The root element of the parsed SVG file.
        data (list): List of data entries, each containing ['Filename', X, Y, Neighbors].
        layer_id (str): The ID of the layer containing adsorbate circles to check.
        current_file (str): The current file being checked.

    Returns:
        dict: A dictionary storing overlap results for the current file.
    """
    # Convert the data to a dictionary for easier access by filename
    data_dict = {entry[0]: (float(entry[1]), float(entry[2])) for entry in data}

    # If the current file is the first in the sequence, skip overlap checks
    if current_file not in data_dict or all(file >= current_file for file in data_dict):
        return {"File": current_file, "Overlap": False, "Details": []}

    # Extract adsorbate layer circles
    adsorbate_circles = extract_layer_circles(root, layer_id)

    # Prepare the overlap results
    result = {"File": current_file, "Overlap": False, "Details": []}

    # Check overlaps with previous files' label 1 coordinates
    for prev_filename, (prev_x, prev_y) in data_dict.items():
        if prev_filename < current_file:  # Only compare with previous files
            for circle in adsorbate_circles:
                x, y, label = circle["x"], circle["y"], circle["label"]
                if abs(prev_x - x) < 1e-6 and abs(prev_y - y) < 1e-6:
                    result["Overlap"] = True
                    result["Details"].append(
                        {
                            "Previous File": prev_filename,
                            "Current Label": label,
                            "Coordinates": (x, y),
                        }
                    )

    return result


@log_execution
def process_overlaps(data, input_folder, layer_id):
    """
    Process all SVG files in the input folder and check for overlaps.

    Args:
        data (list): List of data entries from previous processing.
        input_folder (str): Path to the folder containing SVG files.
        layer_id (str): The layer ID to check overlaps in.

    Returns:
        list: Overlap results for each file.
    """
    overlap_results = []

    # Get all relevant SVG files
    svg_files = sorted(
        filename for filename in os.listdir(input_folder) 
        if filename.endswith(".svg") and filename.startswith("m")
    )

    # Initialize tqdm progress bar
    with tqdm(total=len(svg_files), desc="Checking overlaps", unit="file") as pbar:
        for filename in svg_files:
            file_path = os.path.join(input_folder, filename)
            try:
                # Call check_overlap with the current file and data
                overlap = check_overlap(file_path, data, layer_id, current_file=filename)
                overlap_results.append(overlap)
            except Exception as e:
                print(f"Error checking overlaps in {filename}: {e}")
            finally:
                # Update the progress bar after processing each file
                pbar.update(1)

    return overlap_results


@log_execution
def copy_svgs_to_temp(input_folder):
    """
    Creates a temporary folder and copies all SVG files from the input folder into it.

    Args:
        input_folder (str): Path to the folder containing SVG files.

    Returns:
        str: Path to the created temporary folder.
    """
    # Create a temporary directory in the current working directory
    temp_dir = os.path.join(os.getcwd(), "temp_svgs")
    os.makedirs(temp_dir, exist_ok=True)  # Ensure the directory exists
    # print(f"Temporary folder created at: {temp_dir}")

    # Copy all SVG files to the temporary folder
    for filename in os.listdir(input_folder):
        if filename.endswith(".svg") and filename.startswith("m"):
            src_path = os.path.join(input_folder, filename)
            dest_path = os.path.join(temp_dir, filename)
            shutil.copy(src_path, dest_path)
            # print(f"Copied: {filename} to {temp_dir}")

    return temp_dir


@with_parsed_svg
def extract_svg_metadata(root, target_layer_id):
    """
    Extracts metadata from an SVG file using ElementTree.

    Args:
        root (Element): The root of the parsed SVG file.
        target_layer_id (str): The ID of the target layer to extract the transform attribute.

    Returns:
        tuple: A tuple containing the translation (translation_x, translation_y)
               and the circle radius.

    Raises:
        ValueError: If required metadata cannot be determined.
    """
    # Extract the translation transform for the target layer
    translation_x, translation_y = get_layer_transform(root, target_layer_id)

    # Extract the circle radius
    circle_radius = extract_circle_radius(root)

    return translation_x, translation_y, circle_radius


@with_parsed_svg_with_minidom
def add_previous_positions_group_with_minidom(
    dom,
    svg_root,
    svg_file_path,
    unique_positions,
    translation_x,
    translation_y,
    circle_radius,
):
    """
    Adds a group of circles representing unique positions to the SVG file
    using the minidom module, positioning the group before the penultimate <defs> tag.

    Args:
        dom (xml.dom.minidom.Document): The parsed SVG document.
        svg_root (xml.dom.minidom.Element): The root element of the SVG file.
        svg_file_path (str): The path to the SVG file.
        unique_positions (set): A set of (x, y) tuples representing unique positions.
        translation_x (float): The x-translation for positioning.
        translation_y (float): The y-translation for positioning.
        circle_radius (float): Radius of the circle elements.

    Raises:
        Exception: If the file cannot be processed.
    """
    # Check for valid unique_positions
    if not isinstance(unique_positions, (set, list)):
        raise ValueError(
            f"unique_positions must be a set or list of (x, y) tuples. Got: {unique_positions}"
        )

    # Create a group for previous positions
    previous_positions_group = create_previous_positions_group(
        dom, group_id="previous label " + str(label_of_interest) + " positions"
    )

    # Add circle elements to the group
    add_circle_elements_to_group(
        dom,
        previous_positions_group,
        unique_positions,
        translation_x,
        translation_y,
        radius=circle_radius,
    )

    # Find all <defs> tags
    defs_elements = dom.getElementsByTagName("defs")

    # Insert the group before the second-to-last <defs> tag
    if len(defs_elements) >= 3:
        target_defs = defs_elements[-3]
        svg_root.insertBefore(previous_positions_group, target_defs)
    else:
        # Fallback: append the group at the end
        svg_root.appendChild(previous_positions_group)

    # Serialize and save the modified SVG
    with open(svg_file_path, "w", encoding="utf-8") as f:
        f.write(dom.toprettyxml())


def process_svg_file(svg_file_path, unique_positions, target_layer_id):
    """
    Processes a single SVG file, extracting metadata and adding a group of circles.

    Args:
        svg_file_path (str): Path to the SVG file.
        unique_positions (set): Set of (x, y) tuples representing unique positions.
        target_layer_id (str): ID of the target layer to extract transform info.
    """
    # Step 1: Extract metadata using ElementTree
    translation_x, translation_y, circle_radius = extract_svg_metadata(
        svg_file_path, target_layer_id
    )

    # Step 2: Modify SVG using minidom
    add_previous_positions_group_with_minidom(
        svg_file_path, unique_positions, translation_x, translation_y, circle_radius
    )


@log_execution
def add_previous_positions_to_all_svgs(data, temp_folder, target_layer_id):
    """
    Processes all SVG files in the temp_folder and adds previous positions
    to each file by calling add_previous_positions_to_svg.

    Args:
        data (list): A list of data entries where each entry is a list in the form:
                     [filename, x, y, 'optional previous label positions']
        temp_folder (str): Path to the folder containing the SVG files.
        target_layer_id (str): The ID of the target layer to extract the transform attribute.

    Raises:
        FileNotFoundError: If the specified temp_folder does not exist.
    """
    if not os.path.exists(temp_folder):
        raise FileNotFoundError(f"The folder '{temp_folder}' does not exist.")

    # Convert data into a dictionary for easier lookup
    data_dict = {entry[0]: (float(entry[1]), float(entry[2])) for entry in data}

    # Sort SVG files in the temp_folder
    sorted_files = sorted(
        f for f in os.listdir(temp_folder) if f.endswith(".svg") and f.startswith("m")
    )

    # Initialize tqdm progress bar
    with tqdm(total=len(sorted_files), desc="Processing SVG files", unit="file") as pbar:
        for current_file in sorted_files:
            # print(f"Processing {current_file}...")

            current_path = os.path.join(temp_folder, current_file)

            # Collect unique positions from previous files
            unique_positions = set()
            for prev_file in sorted_files:
                if prev_file < current_file and prev_file in data_dict:
                    x, y = data_dict[prev_file]
                    unique_positions.add((x, y))

            if not unique_positions:
                print(f"No previous positions found for {current_file}. Skipping...")
                pbar.update(1)  # Still update the progress bar
                continue

            try:
                # Call the single SVG function
                process_svg_file(current_path, unique_positions, target_layer_id)
                # print(f"Successfully updated {current_file}.")
            except Exception as e:
                print(f"Error processing {current_file}: {e}")
            finally:
                # Update the progress bar after processing each file
                pbar.update(1)


@with_parsed_svg
def insert_circle_in_last_defs(
    root: ET.Element, file_path: str, output_path: Optional[str] = None
) -> None:
    """
    Inserts a blue circle element at the beginning of the last defs tag in an SVG file.

    Args:
        root: The root element of the SVG.
        file_path: The file path of the SVG being processed.
        output_path: Optional path to save the modified SVG. If None, overwrites the input file.
    """
    # Get the SVG namespace dynamically from the root
    namespaces = root.namespaces
    svg_ns = (
        list(namespaces.items())[0][1],
        list(namespaces.items())[1][1],
    )  # Default to SVG namespace if not found

    # Use the namespace in tag lookups and element creation
    defs_elements = root.findall(f".//{{{svg_ns[0]}}}defs")

    if not defs_elements:
        # Create a new <defs> element if none exists
        defs = ET.Element(f"{{{svg_ns[0]}}}defs")
        root.insert(0, defs)
        defs_elements = [defs]

    # Get the last <defs> element
    last_defs = defs_elements[-1]

    # Create the new <g> element with the circle
    g_element = ET.Element(
        f"{{{svg_ns[0]}}}g",
        {
            "id": "previous label " + str(label_of_interest),
            "style": "stroke:rgb(0,0,255); stroke-width:0.1; fill:rgb(0,0,255);stroke-opacity:1.0;fill-opacity:0.5;"
            "",
        },
    )

    # Create and add the circle element
    circle = ET.Element(f"{{{svg_ns[0]}}}circle", {"cx": "0", "cy": "0", "r": "2"})
    g_element.append(circle)

    # Insert the <g> element at the beginning of the last <defs>
    last_defs.insert(0, g_element)

    # Register the SVG namespace to avoid ns0 prefixes
    ET.register_namespace("", svg_ns[0])
    ET.register_namespace(list(namespaces.items())[1][0], svg_ns[1])

    # Save the modified SVG
    tree = ET.ElementTree(root)
    if output_path:
        tree.write(output_path)
    else:
        tree.write(file_path)


@with_parsed_svg
def insert_use_after_defs(
    root: ET.Element, file_path: str, x: str = "83", y: str = "167.892"
) -> None:
    """
    Inserts a use element referencing 'previous label 1' after the last defs tag.

    Args:
        root: The root element of the SVG.
        file_path: The file path of the SVG being processed.
        x: X-coordinate for the use element.
        y: Y-coordinate for the use element.
    """
    # Find all <defs> elements
    defs_elements = root.findall(".//defs")

    if not defs_elements:
        raise ValueError("No defs elements found in the SVG file")

    # Get the last <defs> element
    last_defs = defs_elements[-1]

    # Create the <use> element with xlink:href
    use_element = ET.Element(
        "use",
        {"{http://www.w3.org/1999/xlink}href": "#previous label " + str(label_of_interest), "x": x, "y": y},
    )

    # Insert the <use> element right after the last <defs>
    parent = root
    children = list(parent)
    last_defs_index = children.index(last_defs)
    parent.insert(last_defs_index + 1, use_element)

    # Save the modified SVG
    tree = ET.ElementTree(root)
    tree.write(file_path)


@with_parsed_svg
def insert_text_labels(root: ET.Element, file_path: str) -> None:
    """
    Inserts text labels in the last styled group of the SVG file.

    Args:
        root: The root element of the SVG.
        file_path: The file path of the SVG being processed.
    """
    # Find all groups with the specific style
    styled_groups = root.findall(
        './/g[@style="font-size: 6pt; font-weight:bold; font-family:sans-serif;"]'
    )

    if not styled_groups:
        # Create a new styled group if none exists
        styled_group = ET.Element(
            "g", {"style": "font-size: 6pt; font-weight:bold; font-family:sans-serif;"}
        )
        root.append(styled_group)
        styled_groups = [styled_group]

    # Get the last styled group
    last_group = styled_groups[-1]

    # Create and add the first text element
    text1 = ET.Element("text", {"x": "6.75", "y": "162.392"})
    text1.text = "Previous"

    # Create and add the second text element
    text2 = ET.Element("text", {"x": "6.75", "y": "169.892"})
    text2.text = "Label " + str(label_of_interest) + " Positions"

    # Add text elements to the group
    last_group.insert(0, text2)
    last_group.insert(0, text1)

    # Save the modified SVG
    tree = ET.ElementTree(root)
    tree.write(file_path)


@log_execution
def process_svg_files_in_directory(directory_path: str) -> None:
    """
    Processes all SVG files in the given directory that start with 'm' and end with '.svg'.

    Args:
        directory_path: Path to the directory containing SVG files.
    """
    dir_path = Path(directory_path)
    svg_files = [f for f in dir_path.glob("m*.svg")]

    if not svg_files:
        print(f"No SVG files starting with 'm' found in {directory_path}")
        return

    # Initialize tqdm progress bar
    with tqdm(total=len(svg_files), desc="Processing SVG files", unit="file") as pbar:
        for svg_file in svg_files:
            try:
                # print(f"Processing {svg_file.name}...")
                file_path = str(svg_file)

                # Apply modifications
                insert_circle_in_last_defs(file_path, file_path)
                insert_use_after_defs(file_path, file_path)
                insert_text_labels(file_path, file_path)

                # print(f"Successfully processed {svg_file.name}")
            except Exception as e:
                print(f"Error processing {svg_file.name}: {e}")
            finally:
                # Update the progress bar after each file is processed
                pbar.update(1)

    print(f"Finished processing all SVG files in {directory_path}")


if __name__ == "__main__":
    data = process_svg_files(
        input_folder, output_csv, layer_of_interest, str(label_of_interest)
    )
    plot_label_frequencies(data)
    label_overlaps = process_overlaps(data, input_folder, layer_of_interest)
    created_directory = copy_svgs_to_temp(input_folder)
    add_previous_positions_to_all_svgs(data, created_directory, layer_of_interest)
    process_svg_files_in_directory(created_directory)
