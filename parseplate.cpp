#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <limits>

using namespace std;

void readFile(string fname, string data[13], int vals_raw[8][12], float& temperature)
{
	ifstream f;
	f.open(fname);
	if(!f.is_open())
	{
		throw runtime_error("parseplate: fatal: failed to open file "+fname);
	}
	string line;
	int i = 0;
	getline(f, line);
	while(line != "")
	{
		data[i] = line;
		i++;
		getline(f, line);
	}
	for(int i = 3; i<11; i++)
	{
		istringstream line(data[i]);
		if(i == 3)
		{

			line >> temperature; //save temperature
			while(line.fail()) //bad input?
			{
	        line.clear(); //clear the fail status bits
	        line.ignore(numeric_limits<streamsize>::max(), '\n'); //get rid of the bad input
	        line >> temperature; //try again
	    	}
		}
		for(int j = 0; j<12; j++)
		{
			int val;
			string val_raw;
			string val_int;
			line >> val_raw; //get raw string value for number
			while(line.fail()) //bad input?
			{
	        line.clear(); //clear the fail status bits
	        line.ignore(numeric_limits<streamsize>::max(), '\n'); //get rid of the bad input
	        line >> val_raw; //try again
	    	}
	    	val_int.push_back(val_raw[0]); //ugly hack to convert float with known precision to int without ever storing it as a float and risking roundoff error
	    	val_int.append(val_raw.substr(2, string::npos));
	    	while(val_int.size() < 5)
	    	{
	    		val_int = val_int+"0";
	    	}
	    	val = stoi(val_int); //end ugly hack
			vals_raw[i-3][j] = val;
		}

	}
	f.close();
}

void print_tabular(int values[8][12], bool results[8][12], int FIELD_WIDTH)
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	string header = "   ";
	for(int i = 1; i<13; i++)
	{
		header = header+to_string(i);
		for(int j = 0; j<FIELD_WIDTH-to_string(i).size(); j++) //spacing out the header so it lines up with the data
		{
			header = header+" ";
		}
	}
	cout << header << endl;
	for(int i = 0; i<8; i++)
	{
		cout << letters[i]+" " << flush;
		for(int j = 0; j<12; j++)
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat);
			string outval(outval_cstr);
			if(outval[0] != '-')
			{
				outval = " "+outval; //left-pad a space on positive values
			}
			cout << outval << "(" << (results[i][j]?"+":"-") << ")  " << flush;
		}
		cout << endl;
	}
}

void print_csv(int values[8][12])
{
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++)
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat);
			string outval(outval_cstr);
			cout << outval << (j<11?",":"") << flush;
		}
		cout << endl;
	}
}

void print_csv_results(bool values[8][12])
{
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++)
		{
			cout << (values[i][j]?"+":"-") << (j<11?",":"") << flush;
		}
		cout << endl;
	}
}

void print_list_csv(int values[8][12], bool results[8][12], string well_contents[8][12], string platenum)
{
	if(platenum.size() > 0)
	{
		cout << "file,row,column,well content,value,result" << endl;
	}
	else
	{
		cout << "row,column,well content,value,result" << endl;
	}
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++) //iterate over values
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat); 
			string outval(outval_cstr);
			cout << platenum << letters[i] << "," << to_string(j+1) << "," << well_contents[i][j] << "," <<  outval << "," << (results[i][j]?"+":"-") << endl;
		}
	}
}

void print_list(int values[8][12], bool results[8][12], string well_contents[8][12])
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++) //iterate over values
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat); 
			string outval(outval_cstr);
			cout << letters[i] << to_string(j+1) << " (" << well_contents[i][j] << "): " << outval << "(" << (results[i][j]?"+":"-") << ")" << endl;
		}
	}
}

string parse_result(bool result1, bool result2)
{
	if(result1 && result2)
	{
		return "+";
	}
	else if(!result1 && !result2)
	{
		return "-";
	}
	else if(!result1 && result2)
	{
		return "^";
	}
	else if(result1 && !result2) //this should not happen
	{
		return "V";
	}
	else
	{
		return "."; //this literally can't happen, included to prevent compiler warning
	}
}

