use gdk_pixbuf::{PixbufLoader};
use gdk_pixbuf::prelude::*; // Import the necessary traits
use std::{fs, path::Path};
use quick_xml::reader::Reader;
use quick_xml::writer::Writer;
use quick_xml::events::{Event, BytesStart};
use std::process::Command;
use std::fs::File;
use std::io::Write;
use std::ffi::OsStr;
use glob::glob;
use std::path::PathBuf;

// Load the svg files in sorted order for further processing
pub fn load_sorted_svgs(input_dir: &str) -> Result<Vec<std::fs::DirEntry>, Box<dyn std::error::Error>> {
    if !Path::new(input_dir).exists() {
        return Err("Input directory does not exist".into());
    }
    
    let mut svg_files = fs::read_dir(input_dir)?
        .filter_map(Result::ok)
        .filter(|entry| {
            let path = entry.path();
            let filename = path.file_name()
                .and_then(|name| name.to_str())
                .unwrap_or("");
            filename.starts_with('m') && filename.ends_with(".svg") && 
            filename[1..filename.len()-4].parse::<i32>().is_ok()
        })
        .collect::<Vec<_>>();
    
    svg_files.sort_by_key(|entry| {
        let path = entry.path();
        let filename = path.file_name()
            .and_then(|name| name.to_str())
            .unwrap_or("");
        filename[1..filename.len()-4].parse::<i32>().unwrap_or(0)
    });
    
    if svg_files.is_empty() {
        return Err("No matching SVG files found".into());
    }
    
    Ok(svg_files)
}

pub fn add_white_background_to_svg(svg_path: &Path) -> Result<(), Box<dyn std::error::Error>> {
    // Read the SVG file
    let contents = fs::read_to_string(svg_path)?;

    // Create XML reader
    let mut reader = Reader::from_str(&contents);
    reader.config_mut().trim_text(false);

    // Create a temporary output file
    let temp_path = svg_path.with_extension("svg.tmp");
    let mut writer = Writer::new_with_indent(fs::File::create(&temp_path)?, b' ', 2);

    // Prepare the white rectangle
    let white_rect = BytesStart::new("rect")
        .with_attributes(vec![
            ("x", "0"),
            ("y", "0"),
            ("width", "100%"),
            ("height", "100%"),
            ("fill", "white"),
        ]);

    let mut inside_svg = false; // Track whether we are inside the <svg> tag

    // Process XML events
    loop {
        match reader.read_event() {
            Ok(Event::Decl(decl)) => {
                // Write the XML declaration
                writer.write_event(Event::Decl(decl))?;
            }
            Ok(Event::DocType(doctype)) => {
                // Write the DOCTYPE
                writer.write_event(Event::DocType(doctype))?;
            }
            Ok(Event::Start(e)) if e.name().as_ref() == b"svg" && !inside_svg => {
                // Write the <svg> tag
                writer.write_event(Event::Start(e.clone()))?;

                // Add the white rectangle as the first child
                writer.write_event(Event::Empty(white_rect.clone()))?;
                inside_svg = true;
            }
            Ok(Event::End(e)) if e.name().as_ref() == b"svg" => {
                // Write the closing </svg> tag
                writer.write_event(Event::End(e))?;
                inside_svg = false;
            }
            Ok(Event::Eof) => break, // End of the file
            Ok(event) => {
                // Write all other events as-is
                writer.write_event(event)?;
            }
            Err(e) => return Err(Box::new(e)),
        }
    }

    // Finalize the writer
    writer.into_inner().sync_all()?;

    // Replace the original file with the modified version
    fs::rename(temp_path, svg_path)?;

    Ok(())
}

