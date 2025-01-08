import os
import math
import csv
import xml.etree.ElementTree as ET

# Constants
diameter_silver_fcc_nm = 0.408  # Diameter of silver in fcc lattice in nanometers


def parse_svg_dimensions(svg_file):
    """Extract the boundary dimensions from the SVG's viewBox attribute."""
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Parse the viewBox attribute
    viewBox = root.attrib.get("viewBox")
    if not viewBox:
        raise ValueError("SVG file does not have a viewBox attribute.")

    # Split viewBox into components: min-x, min-y, width, height
    _, _, boundary_width, boundary_height = map(float, viewBox.split())
    return boundary_width, boundary_height


def parse_start_position(svg_file):
    """Parse the SVG to find the start position (center of the enclosing circle)."""
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Define SVG namespace
    namespace = {
        "svg": "http://www.w3.org/2000/svg",
        "xlink": "http://www.w3.org/1999/xlink",
    }
    ET.register_namespace("", namespace["svg"])  # Preserve SVG namespace

    # Find the start position element
    use_element = root.find(
        ".//svg:use[@xlink:href='#write the start position']", namespace
    )
    if use_element is None:
        raise ValueError("Could not find the start position element in the SVG file.")

    # Get the translation values from the transform attribute
    transform = use_element.attrib.get("transform", "")
    if not transform.startswith("translate("):
        raise ValueError(
            "Start position element does not have a valid transform attribute."
        )

    translate_values = transform[len("translate(") : -1].split(",")
    translation_x = float(translate_values[0])
    translation_y = float(translate_values[1])

    # Find the circle within the start position group
    start_circle = root.find(
        ".//svg:g[@id='write the start position']/svg:circle", namespace
    )
    if start_circle is None:
        raise ValueError("Could not find the start position circle.")

    cx = float(start_circle.attrib["cx"]) + translation_x
    cy = float(start_circle.attrib["cy"]) + translation_y

    return cx, cy


def extract_diffusion_points(svg_file):
    """Parse the SVG to extract all diffusion points, including the radius of one point."""
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Define SVG namespace
    namespace = {
        "svg": "http://www.w3.org/2000/svg",
        "xlink": "http://www.w3.org/1999/xlink",
    }
    ET.register_namespace("", namespace["svg"])  # Preserve SVG namespace

    # Find the <use> element that references the diffusion group
    use_element = root.find(".//svg:use[@xlink:href='#follow the atom']", namespace)
    if use_element is None:
        raise ValueError("Could not find the diffusion group element in the SVG file.")

    # Extract the translation values from the transform attribute
    transform = use_element.attrib.get("transform", "")
    translation_x, translation_y = 0, 0
    if transform.startswith("translate("):
        translate_values = transform[len("translate(") : -1].split(",")
        translation_x = float(translate_values[0])
        if len(translate_values) > 1:
            translation_y = float(translate_values[1])

    # Find all circles in the diffusion group
    diffusion_points = []
    diffusion_circles = root.findall(
        ".//svg:g[@id='follow the atom']/svg:circle", namespace
    )

    # Variable to store the radius of one diffusion point
    diffusion_point_radius = None

    for i, circle in enumerate(diffusion_circles):
        cx = float(circle.attrib["cx"]) + translation_x
        cy = float(circle.attrib["cy"]) + translation_y
        diffusion_points.append((cx, cy))

        # Extract the radius of the first diffusion point
        if diffusion_point_radius is None and "r" in circle.attrib:
            diffusion_point_radius = float(circle.attrib["r"])

    if diffusion_point_radius is None:
        raise ValueError("Could not find any diffusion point radius in the SVG file.")

    return diffusion_points, diffusion_point_radius