void print_tabular_diff(int values[8][12], bool results[8][12], int values2[8][12], bool results2[8][12], int FIELD_WIDTH)
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	string header = "   ";
	for(int i = 1; i<13; i++)
	{
		header = header+to_string(i);
		for(int j = 0; j<FIELD_WIDTH-to_string(i).size(); j++) //spacing out the header so it lines up with the data
		{
			header = header+" ";
		}
	}
	cout << header << endl;
	for(int i = 0; i<8; i++)
	{
		cout << letters[i]+" " << flush;
		for(int j = 0; j<12; j++)
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values[i][j]-values2[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat);
			string outval(outval_cstr);
			if(outval[0] != '-')
			{
				outval = " "+outval; //left-pad a space on positive values
			}
			cout << outval << "(" << parse_result(results[i][j], results2[i][j]) << ")  " << flush;
		}
		cout << endl;
	}
}

void print_csv_diff(int values[8][12], int values2[8][12])
{
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++)
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values2[i][j]-values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat);
			string outval(outval_cstr);
			cout << outval << (j<11?",":"") << flush;
		}
		cout << endl;
	}
}

void print_csv_results_diff(bool values[8][12], bool values2[8][12])
{
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++)
		{
			cout << parse_result(values[i][j], values2[i][j]) << (j<11?",":"") << flush;
		}
		cout << endl;
	}
}

void print_results_diff(bool values[8][12], bool values2[8][12])
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	string header = "  ";
	for(int i = 1; i<13; i++)
	{
		header = header+to_string(i)+(i<10?"  ":" ");
	}
	cout << header << endl;
	for(int i = 0; i<8; i++)
	{
		cout << letters[i] << " " << flush;
		for(int j = 0; j<12; j++)
		{
			cout << parse_result(values[i][j], values2[i][j]) << "  " << flush;
		}
		cout << endl;
	}
}

void print_results(bool values[8][12])
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	string header = "  ";
	for(int i = 1; i<13; i++)
	{
		header = header+to_string(i)+(i<10?"  ":" ");
	}
	cout << header << endl;
	for(int i = 0; i<8; i++)
	{
		cout  << i << " " << flush;
		for(int j = 0; j<12; j++)
		{
			cout << (values[i][j]?"+":"-") << "  " << flush;
		}
		cout << endl;
	}
}

void print_list_csv_diff(int values[8][12], bool results[8][12], int values2[8][12], bool results2[8][12], string well_contents[8][12])
{
	cout << "row,column,well content,value,result" << endl;
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++) //iterate over values
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values2[i][j]-values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat); 
			string outval(outval_cstr);
			cout << letters[i] << "," << to_string(j+1) << "," << well_contents[i][j] << "," <<  outval << "," << parse_result(results[i][j], results2[i][j]) << endl;
		}
	}
}

void print_list_diff(int values[8][12], bool results[8][12], int values2[8][12], bool results2[8][12], string well_contents[8][12])
{
	string letters[8] = {"A","B","C","D","E","F","G","H"}; //letters for rows
	for(int i = 0; i<8; i++)
	{
		for(int j = 0; j<12; j++) //iterate over values
		{
			char outval_cstr[8]; //using c-style string to set float precision w/ format string
			float outfloat = ((values2[i][j]-values[i][j])/10000.0);
			sprintf(outval_cstr, "%.4f", outfloat); 
			string outval(outval_cstr);
			cout << letters[i] << to_string(j+1) << " (" << well_contents[i][j] << "): " << outval << "(" << parse_result(results[i][j], results2[i][j]) << ")" << endl;
		}
	}
}

