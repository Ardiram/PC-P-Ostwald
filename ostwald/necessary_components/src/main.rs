mod svg_utils;

use std::path::Path;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let input_dir = "./"; // Specify the input directory containing the SVG files
    let output_dir = "./png_for_video"; // Specify the output directory for PNG files
    let input_pattern = "./png_for_video/m%3d.png"; // Replace with the actual pattern of your PNG files
    let concat_file = "./concat.txt";
    let final_video = "./png_for_video/video.mp4";
    let segment_duration = 10; // Duration of each segment in seconds
    let framerate = 6; // Set desired frame rate (e.g., 6 FPS)

    // Ensure the output directory exists
    if !Path::new(output_dir).exists() {
        std::fs::create_dir(output_dir)?;
    }

    // Load and sort SVG files from the input directory
    let svg_files = svg_utils::load_sorted_svgs(input_dir)?;

    // Process each SVG file
    for entry in svg_files {
        let input_path = entry.path();

        let file_stem = input_path.file_stem().and_then(|s| s.to_str()).unwrap_or("output");
        let output_path = Path::new(output_dir).join(format!("{file_stem}.png"));

        println!("Converting {:?} to {:?}", input_path, output_path);

        // Convert the SVG to PNG
        if let Err(e) = svg_utils::svg_to_png(&input_path, &output_path) {
            eprintln!("Failed to convert {:?}: {}", input_path, e);
        }
    }

    println!("Finished converting SVG files to PNGs!");

    // Step 4: Generate smaller video chunks
    let total_frames = std::fs::read_dir(output_dir)?
        .filter(|entry| entry.as_ref().unwrap().path().extension() == Some("png".as_ref()))
        .count();
    let total_duration = (total_frames as f32) / (framerate as f32); // Duration in seconds
    let num_segments = (total_duration / segment_duration as f32).ceil() as usize; // Total number of segments

    println!(
        "Total frames: {}, Total video duration: {:.2} seconds, Total segments: {}",
        total_frames, total_duration, num_segments
    );

    let mut start_time = 0;
    let mut segments = Vec::new();

    for i in 0..num_segments {
        let segment_file = format!("./png_for_video/segment{}.mp4", i + 1);

        match svg_utils::create_video_chunk(
            input_pattern,
            &segment_file,
            start_time,
            segment_duration,
            framerate,
        ) {
            Ok(_) => {
                println!("Created video chunk: {}", segment_file);
                segments.push(segment_file);
                start_time += segment_duration;
            }
            Err(e) => {
                println!(
                    "Failed to create video chunk starting from {} seconds: {}",
                    start_time, e
                );
                break;
            }
        }
    }

    // Step 5: Create concat file
    svg_utils::create_concat_file(&segments, concat_file)?;

    // Step 6: Concatenate segments into the final video
    svg_utils::concatenate_segments(concat_file, final_video)?;

    println!("Final video created at: {}", final_video);

    Ok(())
}