def detect_boundary_crossings(svg_file):
    """
    Detect horizontal and vertical boundary crossings:
    - Horizontal: Based on abrupt shifts in cx values.
    - Vertical: Based on periodic boundary conditions inferred from substrate layer.
    """
    namespace = {"svg": "http://www.w3.org/2000/svg"}
    try:
        # Parse the SVG file
        tree = ET.parse(svg_file)
        root = tree.getroot()

        # Part 1: Horizontal boundary crossing detection (abrupt shifts)
        circles_diffusion_path = root.findall(
            ".//svg:g[@id='follow the atom']/svg:circle", namespace
        )

        if not circles_diffusion_path:
            print(f"No diffusion path circles found in {svg_file}.")
            return False

        # Extract cx, cy values in order
        positions = []
        for circle in circles_diffusion_path:
            cx = float(circle.attrib.get("cx", 0))
            cy = float(circle.attrib.get("cy", 0))
            positions.append((cx, cy))

        # Constants for thresholds
        y_shift_threshold = 0.9427  # Expected y-shift for moving one row
        x_shift_threshold = 161.6  # Threshold for significant horizontal shift

        # Horizontal crossing detection
        horizontal_crossing_detected = False
        crossing_points = None  # To store the first detected crossing points

        # Iterate over all positions
        for i in range(len(positions)):
            curr_cx, curr_cy = positions[i]

            # Compare current point with all other points (potential neighbors)
            for j in range(len(positions)):
                if i == j:
                    continue  # Skip self-comparison

                neighbor_cx, neighbor_cy = positions[j]
                y_shift = abs(curr_cy - neighbor_cy)

                # Check if neighbor is in the same row or adjacent row
                if y_shift <= y_shift_threshold:
                    horizontal_shift = abs(curr_cx - neighbor_cx)
                    if horizontal_shift > x_shift_threshold:
                        horizontal_crossing_detected = True
                        crossing_points = (curr_cx, curr_cy, neighbor_cx, neighbor_cy)
                        break

        # Print detected crossing only once per file
        if horizontal_crossing_detected:
            print(
                f"Horizontal boundary crossing detected between points ({crossing_points[0]}, {crossing_points[1]}) and ({crossing_points[2]}, {crossing_points[3]})."
            )

        # Part 2: Vertical boundary crossing detection (periodic logic)
        circles_substrate_layer = root.findall(
            './/svg:g[@id="substrate layer C with circles everywhere"]//svg:use',
            namespace,
        ) or root.findall(
            './/svg:g[@id="substrate layer C with outside circles"]//svg:use',
            namespace,
        )

        if not circles_substrate_layer:
            print(
                f"No substrate circles found in {svg_file}. Assuming no vertical boundary crossing."
            )
            return horizontal_crossing_detected

        # Extract substrate circle positions
        substrate_positions = []
        for circle in circles_substrate_layer:
            x = float(circle.attrib.get("x", 0))
            y = float(circle.attrib.get("y", 0))
            substrate_positions.append((x, y))

        # Deduce vertical boundaries (assuming a grid)
        y_coords = sorted(set(pos[1] for pos in substrate_positions))
        y_step = y_coords[1] - y_coords[0] if len(y_coords) > 1 else None
        min_y, max_y = min(y_coords), max(y_coords)

        if not y_step:
            print(
                f"Unable to infer vertical boundaries from {svg_file}. Assuming no vertical boundary crossing."
            )
            return horizontal_crossing_detected

        # Check for vertical boundary crossing
        vertical_crossing_detected = False
        for _, cy in positions:
            if cy < min_y or cy > max_y:
                print(f"Vertical boundary crossing detected at cy={cy}.")
                vertical_crossing_detected = True
                break

        return horizontal_crossing_detected or vertical_crossing_detected

    except ET.ParseError as e:
        print(f"Error parsing SVG file {svg_file}: {e}")
        return False
    except Exception as e:
        print(f"Error detecting boundary crossings in {svg_file}: {e}")
        return False


def calculate_max_distance_and_direction(
    start_pos, diffusion_points, diffusion_point_radius
):
    """Calculate the radius of the enclosing circle and the farthest diffusion point."""
    cx_start, cy_start = start_pos
    max_distance = 0
    edge_point = None

    # Calculate the distance to each diffusion point
    for cx, cy in diffusion_points:
        distance = math.sqrt((cx - cx_start) ** 2 + (cy - cy_start) ** 2)
        if distance > max_distance:
            max_distance = distance
            edge_point = (cx, cy)

    # Adjust the radius to include the diffusion circle's radius
    adjusted_radius = max_distance + diffusion_point_radius
    return adjusted_radius, edge_point