void printOutFmts()
{
	cout << "Valid output formats: " << endl;
	cout << "  Data formats (will output both files in 2-file mode): " << endl;
	cout << "    raw_table: outputs a human-readable table of raw values and +/- results" << endl;
	cout << "    raw_list: outputs a human-readable detailed list of values adjusted for negative control and +/- results" << endl;
	cout << "    table: outputs a human-readable table of values adjusted for negative control and +/- results" << endl;
	cout << "    list: outputs a human-readable detailed list of raw values and +/- results" << endl;
	cout << "    raw_csv_list: outputs a detailed CSV list of raw values and +/- results" << endl;
	cout << "    csv_table: outputs a human-readable detailed list of values adjusted for negative control" << endl;
	cout << "    csv_list: outputs a CSV detailed list of values adjusted for negative control and +/- results" << endl;
	cout << "    csv_results: outputs a CSV table of +/- results" << endl;
	cout << "    raw_csv_table: outputs a CSV table of raw values" << endl;
	cout << "    results: outputs a human-readable table of +/- results" << endl;
	cout << "  Diff formats (can only be used in 2-file mode): " << endl;
	cout << "    raw_table_diff: outputs a human-readable table of differences in raw values and +/- results" << endl;
	cout << "    raw_list_diff: outputs a human-readable detailed list of differences in raw values and +/- results" << endl;
	cout << "    table_diff: outputs a human-readable table of differences in values adjusted for negative control" << endl;
	cout << "    list_diff: outputs a human-readable detailed list of differences in values adjusted for negative control and +/- results" << endl;
	cout << "    raw_csv_list_diff: outputs a CSV detailed list of differences in raw values and +/- results" << endl;
	cout << "    csv_table_diff: outputs a CSV table of differences in values adjusted for negative control" << endl;
	cout << "    csv_list_diff: outputs a CSV detailed list of differences in values adjusted for negative control and +/- results" << endl;
	cout << "    csv_results_diff: outputs a CSV table of differences in +/- results" << endl;
	cout << "    raw_csv_table_diff: outputs a CSV table of differences in raw values" << endl;
	cout << "    results_diff: outputs a human-readable table of differences in +/- results" << endl;
	cout << "  all: outputs all output fomats listed above (or all data formats in 1-file mode)" << endl;
}

void printUsage()
{
	cout << "parseplate: usage: parseplate file1 file2 output-formats" << endl;
	cout << "run \"parseplate -h\" to display this usage message." << endl;
	cout << endl;
	cout << "Arguments:" << endl;
	cout << "  file1: the first input file." << endl;
	cout << "  file2: the second input file. Specify as \"-\" to run in 1-file mode." << endl;
	cout << "  output-formats: one or more space-separated output formats" << endl;
	cout << endl;
	printOutFmts();
}

void badFmt(bool diff, string arg)
{
	if(diff)
	{
		cout << "parseplate: fatal: diff output format \"" << arg << "\" in 1-file mode" << endl;

	}
	else
	{
		cout << "parseplate: fatal: bad output format \"" << arg << "\"" << endl;
	}
	cout << endl;
	printOutFmts();
}


