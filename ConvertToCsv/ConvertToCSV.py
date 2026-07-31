import csv
# caution ai generated code

def convert_edge_list_to_csv(input_file: str, output_file: str) -> None:
    with open(input_file, "r", encoding="utf-8") as infile, \
         open(output_file, "w", newline="", encoding="utf-8") as outfile:

        writer = csv.writer(outfile)
        writer.writerow(["source", "target"])

        for line_number, line in enumerate(infile, start=1):
            line = line.strip()

            if not line:
                continue

            values = line.split()

            if len(values) != 2:
                print(f"Skipping invalid line {line_number}: {line}")
                continue

            source, target = values
            writer.writerow([source, target])


convert_edge_list_to_csv("graph.txt", "graph.csv")