def add_enclosing_circle_and_line(svg_file, center, radius, edge_point, output_file):
    """Add an enclosing circle and a line to the SVG with unique IDs for easier extraction."""
    cx_start, cy_start = center
    cx_edge, cy_edge = edge_point

    # Calculate the exact endpoint of the line on the enclosing circle's edge
    distance = math.sqrt((cx_edge - cx_start) ** 2 + (cy_edge - cy_start) ** 2)
    if distance == 0:
        raise ValueError(
            "Edge point coincides with start position; no valid diffusion path."
        )

    # Calculate the endpoint of the line on the circle's perimeter
    line_end_x = cx_start + radius * (cx_edge - cx_start) / distance
    line_end_y = cy_start + radius * (cy_edge - cy_start) / distance

    # Parse the SVG file
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Define SVG namespace
    namespace = {"svg": "http://www.w3.org/2000/svg"}
    ET.register_namespace("", namespace["svg"])  # Preserve SVG namespace

    # Add a new <circle> element with blue stroke and stroke-width 0.3 and unique ID
    circle = ET.Element(
        "circle",
        {
            "cx": str(cx_start),
            "cy": str(cy_start),
            "r": str(radius),
            "stroke": "blue",
            "fill": "none",
            "stroke-width": "0.3",
            "id": "enclosing_circle",  # Unique ID for easier extraction
        },
    )
    root.append(circle)

    # Add a line from the center to the calculated point on the circle's edge with unique ID
    line = ET.Element(
        "line",
        {
            "x1": str(cx_start),
            "y1": str(cy_start),
            "x2": str(line_end_x),
            "y2": str(line_end_y),
            "stroke": "blue",
            "stroke-width": "0.3",
            "id": "enclosing_line",  # Unique ID for easier extraction
        },
    )
    root.append(line)

    # Save the modified SVG
    tree.write(output_file)


def process_all_svgs(root_folder, output_csv):
    """Process m_21.svg files, add enclosing circles and lines, and save radii in nanometers to a CSV file."""

    # Get all "numberK" folders and sort them
    number_folders = sorted(
        [
            f
            for f in os.listdir(root_folder)
            if os.path.isdir(os.path.join(root_folder, f))
        ]
    )

    # Prepare the CSV structure
    rows = [[""] + number_folders]  # First row: header with "numberK" folder names

    # Iterate through run folders
    for run_index in range(1, 6):  # From run_1 to run_5
        row = [f"run_{run_index}"]
        for folder in number_folders:
            run_folder = os.path.join(root_folder, folder, f"run_{run_index}")
            svg_file = os.path.join(run_folder, "m_21.svg")
            output_file = os.path.join(run_folder, "m_21_circle.svg")
            if os.path.exists(svg_file):
                try:
                    # Step 1: Parse boundary dimensions (for legacy use)
                    boundary_width, boundary_height = parse_svg_dimensions(svg_file)

                    # Step 2: Find start position
                    start_pos = parse_start_position(svg_file)

                    # Step 3: Extract diffusion points and their radius
                    diffusion_points, diffusion_point_radius = extract_diffusion_points(
                        svg_file
                    )

                    # Calculate conversion factor
                    conversion_factor = diameter_silver_fcc_nm / (
                        2 * diffusion_point_radius
                    )

                    # Step 4: Detect periodic boundary crossing using the parallelogram method
                    if detect_boundary_crossings(svg_file):
                        print(
                            f"Skipping {svg_file}: Periodic boundary crossing detected."
                        )
                        row.append(None)
                        continue

                    # Step 5: Calculate maximum distance and edge point
                    adjusted_radius, edge_point = calculate_max_distance_and_direction(
                        start_pos, diffusion_points, diffusion_point_radius
                    )

                    # Convert radius to nanometers
                    adjusted_radius_nm = adjusted_radius * conversion_factor

                    # Step 6: Add enclosing circle and line to SVG
                    add_enclosing_circle_and_line(
                        svg_file, start_pos, adjusted_radius, edge_point, output_file
                    )

                    # Append the converted radius to the CSV row
                    row.append(f"{adjusted_radius_nm:.6f}")

                except Exception as e:
                    print(f"Error processing {svg_file}: {e}")
                    row.append(None)
            else:
                print(f"File not found: {svg_file}")
                row.append(None)
        rows.append(row)

    # Write all rows to a CSV file
    with open(output_csv, mode="w", newline="") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerows(rows)
    print(f"CSV file created: {output_csv}")


# Main script
if __name__ == "__main__":
    root_folder = os.getcwd()  # Replace with the path to your main folder
    output_csv = "enclosing_circle_radii_nm.csv"  # Output CSV file in the current working directory
    process_all_svgs(root_folder, output_csv)