int main(int argc, char** argv)
{
	int outtype = 0; //1 tabular raw, 2 list raw, 4 tabular adjusted, 8 list adjusted (15 is all, these are bit flags)
	bool single = false;
	if(argc < 2) //no input file
	{
		cout << "parseplate: fatal: no input files specified" << endl;
		cout << endl;
		printUsage();
		
		return 2;
	}
	else if(argc == 2 && (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"?") == 0 || strcmp(argv[1],"--help") == 0)) //help
	{
		printUsage();
		return 0;
	}
	else if(argc == 2 || (argc >= 3 && strcmp(argv[2],"-") == 0)) //1 file only
	{
		single = true;
	}
	if(argc >= 4)
	{
		for(int i = 3; i<argc; i++)
		{
			if(strcmp(argv[i],"all") == 0)
			{
				outtype = (single?1023:1048575); //everything (or everything but diffs in 1-file mode)
				break;
			}
			else if(strcmp(argv[i],"raw_table") == 0)
			{
				outtype = outtype | 1;
			}
			else if(strcmp(argv[i],"raw_list") == 0)
			{
				outtype = outtype | 2;
			}
			else if(strcmp(argv[i],"table") == 0)
			{
				outtype = outtype | 4;
			}
			else if(strcmp(argv[i],"list") == 0)
			{
				outtype = outtype | 8;
			}
			else if(strcmp(argv[i],"raw_csv_list") == 0)
			{
				outtype = outtype | 16;
			}
			else if(strcmp(argv[i],"csv_table") == 0)
			{
				outtype = outtype | 32;
			}
			else if(strcmp(argv[i],"csv_list") == 0)
			{
				outtype = outtype | 64;
			}
			else if(strcmp(argv[i],"csv_results") == 0)
			{
				outtype = outtype | 128;
			}
			else if(strcmp(argv[i],"raw_csv_table") == 0)
			{
				outtype = outtype | 256;
			}
			else if(strcmp(argv[i],"results") == 0)
			{
				outtype = outtype | 512;
			}
			else if(strcmp(argv[i],"raw_table_diff") == 0)
			{
				outtype = outtype | 1024;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"raw_list_diff") == 0)
			{
				outtype = outtype | 2048;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"table_diff") == 0)
			{
				outtype = outtype | 4096;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"list_diff") == 0)
			{
				outtype = outtype | 8192;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"raw_csv_list_diff") == 0)
			{
				outtype = outtype | 16384;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"csv_table_diff") == 0)
			{
				outtype = outtype | 32768;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"csv_list_diff") == 0)
			{
				outtype = outtype | 65536;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"csv_results_diff") == 0)
			{
				outtype = outtype | 131072;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"raw_csv_table_diff") == 0)
			{
				outtype = outtype | 262144;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else if(strcmp(argv[i],"results_diff") == 0)
			{
				outtype = outtype | 524288;
				if(single)
				{
					badFmt(true, argv[i]);
					return 2;
				}
			}
			else
			{
				badFmt(false, argv[i]);
				return 2;
			}
		}
	}
	if(outtype == 0) //no flags set
	{
		outtype = (single?1023:1048575); //everything (or everything but diffs in 1-file mode)
	}
	int vals[8][12]; //values adjusted for negative control value from file 1
	int vals_raw[8][12]; //raw values from file 1
	bool results[8][12]; //positive or negative result only from file 1
	int vals2[8][12]; //values adjusted for negative control value from file 2
	int vals_raw2[8][12]; //raw values from file 2
	bool results2[8][12]; //positive or negative result only from file 2
	string data[13]; //the raw lines from file 1
	string data2[13]; //the raw lines from file 2
	float temperature1 = 0; //temperature from file 1
	float temperature2 = 0; //temperature from file 2
	int FIELD_WIDTH = 12; //field width for output formatting
	string well_contents[8][12] = {{"Negative control", "Dextrin", "D-maltose", "D-trehalose", "D-cellobiose", "Gentiobiose", "Sucrose", "D-turanose", "Stachyose", "Positive control", "pH 6", "pH 5"}, {"D-raffinose", "𝛼-D-lactose", "D-Melibiose", "𝛽-methyl-D-glucoside", "D-salicin", "N-acetyl-D-glucosamine", "N-acetyl-𝛽-D-glucosamine", "N-acetyl-D-galactosamine", "N-acetyl-neuraminic acid", "1% NaCl", "4% NaCl", "8% NaCl"}, {"𝛼-D-glucose", "D-mannose", "D-fructose", "D-galactose", "3-methyl glucose", "D-fucose", "L-fucose", "L-rhamnose", "inosine", "1% sodium lactate", "Fusidic Acid", "D-serine"}, {"D-sorbitol", "D-mannitol", "D-arabitol", "myo-inositol", "Glycerol", "D-glucose-6-phosphate", "D-fructose-6-phosphate", "D-aspartic acid", "D-serine", "Troleandomycin", "Rifamycin SV", "Minocycline"}, {"Gelatin", "Glycyl-L-proline", "L-alanine", "L-arginine", "L-aspartic acid", "L-glutamic acid", "L-histidine", "L-pyroglutamic acid", "L-serine", "Lincomycin", "Guanidine HCl", "Niaproof4"}, {"Pectin", "D-galacturonic acid", "L-galactonic acid lactone", "D-gluconic acid", "D-glucuronic acid", "glucuronamide", "Mucic acid", "Quinic acid", "D-saccharic acid", "Vancomycin", "Tetrazolium violet", "Tetrazolium blue"}, {"p-hydroxy-phenylacetic acid", "methyl pyruvate", "D-lactic acid methyl ester", "L-lactic acid", "Citric acid", "𝛼-keto-glutaric acid", "D-malic acid", "L-malic acid", "bromo-succinic acid", "nalidixic acid", "lithium chloride", "potassium tellurite"}, {"Tween 40", "𝛾-amino-butyric acid", "𝛼-hydroxy-butyric acid", "𝛽-hydroxy-D, L-butyric acid", "𝛼-keto-butyric acid", "acetoacetic acid", "propionic acid", "acetic acid", "formic acid", "aztreonam", "sodium butyrate", "sodium bromate"}};
	try
	{
		readFile(argv[1], data, vals_raw, temperature1);
		if(!single)
		{
			readFile(argv[2], data2, vals_raw2, temperature2);
		}
	}
	catch(exception& err)
	{
		cout << err.what() << endl;
		return 1;
	}
	int neg_ctrl = vals_raw[0][0]; //extract negative control for file 1
	int pos_ctrl = vals_raw[0][9]-vals_raw[0][0]; //extract positive control (and subtract negative control) for file 1
	int neg_ctrl2 = vals_raw2[0][0]; //extract negative control for file 2
	int pos_ctrl2 = vals_raw2[0][9]-vals_raw2[0][0]; //extract positive control (and subtract negative control) for file 2

	for(int i = 0; i<8; i++) //subtract negative control value from all values
	{
		for(int j = 0; j<12; j++)
		{
			vals[i][j] =  vals_raw[i][j]-neg_ctrl;
			if(!single)
			{
				vals2[i][j] =  vals_raw2[i][j]-neg_ctrl2;
			}
		}
	}

	for(int i = 0; i<8; i++) //determine positive/negative
	{
		for(int j = 0; j<12; j++)
		{
			bool result;
			bool result2;
			if(j<9) //plate columns 1-9
			{
				result = vals[i][j] > 0;
				if(!single)
				{
					result2 = vals2[i][j] > 0;
				}
			}
			else //plate columns 10-12
			{
				result = !(vals[i][j] < (pos_ctrl/2.0));
				if(!single)
				{
					result2 = !(vals2[i][j] < (pos_ctrl2/2.0));
				}
			}
			results[i][j] =  result;
			if(!single)
			{
				results2[i][j] =  result2;
			}
		}
	}


