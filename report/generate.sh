spps=( 1 4 8 16 32 )

samplermodes=( all_lights uniform importance importance_no_distance distance distance_squared )

EXEC="./build/apps/VI-RT-V4-PathTracing"
OUTPUT_PATH="./report/outputs"

mkdir -p $OUTPUT_PATH

for spp in "${spps[@]}"; do
  for mode in "${samplermodes[@]}"; do
    output_file="${OUTPUT_PATH}/output${spp}_${mode}.ppm"
    hyperfine_file="${OUTPUT_PATH}/hyperfine_${spp}_${mode}.json"
    if [[ -e "$output_file" ]]; then
      echo "Skipping $output_file (already exists)."
    else
      hyperfine "$EXEC $output_file $spp $mode" --export-json "$hyperfine_file" --ignore-failure
    fi
    ffmpeg -y -f image2 -i "$output_file" "${output_file%.ppm}.png" > /dev/null 2>&1
  done
done

for spp in "${spps[@]}"; do
  for mode in "${samplermodes[@]}"; do
    if [[ "$mode" == "all_lights" ]]; then
      continue
    fi

    img="${OUTPUT_PATH}/output${spp}_${mode}.png"
    img_ref="${OUTPUT_PATH}/output${spp}_all_lights.png"

    echo "Comparing $img with $img_ref"
    raytracer-rmse.exe "$img" "$img_ref" 0.5 "${OUTPUT_PATH}/rmse${spp}_${mode}.png" --output-to-json "${OUTPUT_PATH}/rmse${spp}_${mode}.json"
  done
done