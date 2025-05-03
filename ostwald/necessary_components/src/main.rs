mod svg_utils;

use std::path::Path;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let input_dir = "./"; // Directory with SVGs
    let output_dir = "./png_for_video"; // PNG output dir
    let input_pattern = "./png_for_video/m%3d.png"; // PNG pattern for globbing
    let concat_file = "./concat.txt";
    let final_video = "./png_for_video/video.mp4";
    let segment_duration = 10;
    let framerate = 6;

    if !Path::new(output_dir).exists() {
        std::fs::create_dir(output_dir)?;
    }

    // Step 1: Convert SVGs to PNGs
    let svg_files = svg_utils::load_sorted_svgs(input_dir)?;
    for entry in svg_files {
        let input_path = entry.path();
        let file_stem = input_path.file_stem().and_then(|s| s.to_str()).unwrap_or("output");
        let output_path = Path::new(output_dir).join(format!("{file_stem}.png"));
        println!("Converting {:?} to {:?}", input_path, output_path);
        if let Err(e) = svg_utils::svg_to_png(&input_path, &output_path) {
            eprintln!("Failed to convert {:?}: {}", input_path, e);
        }
    }

    println!("Finished converting SVG files to PNGs!");

    // 2. Fill any missing gaps and get total frames
    let dir = Path::new(output_dir);
    let total_frames = svg_utils::fill_missing_frames(dir, "m", 3)?;
    if total_frames == 0 {
        eprintln!("No PNG frames found in {}", output_dir);
        return Ok(());
    }
    
    let total_duration = total_frames as f32 / framerate as f32;
    let num_segments = (total_duration / segment_duration as f32).ceil() as usize;
    
    println!(
        "Total frames: {}, Total video duration: {:.2} seconds, Total segments: {}",
        total_frames, total_duration, num_segments
    );

    // 3. Create chunks
    let mut start_time = 0;
    let mut segments = Vec::new();
    for i in 0..num_segments {
        let segment_file = format!("{}/segment{}.mp4", output_dir, i + 1);
        svg_utils::create_video_chunk(
            input_pattern,
            &segment_file,
            start_time,
            segment_duration,
            framerate,
        )?;
        println!("Created {}", segment_file);
        segments.push(segment_file);
        start_time += segment_duration;
    }

    // Step 5: Concatenate all chunks
    svg_utils::create_concat_file(&segments, concat_file)?;
    svg_utils::concatenate_segments(concat_file, final_video)?;

    println!("Final video created at: {}", final_video);
    Ok(())
}
