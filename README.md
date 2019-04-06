# parseplate
A script for parsing data from a Biolog Gen III MicroPlate.

## Usage
parseplate: usage: parseplate file1 file2 output-formats
run "parseplate -h" to display this usage message.

Arguments:
  file1: the first input file.
  file2: the second input file. Specify as "-" to run in 1-file mode.
  output-formats: one or more space-separated output formats

Valid output formats: 
  Data formats (will output both files in 2-file mode): 
    raw_table: outputs a human-readable table of raw values and +/- results
    raw_list: outputs a human-readable detailed list of values adjusted for negative control and +/- results
    table: outputs a human-readable table of values adjusted for negative control and +/- results
    list: outputs a human-readable detailed list of raw values and +/- results
    raw_csv_list: outputs a detailed CSV list of raw values and +/- results
    csv_table: outputs a human-readable detailed list of values adjusted for negative control
    csv_list: outputs a CSV detailed list of values adjusted for negative control and +/- results
    csv_results: outputs a CSV table of +/- results
    raw_csv_table: outputs a CSV table of raw values
    results: outputs a human-readable table of +/- results
  Diff formats (can only be used in 2-file mode): 
    raw_table_diff: outputs a human-readable table of differences in raw values and +/- results
    raw_list_diff: outputs a human-readable detailed list of differences in raw values and +/- results
    table_diff: outputs a human-readable table of differences in values adjusted for negative control
    list_diff: outputs a human-readable detailed list of differences in values adjusted for negative control and +/- results
    raw_csv_list_diff: outputs a CSV detailed list of differences in raw values and +/- results
    csv_table_diff: outputs a CSV table of differences in values adjusted for negative control
    csv_list_diff: outputs a CSV detailed list of differences in values adjusted for negative control and +/- results
    csv_results_diff: outputs a CSV table of differences in +/- results
    raw_csv_table_diff: outputs a CSV table of differences in raw values
    results_diff: outputs a human-readable table of differences in +/- results
  all: outputs all output fomats listed above (or all data formats in 1-file mode)
  
  ## To build
  
  To build parseplate, simply compile it with a C++ compiler. No additional libraries are needed.