if(outtype & 1)
{
	if(outtype != 1)
	{
		cout << "Raw Values: " << endl;
	}
	if(!single)
	{
		cout << argv[1] << ": " << endl;
	}
	print_tabular(vals_raw, results, 12);
	if(!single)
	{
		cout << endl << argv[2] << ": " << endl;
		print_tabular(vals_raw2, results2, 12);
	}
	cout << endl;
}
if(outtype & 2)
{
	if(outtype != 2)
	{
		cout << "Detailed Raw Values: " << endl;
	}
	if(!single)
	{
		cout << argv[1] << ": " << endl;
	}
	print_list(vals_raw, results, well_contents);
	if(!single)
	{
		cout << endl << argv[2] << ": " << endl;
		print_list(vals_raw2, results2, well_contents);
	}
	cout << endl;
}
if(outtype & 4)
{
	if(outtype != 4)
	{
		cout << "Values Adjusted for Negative Control: " << endl;
	}
	if(!single)
	{
		cout << argv[1] << ": " << endl;
	}
	print_tabular(vals, results, 12);
	if(!single)
	{
		cout << endl << argv[2] << ": " << endl;
		print_tabular(vals2, results2, 12);
	}
	cout << endl;
}
if(outtype & 8)
{
	if(outtype != 8)
	{
		cout << "Detailed Values Adjusted for Negative Control: " << endl;
	}
	if(!single)
	{
		cout << argv[1] << ": " << endl;
	}
	print_list(vals, results, well_contents);
	if(!single)
	{
		cout << endl << argv[2] << ": " << endl;
		print_list(vals2, results2, well_contents);
	}
	cout << endl;
}
if(outtype & 512)
{
	if(outtype != 512)
	{
		cout << "Results: " << endl;
	}
	if(!single)
	{
		cout << argv[1] << ": " << endl;
	}
	print_results(results);
	if(!single)
	{
		cout << endl << argv[2] << ": " << endl;
		print_results(results2);
	}
	cout << endl;
}
if(outtype & 16)
{
	if(outtype != 16)
	{
		cout << "Detailed Raw Values CSV: " << endl;
	}
	print_list_csv(vals_raw, results, well_contents, (single?"":"1,"));
	if(!single)
	{
		print_list_csv(vals_raw2, results2, well_contents, "2,");
	}
	cout << endl;
}
if(outtype & 32)
{
	if(outtype != 32)
	{
		cout << "Values Adjusted for Negative Control CSV: " << endl;
	}
	print_csv(vals);
	if(!single)
	{
		cout << endl;
		cout << endl;
		print_csv(vals2);
	}
	cout << endl;
}
if(outtype & 64)
{
	if(outtype != 64)
	{
		cout << "Detailed Values Adjusted for Negative Control CSV: " << endl;
	}
	print_list_csv(vals, results, well_contents, (single?"":"1,"));
	if(!single)
	{
		print_list_csv(vals2, results2, well_contents, "2,");
	}
	cout << endl;
}
if(outtype & 128)
{
	if(outtype != 128)
	{
		cout << "Results CSV: " << endl;
	}
	print_csv_results(results);
	if(!single)
	{
		cout << endl;
		cout << endl;
		print_csv_results(results2);
	}
	cout << endl;
}
if(outtype & 256)
{
	if(outtype != 256)
	{
		cout << "Raw Values CSV: " << endl;
	}
	print_csv(vals_raw);
	if(!single)
	{
		cout << endl;
		cout << endl;
		print_csv(vals_raw2);
	}
	cout << endl;
}
if(outtype & 1024)
{
	if(outtype != 1024)
	{
		cout << "Diff Raw Values: " << endl;
	}
	print_tabular_diff(vals_raw, results, vals_raw2, results2, 12);
	cout << endl;
}
if(outtype & 2048)
{
	if(outtype != 2048)
	{
		cout << "Diff Detailed Raw Values: " << endl;
	}
	print_list_diff(vals_raw, results, vals_raw2, results2, well_contents);
	cout << endl;
}
if(outtype & 4096)
{
	if(outtype != 4096)
	{
		cout << "Diff Values Adjusted for Negative Control: " << endl;
	}
	print_tabular_diff(vals, results, vals2, results2, 12);
	cout << endl;
}
if(outtype & 8192)
{
	if(outtype != 8192)
	{
		cout << "Diff Detailed Values Adjusted for Negative Control: " << endl;
	}
	print_list_diff(vals, results, vals2, results2, well_contents);
	cout << endl;
}
if(outtype & 16384)
{
	if(outtype != 16384)
	{
		cout << "Diff Detailed Raw Values CSV: " << endl;
	}
	print_list_csv_diff(vals_raw, results, vals_raw2, results2, well_contents);
	cout << endl;
}
if(outtype & 32768)
{
	if(outtype != 32768)
	{
		cout << "Diff Values Adjusted for Negative Control CSV: " << endl;
	}
	print_csv_diff(vals, vals2);
	cout << endl;
}
if(outtype & 65536)
{
	if(outtype != 65536)
	{
		cout << "Diff Detailed Values Adjusted for Negative Control CSV: " << endl;
	}
	print_list_csv_diff(vals, results, vals2, results2, well_contents);
	cout << endl;
}
if(outtype & 131072)
{
	if(outtype != 131072)
	{
		cout << "Diff Results CSV: " << endl;
	}
	print_csv_results_diff(results, results2);
	cout << endl;
}
if(outtype & 262144)
{
	if(outtype != 262144)
	{
		cout << "Diff Raw Values CSV: " << endl;
	}
	print_csv_diff(vals_raw, vals_raw2);
	cout << endl;
}
if(outtype & 524288)
{
	if(outtype != 524288)
	{
		cout << "Diff Results: " << endl;
	}
	print_results_diff(results, results2);
	cout << endl;
}

	return 0;
}