pub fn svg_to_png(input_path: &Path, output_path: &Path) -> Result<(), Box<dyn std::error::Error>> {
    // First, add a white background to the SVG
    add_white_background_to_svg(input_path)?;

    // Read the modified SVG file data
    let file_data = fs::read(input_path)?;

    // Create a PixbufLoader with the MIME type for SVG
    let loader = PixbufLoader::with_mime_type("image/svg+xml")
        .map_err(|e| format!("Failed to create PixbufLoader: {e}"))?;

    // Set custom resolution by providing a scale factor
    // For example, scaling 4x increases the resolution proportionally
    loader.set_size(7680, 4320); // Replace with your desired width/height

    // Write the SVG data into the loader
    loader.write(&file_data)?;
    loader.close()?;

    // Extract the rendered Pixbuf
    let pixbuf = loader
        .pixbuf()
        .ok_or("Failed to load Pixbuf from SVG data")?;

    // Save the Pixbuf as a PNG file
    pixbuf.savev(output_path, "png", &[])?;

    Ok(())
}

/// Simply counts how many files match prefix###extension (after filling).
pub fn count_existing_frames(
    dir: &Path,
    extension: &str,
) -> usize {
    fs::read_dir(dir)
        .unwrap()
        .filter_map(Result::ok)
        .filter(|e| {
            let p = e.path();
            p.extension() == Some(OsStr::new(&extension[1..]))
        })
        .count()
}


pub fn fill_missing_frames(
    dir: &Path,
    prefix: &str,
    digits: usize,
) -> Result<usize, Box<dyn std::error::Error>> {
    // 1. Glob all matching files (e.g. "./png_for_video/m*.png")
    let pattern = format!("{}/{}*.png", dir.display(), prefix);
    let mut paths: Vec<PathBuf> = glob(&pattern)?
        .filter_map(Result::ok)
        .collect();

    // 2. Sort by the numeric part of the stem
    paths.sort_by_key(|p| {
        p.file_stem()
         .and_then(|s| s.to_str())
         .and_then(|stem| stem.strip_prefix(prefix))
         .and_then(|num| num.parse::<usize>().ok())
         .unwrap_or(0)
    });

    // 3. Rename each in sequence: m001.png, m002.png, …
    for (i, old_path) in paths.iter().enumerate() {
        let new_name = format!("{prefix}{:0width$}.png", i + 1, width = digits);
        let new_path = dir.join(new_name);
        if old_path != &new_path {
            fs::rename(old_path, &new_path)?;
        }
    }

    // 4. Return total count
    Ok(paths.len())
}

/// Generate a video chunk from PNGs within the specified time range.
pub fn create_video_chunk(
    input_pattern: &str,
    output_segment: &str,
    start_time: u32,
    duration: u32,
    framerate: u32,
) -> Result<(), Box<dyn std::error::Error>> {
    let status = Command::new("ffmpeg")
        .args([
            "-framerate",
            &framerate.to_string(),
            "-i",
            input_pattern,
            "-ss",
            &start_time.to_string(),
            "-t",
            &duration.to_string(),
            "-c:v",
            "libx264",
            "-preset",
            "ultrafast",
            "-pix_fmt",
            "yuv420p",
            output_segment,
        ])
        .status()?;

    if !status.success() {
        return Err(format!("Failed to create video chunk: {}", output_segment).into());
    }
    Ok(())
}

/// Create a file listing video segments for concatenation.
pub fn create_concat_file(
    segment_files: &[String], // Updated to match main.rs
    concat_file: &str,
) -> Result<(), Box<dyn std::error::Error>> {
    let mut file = File::create(concat_file)?;
    for segment in segment_files {
        writeln!(file, "file '{}'", segment)?;
    }
    Ok(())
}

/// Concatenate video segments into a final video.
pub fn concatenate_segments(
    concat_file: &str,
    output_video: &str,
) -> Result<(), Box<dyn std::error::Error>> {
    let status = Command::new("ffmpeg")
        .args([
            "-f",
            "concat",
            "-safe",
            "0",
            "-i",
            concat_file,
            "-c:v",
            "libx264",
            "-preset",
            "ultrafast",
            "-pix_fmt",
            "yuv420p",
            output_video,
        ])
        .status()?;

    if !status.success() {
        return Err(format!("Failed to concatenate video segments into: {}", output_video).into());
    }
    Ok(())
}
